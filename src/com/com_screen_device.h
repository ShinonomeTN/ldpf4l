//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_COM_SCREEN_DEVICE_H
#define LDPF4L_COM_SCREEN_DEVICE_H

#include <unitypes.h>

#include "../utils/common_types.h"

struct ll_screen_device_CLASS;
struct ll_screen_device;

typedef struct ll_screen_device_CLASS {

    /*
     *
     * All device accept an array of data, and a rectangle described a drawing area
     *
     * */
    int32_t (*flush)(struct ll_screen_device *device, const uint32_t *pixels, const RectTuple *dimension);

    /*
     *
     * Normally, screen has a attribute to describe how bright it is and can be tune.
     *
     * */
    int32_t (*set_backlight_level)(struct ll_screen_device *device, int32_t level);

    int32_t (*get_backlight_level)(struct ll_screen_device *device, int32_t *output);

    /*
     *
     * Screens maybe not transparent, maybe it's, but we don't known if we have no instance of a screen.
     *
     * */
    int32_t (*set_background)(struct ll_screen_device *device, const Rgba8 *color);

    int32_t (*get_background)(struct ll_screen_device *device, Rgba8 *color);

    /*
     *
     * Screen must have their own dimension
     *
     * */
    int32_t (*get_width)(struct ll_screen_device *device, uint32_t *output);

    int32_t (*get_height)(struct ll_screen_device *device, uint32_t *output);

    /*
     *
     * When we don't need it anymore, resources that it using should be handle properly
     *
     * */
    void (*destroy)(struct ll_screen_device *device);
} ll_screen_device_CLASS;

typedef struct ll_screen_device {
    const ll_screen_device_CLASS *class;
    const char *name;
    uint8_t colorDepth;
    void *data;
} ll_screen_device;

#endif //LDPF4L_COM_SCREEN_DEVICE_H
