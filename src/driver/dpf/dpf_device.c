#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dpf_device.h"
#include "warp_scsi.h"

#include "../../utils/common_types.h"
#include "../../utils/log.h"

#define USB_COMMAND_BLIT         0x12
#define USB_COMMAND_SET_PROPERTY 0x01

struct dpf_device {
    libusb_context *usbContext;
    libusb_device_handle *usbDevice;

    // Basic info
    unsigned int screenWidth;
    unsigned int screenHeight;

    unsigned long bufferSize;
    unsigned char *buffer;

    // Properties
    int brightness;
};

static unsigned char global_buffer_exec_cmd[16] = {
        0xcd, 0x00, 0x00, 0x00,
        0x00, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
};

dpf_device *dpf_device_init_auto() {
    dpf_device *device = (struct dpf_device *) malloc(sizeof(dpf_device));
    if (!device) {
        log_error("Could not allocate memory for DPF device context.");
        return NULL;
    }

    libusb_context *context = NULL;
    if (libusb_init(&context) < 0) {
        log_error("Could not initialize USB context.");
        return NULL;
    }
    libusb_set_debug(context, 3);

    libusb_device_handle *targetDevice = libusb_open_device_with_vid_pid(context, 0x1908, 0x0102);
    if (!targetDevice) {
        log_error("Usb device open failed.");
        return NULL;
    }

    device->usbContext = context;
    device->usbDevice = targetDevice;

    log_debug("Dpf device created.");

    return device;
}

int dpf_device_acquire_dimensions(dpf_device *device) {
//#define BUFFER_GET_WIDTH(buffer) (buffer[0] | (buffer[1] << 8U))
//#define BUFFER_GET_HEIGHT(buffer) (buffer[2] | (buffer[3] << 8U))

    libusb_device_handle *deviceHandle = device->usbDevice;

    static unsigned char buffer[5];
    static unsigned char command[16] = {
            0xcd, 0x00, 0x00, 0x00,
            0x00, 0x02, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
    };

    command[5] = 0x02;
    if (warp_scsi_read(deviceHandle, command, sizeof(command), buffer, 5) == 0) {
        // Device use short type, do the trick
        unsigned short *data = (unsigned short *) buffer;
        unsigned int width = data[0];
        unsigned int height = data[1];
//       unsigned int width = BUFFER_GET_WIDTH(buffer);
//       unsigned int height = BUFFER_GET_HEIGHT(buffer);

        device->screenWidth = width;
        device->screenHeight = height;

        device->bufferSize = width * height * DPF_BYTE_PRE_PIXEL;
        device->buffer = (unsigned char *) malloc(sizeof(unsigned char) * device->bufferSize);
        if (!device->buffer) {
            log_error("Could not allocate screen buffer.");
            return -1;
        }

        log_debug("Screen dimension read success, screen properties also initialized.");
    } else {
        log_warn("Get device dimensions failed.");
        return -1;
    }

    return 0;
}

#define DPF_SET_USB_COMMAND(cmd) global_buffer_exec_cmd[6] = cmd
#define DPF_PROPERTY_BRIGHTNESS 0x0001

int dpf_device_set_brightness(dpf_device *device, unsigned int brightness) {
    libusb_device_handle *usbDevice = device->usbDevice;

    int brightness_hint = 0;
    if (brightness < 0) {
        brightness_hint = 1;
        brightness = 0;
    } else if (brightness > 7) {
        brightness_hint = 1;
        brightness = 7;
    }

    if (brightness_hint)
        log_warn("%d is invalid value, brightness for dpf should between 0 and 7.", brightness);


    DPF_SET_USB_COMMAND(USB_COMMAND_SET_PROPERTY);
    unsigned char *command = global_buffer_exec_cmd;

    // Do the trick
    unsigned short *data = (unsigned short *) &command[7];
    data[0] = DPF_PROPERTY_BRIGHTNESS;
    data[1] = brightness;
//    command[7] = 0x01; // PROPERTY_BRIGHTNESS
//    command[8] = 0x00; // PROPERTY_BRIGHTNESS >> 8
//    command[9] = SPLIT_TO_2_BYTE_L(brightness);
//    command[10] = SPLIT_TO_2_BYTE_H(brightness);

    int result = warp_scsi_write(usbDevice, command, sizeof(global_buffer_exec_cmd), NULL, 0);
    if (result < 0) {
        log_error("Failed to send brightness setting command to device.\n");
        return result;
    }
    device->brightness = brightness;
    return result;
}

unsigned long dpf_device_get_buffer_size(dpf_device *device) {
    return device->bufferSize;
}

unsigned char *dpf_device_get_buffer(dpf_device *device) {
    return device->buffer;
}

unsigned int dpf_device_get_brightness(dpf_device *device) {
    return device->brightness;
}

unsigned int dpf_device_screen_width(dpf_device *device) {
    return device->screenWidth;
}

unsigned int dpf_device_screen_height(dpf_device *device) {
    return device->screenHeight;
}

int dpf_device_flush(dpf_device *device, RectTuple *rectTuple) {
    return dpf_device_bulk_transfer(device, device->buffer, rectTuple);
}

int dpf_device_bulk_transfer(dpf_device *device, const unsigned char *buffer, RectTuple *rectTuple) {
#define TRANSFER_GET_DATA_SIZE(pixels) (pixels << 1U) // x2
    libusb_device_handle *deviceHandle = device->usbDevice;

    unsigned long pixels = rect_tuple_width(rectTuple) * rect_tuple_height(rectTuple);
    unsigned long dataSize = TRANSFER_GET_DATA_SIZE(pixels);

//    command[6] = USB_COMMAND_BLIT;
    DPF_SET_USB_COMMAND(USB_COMMAND_BLIT);
    unsigned char *command = global_buffer_exec_cmd;

    unsigned short *params = (unsigned short *) &command[7];
    params[0] = rectTuple->x0;
    params[1] = rectTuple->y0;
    params[2] = rectTuple->x1 - 1;
    params[3] = rectTuple->y1 - 1;

//    command[7] = SPLIT_TO_2_BYTE_L(rectTuple->x0);
//    command[8] = SPLIT_TO_2_BYTE_H(rectTuple->x0);
//    command[9] = SPLIT_TO_2_BYTE_L(rectTuple->y0);
//    command[10] = SPLIT_TO_2_BYTE_H(rectTuple->y0);
//    command[11] = SPLIT_TO_2_BYTE_L(rectTuple->x1 - 1);
//    command[12] = SPLIT_TO_2_BYTE_H(rectTuple->x1 - 1);
//    command[13] = SPLIT_TO_2_BYTE_L(rectTuple->y1 - 1);
//    command[14] = SPLIT_TO_2_BYTE_L(rectTuple->y1 - 1);
    command[15] = 0;

    return warp_scsi_write(deviceHandle, command, sizeof(global_buffer_exec_cmd), (unsigned char *) buffer, dataSize);
}

void dpf_destroy(dpf_device *device) {
    libusb_device_handle *handle = device->usbDevice;
    libusb_context *context = device->usbContext;

    libusb_close(handle);
    if (context) libusb_exit(context);
    free(device);
    log_debug("Dpf Device destroyed.");
}