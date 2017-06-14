#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/kernel/suspend.h>
#include <psp2kern/ctrl.h>
#include <string.h>
#include "ds4bt.h"

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(*x))

static ds4_controller ds4_list[DS4_LIST_MAX_SIZE];
static SceUID ds4_list_mempool = -1;
int ds4_num_connected = 0;

int ds4_controller_list_init(void)
{
	SceSize pool_size = sizeof(ds4_input_report) * DS4_LIST_MAX_SIZE;
	SceKernelHeapCreateOpt opt;
	memset(&opt, 0, sizeof(opt));
	opt.size = sizeof(opt);
	opt.uselock = 0x100;
	opt.field_8 = 0x10000;

	ds4_list_mempool = ksceKernelCreateHeap("ds4vita", pool_size, &opt);
	if (ds4_list_mempool < 0)
		return -1;

	memset(ds4_list, 0, sizeof(ds4_list));
	return 0;
}

int ds4_controller_list_fini(void)
{
	if (ds4_list_mempool < 0) {
		return -1;
	}
	return ksceKernelDeleteHeap(ds4_list_mempool);
}

int ds4_controller_list_insert(unsigned int mac[2])
{
	int i;
	ds4_controller *ds4 = NULL;

	if (ds4_num_connected >= DS4_LIST_MAX_SIZE) {
		return -1;
	}

	for (i = 0; i < DS4_LIST_MAX_SIZE; i++) {
		if (!ds4_list[i].connected) {
			ds4 = &ds4_list[i];
			break;
		}
	}

	ds4->connected = 1;
	//ds4->port = port;
	memcpy(ds4->mac, mac, sizeof(ds4->mac));
	ds4->report = ksceKernelAllocHeapMemory(ds4_list_mempool,
						sizeof(*ds4->report));
	memset(ds4->report, 0, sizeof(*ds4->report));

	ds4_num_connected++;
	if (ds4_num_connected == 1) {
		ksceCtrlRegisterVirtualControllerDriver(&driver);
	}

	return i + 1;
}

int ds4_controller_list_remove(int port)
{
	if (port <= 0 || port > DS4_LIST_MAX_SIZE) {
		return -1;
	}
	ds4_controller *ds4 = &ds4_list[port - 1];
	if (!ds4->connected) {
		return -1;
	}
	ksceKernelFreeHeapMemory(ds4_list_mempool, ds4->report);
	ds4->connected = 0;

	ds4_num_connected--;
	if (!ds4_num_connected) {
	ksceCtrlRegisterVirtualControllerDriver(NULL);
	}

	return 0;
}

int ds4_controller_swap(int port0, int port1)
{
	if (port0 <= 0 || port0 > DS4_LIST_MAX_SIZE)
		return -1;
	if (port1 <= 0 || port1 > DS4_LIST_MAX_SIZE)
		return -2;
	if (port0 == port1)
		return -3;
	if (!ds4_list[port0 - 1].connected) {
		return -4;
	}
	ds4_controller tmp;
	memcpy(&tmp, &ds4_list[port1 - 1], sizeof(ds4_controller));
	memcpy(&ds4_list[port1 - 1], &ds4_list[port0 - 1], sizeof(ds4_controller));
	memcpy(&ds4_list[port0 - 1], &tmp, sizeof(ds4_controller));
	return 0;
}

int ds4_mac_to_port(unsigned int mac[2])
{
	for (int i = 0; i < DS4_LIST_MAX_SIZE; i++) {
		if (!ds4_list[i].connected)
			continue;
		if (ds4_list[i].mac[0] == mac[0] && ds4_list[i].mac[1] == mac[1]) {
			return i + 1;
		}
	}
	return -1;

}

ds4_controller* get_ds4_controller(int port)
{
	if (port <= 0 || port > DS4_LIST_MAX_SIZE) {
		return NULL;
	}
	ds4_controller *ds4 = &ds4_list[port - 1];
	if (!ds4->connected) {
		return NULL;
	}
	return ds4;
}
