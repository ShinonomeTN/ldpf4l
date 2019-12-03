//
// Created by cattenlinger on 7/2/19.
//


#ifndef DPF_DISPLAY_DPF_USB_UTILS_H
#define DPF_DISPLAY_DPF_USB_UTILS_H

#include <libusb.h>

libusb_device *dpf_get_device(libusb_context *context);
void dpf_usb_print_device(libusb_device *device);

#endif //DPF_DISPLAY_DPF_USB_UTILS_H
