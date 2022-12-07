//------------------------------------------------------------------------------
/**
 * @file storage_test.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief storage device test application for odroid-jig (emmc, sd, sata, nvme..)
 * @version 0.1
 * @date 2022-12-07
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <getopt.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//------------------------------------------------------------------------------
#include "storage_test.h"

//------------------------------------------------------------------------------
#define	msg(fmt, args...)	fprintf(stdout,"[STORAGE(%s)] " fmt, __func__, ##args)

typedef enum {false, true}  bool;

//------------------------------------------------------------------------------
// Default sotrage read command (16 Mbytes, block count depends on the device type)
//------------------------------------------------------------------------------
#define READ_TEST_CMD		"dd of=/dev/null bs=16M iflag=dsync,direct oflag=dsync,nocache"
#define	REDIRECTION_CMD		"2<&1"

//------------------------------------------------------------------------------
// ODROID-M1 Device node & speed check filename
//------------------------------------------------------------------------------
#define	DEV_NODE_EMMC	"mmcblk0"
#define	DEV_NODE_SDMMC	"mmcblk1"
#define	DEV_NODE_NVME	"nvme0n1"
#define	DEV_NODE_SATA	"sda"

#define	SATA_SPEED	"/sys/devices/platform/fc800000.sata/ata1/link1/ata_link/link1/sata_spd"
#define	NVME_SPEED	"/sys/devices/platform/3c0800000.pcie/pci0002:20/0002:20:00.0/0002:21:00.0/current_link_speed"

//------------------------------------------------------------------------------
int storage_read_test (char *dev_node, int count)
{
	char	cmd[256];
	FILE	*fp;

	if (access (dev_node, R_OK) == 0) {
		memset (cmd, 0x00, sizeof(cmd));
		sprintf (cmd, "%s count=%d if=%s %s",
				READ_TEST_CMD, count, dev_node, REDIRECTION_CMD);
		// msg ("%s\n", cmd);

		if ((fp = popen(cmd, "r")) != NULL) {
			char *ptr;
			memset (cmd, 0x00, sizeof(cmd));
			while (fgets (cmd, sizeof(cmd), fp) != NULL) {
				// msg ("fgets = %s\n", cmd);
				if ((ptr = strstr (cmd, " s, ")) != NULL) {
					int speed;

					if (strstr (cmd, "GB/s") != NULL)
						speed = (int)(atof(ptr+4) * 1000.);
					else
						speed = atoi(ptr+4);

					msg ("speed = %d MB/s\n", speed);
					pclose(fp);
					return	speed;
				}
				memset (cmd, 0x00, sizeof(cmd));
			}
			pclose(fp);
		}
	}
	return	0;
}

//------------------------------------------------------------------------------
int storage_test (const char *dev_name, char *resp_str)
{
	char dev_node[32];
	int speed = 0, resp_pos = 0, test_blk_cnt = 0;

	memset (dev_node, 0, sizeof(dev_node));
	/* test emmc read speed */
	if        (!strncmp ("emmc",  dev_name, strlen("emmc"))) {
		sprintf (dev_node, "/dev/%s", DEV_NODE_EMMC);
		test_blk_cnt = 5;
	} else if (!strncmp ("nvme",  dev_name, strlen("nvme"))) {
		FILE *fp;
		if ((fp = fopen(NVME_SPEED, "r")) != NULL) {
			if (fgets (resp_str, 100, fp) != NULL)
				resp_pos = strlen (resp_str);
			fclose(fp);
		}
		sprintf (dev_node, "/dev/%s", DEV_NODE_NVME);
		test_blk_cnt = 10;

	} else if (!strncmp ("sata",  dev_name, strlen("sata"))) {
		FILE *fp;
		if ((fp = fopen(SATA_SPEED, "r")) != NULL) {
			if (fgets (resp_str, 100, fp) != NULL)
				resp_pos = strlen (resp_str);
			fclose(fp);
		}
		sprintf (dev_node, "/dev/%s", DEV_NODE_SATA);
		test_blk_cnt = 5;
	} else if (!strncmp ("sdmmc", dev_name, strlen("sdmmc"))) {
		sprintf (dev_node, "/dev/%s", DEV_NODE_SDMMC);
		test_blk_cnt = 1;
	}
	if (resp_pos)
		resp_str[resp_pos-1] = '-';

	speed = storage_read_test (dev_node, test_blk_cnt);
	sprintf (&resp_str[resp_pos], "%d MB/s", speed);
	msg ("%s\n", resp_str);
	return	speed;
}

//------------------------------------------------------------------------------
#if defined( __DEBUG_APP__)
//------------------------------------------------------------------------------
static void print_usage(const char *prog)
{
	printf("Usage: %s [-n]\n", prog);
	puts("  -n --device name.\n"
	);
	exit(1);
}

//------------------------------------------------------------------------------
static const char *OPT_DEVICE_NAME = "emmc";

static void parse_opts (int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "dev_name",  	1, 0, 'N' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "n:", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'n':
			OPT_DEVICE_NAME = optarg;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	char resp_str[100];
	parse_opts (argc, argv);
	storage_test (OPT_DEVICE_NAME, resp_str);
	return 0;
}

//------------------------------------------------------------------------------
#endif	// #if defined( __DEBUG_APP__)
//------------------------------------------------------------------------------
