#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dpf_device.h"
#include "warp_scsi.h"

#include "../../utils/log.h"

#define USB_COMMAND_BLIT         0x12
#define USB_COMMAND_SET_PROPERTY 0x01

struct dpf_device {
    libusb_device_handle *usbDevice;

    // Basic info
    unsigned int screenWidth;
    unsigned int screenHeight;

    unsigned long bufferSize;
    unsigned char *buffer;

    Rgba8 backgroundColor;

    // Properties
    int brightness;
};

byte global_buffer_exec_cmd[16] = {
        0xcd, 0x00, 0x00, 0x00,
        0x00, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
};


int dpf_device_open(libusb_device *device, dpf_device **new_device) {

    struct libusb_device_descriptor descriptor;
    libusb_get_device_descriptor(device, &descriptor);
    int idVendor = descriptor.idVendor;
    int idProduct = descriptor.idProduct;

    if (idVendor != 0x1908 || idProduct != 0x0102) {
        log_fatal("Attempt to open an unsupported device [vendor: %x, product: %x]", idVendor, idProduct);
        return DPF_ERROR_DEVICE_NOT_SUPPORTED;
    }

    libusb_device_handle *handle;

    int result = libusb_open(device, &handle);
    if (result < 0) return result;

    dpf_device *dpf = (dpf_device *) malloc(sizeof(dpf_device));
    dpf->usbDevice = handle;

    log_debug("Dpf device created.");

    *new_device = dpf;

    dpf_device_set_background_color(dpf, 0, 0, 0);

    dpf->buffer = NULL;

    return result;
}

void dpf_device_set_background_color(dpf_device *device, unsigned char r, unsigned char g, unsigned char b) {
    Rgba8 *backgroundColor = &(device->backgroundColor);
    backgroundColor->red = r;
    backgroundColor->green = g;
    backgroundColor->blue = b;

    backgroundColor->alpha = 0xFFu;
}

Rgba8 *dpf_device_get_background_color(dpf_device *device) {
    return &(device->backgroundColor);
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
        if (device->buffer != NULL) free(device->buffer);
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

int dpf_device_flush(dpf_device *device, const RectTuple *rectTuple) {
    return dpf_device_bulk_transfer(device, device->buffer, rectTuple);
}

int dpf_device_bulk_transfer(dpf_device *device, const unsigned char *buffer, const RectTuple *rectTuple) {
#define TRANSFER_GET_DATA_SIZE(pixels) (pixels << 1U) // x2 to short
    libusb_device_handle *deviceHandle = device->usbDevice;

    unsigned long pixels = rect_tuple_width(rectTuple) * rect_tuple_height(rectTuple);
    unsigned long dataSize = TRANSFER_GET_DATA_SIZE(pixels);

//    command[6] = USB_COMMAND_BLIT;
    DPF_SET_USB_COMMAND(USB_COMMAND_BLIT);
    unsigned char *command = global_buffer_exec_cmd;

    unsigned short *params = (unsigned short *) &command[7];
    params[0] = rectTuple->x0;
    params[1] = rectTuple->y0;
    params[2] = rectTuple->x1;
    params[3] = rectTuple->y1;

//    command[7] = SPLIT_TO_2_BYTE_L(rectTuple->x0);
//    command[8] = SPLIT_TO_2_BYTE_H(rectTuple->x0);
//    command[9] = SPLIT_TO_2_BYTE_L(rectTuple->y0);
//    command[10] = SPLIT_TO_2_BYTE_H(rectTuple->y0);
//    command[11] = SPLIT_TO_2_BYTE_L(rectTuple->x1 - 1);
//    command[12] = SPLIT_TO_2_BYTE_H(rectTuple->x1 - 1);
//    command[13] = SPLIT_TO_2_BYTE_L(rectTuple->y1 - 1);
//    command[14] = SPLIT_TO_2_BYTE_L(rectTuple->y1 - 1);
    command[15] = 0;

    log_trace("Sending %dpx (%d Bytes) to device.", pixels, dataSize);

    int result = warp_scsi_write(deviceHandle, command, sizeof(global_buffer_exec_cmd), (unsigned char *) buffer,
                                 dataSize);

    return result < 0 ? result : 0;
}

void dpf_destroy(dpf_device *device) {
    libusb_device_handle *handle = device->usbDevice;
    libusb_close(handle);

    if (device->buffer != NULL) free(device->buffer);
    free(device);
    log_debug("Dpf Device destroyed.");
}


/*
 *
 *
 *
 * */

int dpf_open_screen_device(libusb_device *usbDevice, ll_screen_device *screenDevice) {
    dpf_device *dpf = NULL;
    int result = dpf_device_open(usbDevice, &dpf);

    if (result != 0) return result;

    screenDevice->data = dpf;
    screenDevice->name = "dpf";
    screenDevice->class = &ll_dpf_screen_CLASS;
    screenDevice->colorDepth = 16;

    result = dpf_device_acquire_dimensions(dpf);

    return result;
}

int _flush_impl(ll_screen_device *device, const unsigned int *pixels, const RectTuple *dimension) {
    const unsigned int rWidth = rect_tuple_width(dimension);
    const unsigned int rHeight = rect_tuple_height(dimension);

    dpf_device *dpf = (dpf_device *) (device->data);

    unsigned short *buffer = (unsigned short *) dpf->buffer;

    Rgba8 color;
    const unsigned int lineWidth = dpf->screenWidth;
    const unsigned int rectX = dimension->x0;
    const unsigned int rectY = dimension->y0;

    unsigned long pos = 0;
    for (unsigned int iY = 0; iY < rHeight; iY++) {
        unsigned int canvasY = (rectY + iY) * lineWidth;
        for (unsigned int iX = 0; iX < rWidth; iX++) {

            rgba_8_from_int(&color, pixels[(rectX + iX) + canvasY]);
            if (color.alpha != 255) rgba_8_apply_alpha(&color, &dpf->backgroundColor);

//             attention: Because dpf is 2 bytes per pixel, so here the gap is 2
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL)] = TO_RGB565_H(color);
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL) + 1] = TO_RGB565_L(color);
            unsigned short pixel = rgba_8_to_rgb_565_reverse(&color);
            buffer[pos++] = pixel;
//            buffer[pos++] = rgba_8_to_rgb_565(&color);
        }
    }

    int result = dpf_device_flush(dpf, dimension);
    if (result < 0) log_warn("could not flush data to screen.");
    return result;
}

int _set_backlight_level_impl(ll_screen_device *device, const int level) {
    int result = dpf_device_set_brightness((dpf_device *) (device->data), (unsigned int) level);
    return result;
}

int _get_backlight_level_impl(ll_screen_device *device, int *level) {
    *level = dpf_device_get_brightness((dpf_device *) (device->data));
    return 0;
}

int _set_background_impl(ll_screen_device *device, const Rgba8 *color) {
    dpf_device_set_background_color(((dpf_device *) device->data), color->red, color->green, color->blue);
    return 0;
}

int _get_background_impl(ll_screen_device *device, Rgba8 *color) {
    Rgba8 *c = dpf_device_get_background_color((dpf_device *) (device->data));
    color->red = c->red;
    color->green = c->green;
    color->blue = c->blue;
    color->alpha = 255;
    return 0;
}

int _get_width_impl(ll_screen_device *device, unsigned int *output) {
    *output = dpf_device_screen_width((dpf_device *) (device->data));
    return 0;
}

int _get_height_impl(ll_screen_device *device, unsigned int *output) {
    *output = dpf_device_screen_height((dpf_device *) (device->data));
    return 0;
}

void _destroy_impl(ll_screen_device *device) {
    dpf_destroy((dpf_device *) device->data);
}

/*
 *
 *
 *
 * */

ll_screen_device_CLASS ll_dpf_screen_CLASS = {
        .flush = _flush_impl,
        .get_backlight_level = _get_backlight_level_impl,
        .set_backlight_level = _set_backlight_level_impl,
        .get_background = _get_background_impl,
        .set_background = _set_background_impl,
        .get_width = _get_width_impl,
        .get_height = _get_height_impl,
        .destroy = _destroy_impl
};