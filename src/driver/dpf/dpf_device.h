#ifndef LDPF4_DPF_DEVICE_H
#define LDPF4_DPF_DEVICE_H

#include <libusb.h>
#include <unitypes.h>

#define DPF_BYTE_PRE_PIXEL 2

#define DPF_ERROR_DEVICE_NOT_SUPPORTED 1

typedef struct dpf_device_t dpf_device_t;

typedef dpf_device_t* dpf_device_ptr_t;

/**
 * Create a new DPF device
 *
 * @param device usb device
 * @param new_device dpf device handle
 * @return if success, returns 0
 */
int32_t dpf_device_open(libusb_device *device, dpf_device_ptr_t *new_device);

/**
 * Ask the screen it's dimension and re-create the framebuffer
 *
 * @param device DPF device
 * @return if success, returns 0
 */
int32_t dpf_device_acquire_dimensions(dpf_device_ptr_t device);

/**
 * Set back-light brightness
 *
 * @param device DPF device
 * @param brightness 0-7, 0 means off
 * @return if success, returns 0
 */
int32_t dpf_device_set_brightness(dpf_device_ptr_t device, uint32_t brightness);

/**
 * Get current brightness level (cached in context)
 *
 * @param device DPF device
 * @return brightness level
 */
int32_t dpf_device_get_brightness(dpf_device_ptr_t device);

/**
 * Set default background color, the color when screen flushing uses.
 *
 * @param device DPF device
 * @param r red (0 - 255)
 * @param g green (0 - 255)
 * @param b blue (0 - 255)
 */
void dpf_device_set_background_color(dpf_device_ptr_t device, uint8_t r, uint8_t g, uint8_t b);

/**
 * Get current background color
 * @param device DPF device
 * @return RGBa value in a whole uint
 */
uint32_t *dpf_device_get_background_color(dpf_device_t *device);

int32_t dpf_device_bulk_transfer(dpf_device_t *device, const uint8_t *buffer, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

int32_t dpf_device_flush(dpf_device_t *device, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

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