/*
 * (C) Copyright 2009
 * Sean Cross, chumby Industries, sean@chumby.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>
#include <lcd.h>



#define MAX_FLAKES    2048
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define RAND_SIZE 32768
    
static int rand_num[RAND_SIZE];
    
static inline int get_rand_num(void) {
	static int offset = 0;
	int i;
	if(++offset >= RAND_SIZE)
		offset=0;
	i = rand_num[offset];
	return i;
}       
        
struct flake {
	int x, y;
	int angle; 
	int velocity;      
	int active;
};      
        
        
struct globe {
	struct flake	 flakes[MAX_FLAKES];
	int		 plane_w, plane_h;
	char		*flake_count;
	int		 snow_lut[256];
	short		*background;
};  
    
    
    

// The FPS we'll try to run at.
#define FRAME_RATE 100

struct screen {
	int width, height;
	int bpp; 
	short *pixels;
};
            
struct pixel {
    int r, g, b;
};

    


static struct screen scr;
static struct globe gl;
static char flake_buffer[SCREEN_WIDTH*SCREEN_HEIGHT];
static short background[SCREEN_WIDTH*SCREEN_HEIGHT];
static int stopping_snow;
static int draw_frame(struct globe *gl, struct screen *scr);
static int initted = 0;



static void bzero(void *data, int c) {
	memset(data, 0, c);
}

static void reset_timer1(void) {
	// We're using Timer1, which is set for a 3.250 MHz clock.  We want
	// a framerate of 30 fps.  Therefore we should get a callback in
	// 3250000/30 ticks.
	long *TMR_CR1   = (long *)0xd4014028;
	long *TMR_T1_M2 = (long *)0xd401400c;
	long *TMR_IER1  = (long *)0xd4014040;

	*TMR_T1_M2 = (*TMR_CR1) + (3250000/FRAME_RATE);
	*TMR_IER1 = (*TMR_IER1) | 0x4;
}


static int irq_is_running=0;
static int update_snow(void *data) {
	long *TMR_ICR1 = (long *)0xd4014074;

	// Set the timer up first.
	if(!stopping_snow)
		reset_timer1();

	// Don't let us run nested IRQs.
	if(irq_is_running)
		return 0;
	irq_is_running = 1;

	*TMR_ICR1 = (*TMR_ICR1) | 0x4;

	draw_frame(&gl, &scr);

	// Allow us to run an IRQ in the future.
	irq_is_running=0;
}

void set_flake_state(struct screen *scr, struct globe *globe,
                         struct flake *flake, int on) {
	int x1, x2, y1, y2;

	// Set "on" to 1 or -1;
	if(on)
		on=1;
	else
		on=-1;

	// Just divide the x and y coordinates by 2 to get the screen coordinates.
	x2 = x1 = flake->x >> 1;
	y2 = y1 = flake->y >> 1;

	// If the coordinate is odd, then it crosses a boundary.
	if(flake->x&1)
		x2++;
	if(flake->y&1)
		y2++;

	// Ignore it if any pixels are offscreen.
	if(x1 < 0 || x1 >= scr->width
	|| x2 < 0 || x2 >= scr->width
	|| y1 < 0 || y1 >= scr->height
	|| y2 < 0 || y2 >= scr->height)
		return;

	// Rescale the Y to an index into the flake_count buffer.
	if(y1==y2)
		y2 = (y1 *= scr->width);
	else
		y2 = (y1 *= scr->width)+scr->width;

	globe->flake_count[x1+y1]+=on;
	globe->flake_count[x1+y2]+=on;
	globe->flake_count[x2+y1]+=on;
	globe->flake_count[x2+y2]+=on;
}



int move_snow(struct globe *gl, struct screen *scr) {
    int fl;
    struct flake *flake;
    int plane_w = gl->plane_w;
    int plane_h = gl->plane_h;

    // The maximum number of inactive flakes to mark as active per frame.
    int max_mark_active = get_rand_num()&0x07;


    // How many flakes we've marked as active so far.
    int marked_active = 0;


	int active_flakes = 0;


    // Find snow that's active (but off the screen) and deactivate it.
    for(fl=0; fl<MAX_FLAKES; fl++) {
        flake = &gl->flakes[fl];

        // Activate some amount of inactive snow.
        if(!flake->active) {
            if(marked_active < max_mark_active) {
                marked_active++;
                flake->active = 1;
                flake->y = 0;
                flake->x = get_rand_num()%plane_w;
                set_flake_state(scr, gl, flake, 1);
				active_flakes++;
            }
            continue;
        }
        // Deactivate snow that's fallen off the screen.
        if(flake->x < -20 || flake->x > plane_w+20) {
            set_flake_state(scr, gl, flake, 0);
            flake->active = 0;
            continue;
        }
        if( flake->y > plane_h+20 ) {
            set_flake_state(scr, gl, flake, 0);
            flake->active = 0;
            continue;
        }
        if( flake->y < -20 ) {
            set_flake_state(scr, gl, flake, 0);
            flake->active = 0;
            continue;
        }


		active_flakes++;

        // Otherwise, move active flakes.
        set_flake_state(scr, gl, flake, 0);


        // Move a bit.
        {
            int x_move;
            flake->y += get_rand_num()&3;

            x_move = (get_rand_num()&0x07)-3;
            if(x_move == 0x7-3)
                x_move = 0;
            flake->x += x_move;
        }


        set_flake_state(scr, gl, flake, 1);
    }

	//printf("There are %d active flakes\n", active_flakes);

    return 0;
}




int render_scene(struct globe *gl, struct screen *scr) {
    int *fb = (int *)(scr->pixels);
    long *bg = (long *)gl->background;
    char *flake_count_offset = gl->flake_count;
    int pixel=0, pixels=scr->width*scr->height>>1;

    // Go through each pair of pixels and Or-on a pair of snowflakes.
    while(pixel++<pixels) {
        *fb++ = gl->snow_lut[*flake_count_offset++]<<16
              | gl->snow_lut[*flake_count_offset++] | *bg++;
    }

    return 0;
}



static int draw_frame(struct globe *gl, struct screen *scr) {
	move_snow(gl, scr);
	render_scene(gl, scr);
	return 0;
}




int place_initial_flakes(struct globe *gl, struct screen *scr, int count) {
	int flake;
	struct flake *fl;
	printf("Placing %d flakes...\n", count);
	for(flake=0; flake<count; flake++) {
		fl = &gl->flakes[flake];

		fl->active = 1;
		fl->x = get_rand_num()%gl->plane_w;
		fl->y = get_rand_num()%gl->plane_h;
		set_flake_state(scr, gl, fl, 1);
	}   
	printf("All flakes placed\n");
	return 0; 
}       


void ISR_Connect(int int_num, void (*m_func)( void *data), void *data);

int do_snow (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if(!strcmp(argv[1], "init")) {
		int i;
		int part = 2;

		if(initted) {
			printf("Already initted snow\n");
			return 1;
		}
		initted = 1;

		// If the user specified a partition, switch to that part.
		if(argc >= 3 && !strcmp(argv[2], "1"))
			part = 3;

		// Do init.
		scr.width  = SCREEN_WIDTH;
		scr.height = SCREEN_HEIGHT;
		scr.bpp    = 2;
		scr.pixels = (short *)lcd_base;

		// Clear the screen to black.
		printf("Clearing screen to black...\n");
		bzero(scr.pixels, scr.width*scr.height*scr.bpp);


		// Clear out all snow particles as well.
		printf("Clearing gl buffer...\n");
		bzero(&gl,          sizeof(gl));
		gl.plane_w		= scr.width  * 2;
		gl.plane_h		= scr.height * 2;
		gl.flake_count	= flake_buffer;
		gl.background	= background;




		printf("Clearing flake buffer...\n");
		bzero(flake_buffer, sizeof(flake_buffer));


		// Precalculate the snow LUT.
		printf("Precalculating snow LUT...\n");
		for(i=0; i<256; i++) {
			int c = 50*i;
			if(c>255)
				c=255;
			gl.snow_lut[i] = (((c>>3)<<11) | ((c>>2)<<5) | ((c>>3)<<0));
		}




		return 0;
	}

	if(!strcmp(argv[1], "start")) {
		long *ICU_INT_CONF13 = (long *)0xd4282034;
		stopping_snow = 0;

		printf("Placing initial flakes...\n");
		place_initial_flakes(&gl, &scr, 2048);

		// Timer1 sits on IRQ13.  ICU_INT_CONF is therefore ICU_INT_CONF13.
		printf("Setting IRQ handler...\n");
		ISR_Connect(13, update_snow, NULL);

		// Enable the interrupt, with a priority mask of 1.
		printf("Resetting timer 1...\n");
		*ICU_INT_CONF13 = 0x51;
		reset_timer1();

		return 0;
	}

	if(!strcmp(argv[1], "stop")) {
		stopping_snow = 1;
		return 0;
	}

	if(!strcmp(argv[1], "load")) {
		if(argc < 4)
			printf("Usage: %s load [partition] [filename]\n", argv[0]);
		else {
			unsigned long src_len = ~0UL;
			char part_name[4];
			unsigned long load_addr = simple_strtoul(getenv("default_load_addr"), 0, 16);

			char *e2args[] = {
				"ext2load", "mmc", part_name, getenv("default_load_addr"), argv[3],
			};

			// ext2load must have an address that's on an 8-k boundary.
			sprintf(part_name, "0:%s", argv[2]);
			
			if(!do_ext2load(cmdtp, flag, 5, e2args))
				bzero(gl.background, scr.width*scr.height*scr.bpp);

			printf("Unzipping...\n");
			gunzip((void *)gl.background, sizeof(gl.background), (void *)load_addr, &src_len);

			// Precalculate rand LUT.
			printf("Precalculating RAND LUT...\n");
			memcpy(rand_num, (void *)load_addr, sizeof(rand_num));
		}
	}

	if(!strcmp(argv[1], "draw"))
		memcpy(scr.pixels, gl.background, scr.width*scr.height*2);


	if(!strcmp(argv[1], "step"))
		draw_frame(&gl, &scr);

	return 0;
}


U_BOOT_CMD(
	snow,    4,    1,     do_snow,
	"snow   - modify the snow effect\n",
	"[init|step|start|stop|load]\n"
	"    - initialize or step a snow frame\n"
);
