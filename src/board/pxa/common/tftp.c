#include <common.h>
#include <command.h>
#include <net.h>

int do_switch (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *s;
	char *net[2];
	int now, i, original;
	char *hostIP, *clientIP;

#ifdef CONFIG_DRIVER_SMC91111
	net[0] = "smc91111";
#else
	net[0] = NULL;
#endif

#ifdef CONFIG_USB_ETH
	net[1] = "u2o-eth";
#else
	net[1] = NULL;
#endif
	/* get the pre-set host IP and client IP */
	if ((s = getenv("ethact")) != NULL) {

		for (i = 0; i < ARRAY_SIZE(net); i ++)
			if (net[i] && strncmp(s, net[i], strlen(net[i])) == 0) {
				now = i;
				printf("Currently we use %s to download!!\n", net[i]);
				break;
			}

		if (i == ARRAY_SIZE(net))
			now = 0;
	}
	else
		now = 0;

	if (!net[now]) {
		printf ("There is no interface defined!!!\n");
		return -1;
	}

	original = now;
	do {
		/* set the NET to the next interface */
		now += 1;
		if (now >= ARRAY_SIZE(net))
			now = 0;

		if (net[now]) {
			printf("***Set the interface to %s\n", net[now]);
			setenv("ethact", net[now]);
			break;
		}
	
	} while (original != now);

	if (original == now)
		printf("The interface is still %s\n", net[original]);

	/* change the host and client IP to specified addr*/
	if (argc > 1) {
		setenv ("serverip", argv[1]);
		printf("reset host IP to %s\n", argv[1]);

		if (argc > 2) {
			setenv ("ipaddr", argv[2]);
			printf("reset client IP to %s\n\n", argv[2]);
		}
		else
			printf("client IP no change\n\n");
	}
	else
		printf("IP no change\n\n");
	return 0;
}

U_BOOT_CMD(
	switch,	CONFIG_SYS_MAXARGS,	1,	do_switch,
	"switch\t-switch the download by USB or Ethernet, and config conresponsed IP\n",
	"[hostIPaddr] [clientIP addr]\n"
);

extern int check_usb_connection(int);
int do_tftp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *s;
	char *end;
	int ret;
	ulong addr;

	/* check usb connection each time with tftp, if using the
	 * USB over ethernet way
	 */
	if ((s = getenv("ethact")) != NULL
			&& strcmp(s, "u2o-eth") == 0) {
		ret = check_usb_connection(0);
		if (ret) {
			printf("Error, USB not connected!!\n\n");
			return 1;
		}
	}

	/* pre-set load_addr */
	if ((s = getenv("loadaddr")) != NULL) {
		load_addr = simple_strtoul(s, NULL, 16);
	}

	switch (argc) {
	case 1:
		break;

	case 2:
		addr = simple_strtoul(argv[1], &end, 16);
		if (end == (argv[1] + strlen(argv[1])))
			load_addr = addr;
		else
			copy_filename(BootFile, argv[1], sizeof(BootFile));
		break;

	case 3:	load_addr = simple_strtoul(argv[1], NULL, 16);
		copy_filename (BootFile, argv[2], sizeof(BootFile));

		break;

	default: printf ("Usage:\n%s\n", cmdtp->usage);
		show_boot_progress (-80);
		return 1;
	}

	show_boot_progress (80);
	return NetLoop(TFTP);
}

U_BOOT_CMD(
	tftpboot,	CONFIG_SYS_MAXARGS,	1,	do_tftp,
	"tftpboot\t-download image via network using TFTP protocol\n",
	"[loadAddress] [[hostIPaddr:]filename]\n"
);

#if defined(CONFIG_CMD_PING)
int do_ping (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	NetPingIP = string_to_ip(argv[1]);
	if (NetPingIP == 0) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	if (NetLoop(PING) < 0) {
		printf("ping failed; host %s is not alive\n", argv[1]);
		return 1;
	}

	printf("host %s is alive\n", argv[1]);

	return 0;
}

U_BOOT_CMD(
	ping,	2,	1,	do_ping,
	"ping\t- send ICMP ECHO_REQUEST to network host\n",
	"pingAddress\n"
);
#endif
