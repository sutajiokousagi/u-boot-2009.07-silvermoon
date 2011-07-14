/*
 * (C) Copyright 2011
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


/* Marvell nicely defines this for us */
void ISR_Connect(int int_num, void (*m_func)( void *data), void *data);

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
    struct flake   flakes[MAX_FLAKES];
    int            plane_w, plane_h;
    char 		  *flake_count;
    int            snow_lut[256];
	short 		  *background;
};  
    
    
    

// The FPS we'll try to run at.
#define FRAME_RATE 1

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


static long *ICU_INT_CONF13 = (long *)0xd4282034;
static long *ICU_INT_CONF49 = (long *)0xd42820c4;
static long *MFP_102 = (long *)0xd401e198;
static long *GPIO4_PDR = (long *)0xd401910c;
static long *GPIO4_PLR = (long *)0xd4019100;
static long *GPIO4_EDR = (long *)0xd4019148;
static long *GPIO4_PCR = (long *)0xd4019124;
static long *GPIO4_RER = (long *)0xd4019130;
static long *GPIO4_FER = (long *)0xd401913c;
static long *GPIO4_SRER = (long *)0xd401916c;
static long *GPIO4_SFER = (long *)0xd4019184;
static long *GPIO4_APMASK = (long *)0xd401919c;
static long *APBC_GPIO_CLK_RST = (long *)0xd4015008;

static long *TMR_CR1   = (long *)0xd4014028;
static long *TMR_T1_M2 = (long *)0xd401400c;
static long *TMR_IER1  = (long *)0xd4014040;

static void bzero(void *data, int c) {
	memset(data, 0, c);
}

static void reset_timer1(void) {
	// We're using Timer1, which is set for a 3.250 MHz clock.  We want
	// a framerate of 30 fps.  Therefore we should get a callback in
	// 3250000/30 ticks.

	*TMR_T1_M2 = (*TMR_CR1) + (3250000/FRAME_RATE);
	*TMR_IER1 = (*TMR_IER1) | 0x4;
}

static int times[512];
static int time_count = 0;
static int timer_irq_is_running = 0;

static int fire_timer(void *data) {
	int i;

	if (timer_irq_is_running)
		return 0;
	timer_irq_is_running = 1;

	if (time_count)
		printf("There are %d entries in the timer buffer\n", time_count);

	for (i=0; i<time_count; i++) {
		int diff = (times[i]&0x7fffffff);
		printf("Time %d: State %d\n", times[i]&0x7fffffff, !!(times[i]&0x80000000));
	}

	// Reset the number of times
	time_count = 0;

	timer_irq_is_running = 0;
	return 0;
}



static int irq_is_running=0;
static long last_time = 0;
static long this_time;
static int fire_gpio(void *data) {
	// Don't let us run nested IRQs.
	if(irq_is_running)
		return 0;
	irq_is_running = 1;

	this_time = *TMR_CR1;

	times[time_count++] = ((this_time-last_time) & 0x7fffffff) | (((*GPIO4_PLR)&0x40)<<25);

	// Allow us to run an IRQ in the future.
	irq_is_running=0;

	// Clear the edge-detect register, enabling another IRQ to happen
	*GPIO4_EDR = 0x40;

	// Re-queue the timer for sometime in the future
	*TMR_T1_M2 = this_time + (3250000/FRAME_RATE);
	*TMR_IER1 = (*TMR_IER1) | 0x4;
	last_time = this_time;

	return 0;
}

/*
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
    else        y2 = (y1 *= scr->width)+scr->width;

    globe->flake_count[x1+y1]+=on;
    globe->flake_count[x1+y2]+=on;
    globe->flake_count[x2+y1]+=on;
    globe->flake_count[x2+y2]+=on;
}
*/



/*
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
*/




/*
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
*/



/*
static int draw_frame(struct globe *gl, struct screen *scr) {
    move_snow(gl, scr);
    render_scene(gl, scr);
    return 0;
}
*/




/*
int place_initial_flakes(struct globe *gl, struct screen *scr, int count) {
    int flake;
    struct flake *fl;
    for(flake=0; flake<count; flake++) {
        fl = &gl->flakes[flake];
        
        fl->active = 1;
        fl->x = get_rand_num()%gl->plane_w;
        fl->y = get_rand_num()%gl->plane_h;
        set_flake_state(scr, gl, fl, 1);
    }   
    return 0; 
}       
*/



static int do_ir (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if(!strcmp(argv[1], "stop")) {
		stopping_snow = 1;
		return 0;
	}

	if(!strcmp(argv[1], "load")) {
		if(argc < 4)
			printf("Usage: %s load [partition] [filename]\n", argv[0]);
		else {
			unsigned long src_len = ~0UL, dst_len = ~0UL;
			char mem_addr[16];
			char part_name[4];
			char *real_mem_addr;
			static char load_buffer[1024*512];

			char *e2args[] = {
				"ext2load", "mmc", part_name, mem_addr, argv[3],
			};

			// ext2load must have an address that's on an 8-k boundary.
			real_mem_addr = (char *)(((long)load_buffer & 0xfffff000)+0x1000);
			sprintf(mem_addr, "0x%08x", real_mem_addr);

			sprintf(part_name, "0:%s", argv[2]);
			
			if(!do_ext2load(cmdtp, flag, 5, e2args))
				bzero(gl.background, scr.width*scr.height*scr.bpp);

			printf("Unzipping...\n");
			gunzip((void *)gl.background, dst_len, (void *)real_mem_addr, &src_len);

			// Precalculate rand LUT.
			printf("Precalculating RAND LUT...\n");
			memcpy(rand_num, (void *)real_mem_addr, sizeof(rand_num));
		}
	}

	if(!strcmp(argv[1], "install")) {

		// Set MFP_102 to be a GPIO
		*MFP_102 = 0;

		// Make it an input
		*GPIO4_PCR = 0x40;

		// [S]et the IRQ to fire on the [R]ising and [F]alling edges
		*GPIO4_SRER = 0x40;
		*GPIO4_SFER = 0x40;

		// Unmask the interrupt
		*GPIO4_APMASK |= 0x40;

		// GPIO IRQ sits on IRQ49.
		ISR_Connect(49, fire_gpio, NULL);
		ISR_Connect(13, fire_timer, NULL);

		// Enable the interrupt, with a priority mask of 1.
		*ICU_INT_CONF49 = 0x51;
                *ICU_INT_CONF13 = 0x52;

		reset_timer1();

		printf("Installed IR handler\n");

		return 0;
	}

	if(!strcmp(argv[1], "remove")) {
		long *ICU_INT_CONF49 = (long *)0xd42820c4;
		ISR_Connect(49, NULL, NULL);
		*ICU_INT_CONF49 = 0x00;
	}

	return 0;
}


U_BOOT_CMD(
	ir,    4,    1,     do_ir,
	"ir   - set up IR commands\n",
	"[install|remove]\n"
	"    - install or remove the IR handler\n"
);
