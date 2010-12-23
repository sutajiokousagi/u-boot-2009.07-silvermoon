/*
 * (C) Copyright 2002
 * Richard Jones, rjones@nexus-tech.net
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
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <s_record.h>
#include <net.h>
#include <ata.h>
#include <ns16550.h>


/*
void    NS16550_init   (NS16550_t com_port, int baud_divisor);
void    NS16550_putc   (NS16550_t com_port, char c);
char    NS16550_getc   (NS16550_t com_port);
int     NS16550_tstc   (NS16550_t com_port);
void    NS16550_reinit (NS16550_t com_port, int baud_divisor);
*/


#define CP_PORT 2



static int wait_for_serial_data(int port, ulong delay) {
        ulong start = get_timer(0);

        while (!_serial_tstc(port) && get_timer(start) < delay) {
                udelay (100);
        }
	return _serial_tstc(port);
}


int do_cversion (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	char b64_string[25];
	char response_string[4];

	setenv("cversion", "3");

	/* Get the attention of the CP */
	printf("Writing !!!! to port %d\n", CP_PORT);
	_serial_puts("!!!!", CP_PORT);
	printf("Waiting for response...\n");
	wait_for_serial_data(CP_PORT, 2);
	printf("Waiting for port to be clear\n");
	while(_serial_tstc(CP_PORT))
		_serial_getc(CP_PORT);

	printf("Writing HWVR to port %d\n");
	_serial_puts("HWVR", CP_PORT);

	/* Wait 15 msecs for a response */
	wait_for_serial_data(CP_PORT, 15);

	if(!_serial_tstc(CP_PORT)) {
		printf("Timeout while getting chumby hardware version!\n");
		return 1;
	}

	/* Get the initial part of the response */
	for(i=0; i<sizeof(response_string); i++)
		response_string[i] = _serial_getc(CP_PORT);
	if(memcmp(response_string, "HVRS", sizeof(response_string))) {
		printf("Invalid response!  Wanted 'HVRS' got '%c%c%c%c'\n",
			response_string[0], response_string[1],
			response_string[2], response_string[3]);
		return 1;
	}

	for(i=0; i<sizeof(b64_string)-1; i++)
		b64_string[i] = _serial_getc(CP_PORT);
	b64_string[i] = '\0';
	printf("Got response: %s\n", b64_string);
	return 0;
}

U_BOOT_CMD(
	cversion, 1, 1, do_cversion,
	"cversion - get information about chumby hardware version\n",
	"Get information about the hardware version, and store it in the variable 'cversion'\n"
);
