#include <libusb.h>
#include <unitypes.h>

#include "../../utils/common_types.h"
#include "../../com/com_screen_device.h"

#ifndef LDPF4_DPF_DEVICE_H
#define LDPF4_DPF_DEVICE_H

#define DPF_BYTE_PRE_PIXEL 2

#define DPF_ERROR_DEVICE_NOT_SUPPORTED 1

typedef struct dpf_device_t dpf_device_t;

int32_t dpf_device_open(libusb_device *device, dpf_device_t **new_device);

// Ask the screen it's dimension
int32_t dpf_device_acquire_dimensions(dpf_device_t *device);

// Set back-light brightness
int32_t dpf_device_set_brightness(dpf_device_t *device, uint32_t brightness);

int32_t dpf_device_get_brightness(const dpf_device_t *device);

void dpf_device_set_background_color(dpf_device_t *device, uint8_t r, uint8_t g, uint8_t b);

Rgba8 *dpf_device_get_background_color(dpf_device_t *device);

int32_t dpf_device_bulk_transfer(dpf_device_t *device, const uint8_t *buffer, const RectTuple *rectTuple);

int32_t dpf_device_flush(dpf_device_t *device, const RectTuple *rectTuple);

// Properties
uint32_t dpf_device_screen_width(const dpf_device_t *device);

uint32_t dpf_device_screen_height(const dpf_device_t *device);

// Device buffer
uint32_t dpf_device_get_buffer_size(const dpf_device_t *device);

uint8_t *dpf_device_get_buffer(const dpf_device_t *device);

void dpf_destroy(dpf_device_t *device);

/*
 *
 *
 *
 * */

#endif // LDPF4_DPF_DEVICE_H