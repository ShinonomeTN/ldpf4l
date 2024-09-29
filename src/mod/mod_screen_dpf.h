#ifndef LDPF4L_MOD_SCREEN_DPF_H
#define LDPF4L_MOD_SCREEN_DPF_H

#include <libusb.h>
#include "lua.h"
#include "../com/com_screen_device.h"

uint32_t dpf_open_screen_device(libusb_device *usbDevice, ll_screen_device *screenDevice);

extern ll_screen_device_CLASS ll_dpf_screen_CLASS;

#endif