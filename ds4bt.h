#ifndef __DS4BT_H__
#define __DS4BT_H__
#include <psp2kern/ctrl.h>

#define DS4_LIST_MAX_SIZE 4

typedef struct ds4_input_report ds4_input_report;
struct ds4_input_report {
	unsigned char report_id;
	unsigned char left_x;
	unsigned char left_y;
	unsigned char right_x;
	unsigned char right_y;

	unsigned char dpad     : 4;
	unsigned char square   : 1;
	unsigned char cross    : 1;
	unsigned char circle   : 1;
	unsigned char triangle : 1;

	unsigned char l1      : 1;
	unsigned char r1      : 1;
	unsigned char l2      : 1;
	unsigned char r2      : 1;
	unsigned char share   : 1;
	unsigned char options : 1;
	unsigned char l3      : 1;
	unsigned char r3      : 1;

	unsigned char ps   : 1;
	unsigned char tpad : 1;
	unsigned char cnt1 : 6;

	unsigned char l_trigger;
	unsigned char r_trigger;

	unsigned char cnt2;
	unsigned char cnt3;

	unsigned char battery;

	signed short accel_x;
	signed short accel_y;
	signed short accel_z;

	union {
		signed short roll;
		signed short gyro_z;
	};
	union {
		signed short yaw;
		signed short gyro_y;
	};
	union {
		signed short pitch;
		signed short gyro_x;
	};

	unsigned char unk1[5];

	unsigned char battery_level : 4;
	unsigned char usb_plugged   : 1;
	unsigned char headphones    : 1;
	unsigned char microphone    : 1;
	unsigned char padding       : 1;

	unsigned char unk2[2];
	unsigned char trackpadpackets;
	unsigned char packetcnt;

	unsigned int finger1_id        : 7;
	unsigned int finger1_activelow : 1;
	unsigned int finger1_x         : 12;
	unsigned int finger1_y         : 12;

	unsigned int finger2_id        : 7;
	unsigned int finger2_activelow : 1;
	unsigned int finger2_x         : 12;
	unsigned int finger2_y         : 12;

} __attribute__((packed, aligned(32)));

typedef struct ds4_controller {
	int connected;
	//int port;
	unsigned int mac[2];
	ds4_input_report *report;
} ds4_controller;

extern SceCtrlVirtualControllerDriver driver;
extern int ds4_num_connected;

int ds4_send_0x11_report_with_custom_rgb(unsigned int mac0, unsigned int mac1, SceUInt8 r, SceUInt8 g, SceUInt8 b);

int ds4_controller_list_init(void);
int ds4_controller_list_fini(void);
int ds4_controller_list_insert(unsigned int mac[2]);
int ds4_controller_list_remove(int port);
int ds4_controller_swap(int port0, int port1);
int ds4_mac_to_port(unsigned int mac[2]);
ds4_controller* get_ds4_controller(int port);

#endif
