//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_COM_SCREEN_DEVICE_H
#define LDPF4L_COM_SCREEN_DEVICE_H

#include "../utils/common_types.h"

struct ll_screen_device_CLASS;
struct ll_screen_device;

typedef struct ll_screen_device_CLASS {

    /*
     *
     * All device accept an array of data, and a rectangle described a drawing area
     *
     * */
    int (*flush)(struct ll_screen_device *device, const unsigned int *pixels, const RectTuple *dimension);

    /*
     *
     * Normally, screen has a attribute to describe how bright it is and can be tune.
     *
     * */
    int (*set_backlight_level)(struct ll_screen_device *device, const int level);

    int (*get_backlight_level)(struct ll_screen_device *device, int *output);

    /*
     *
     * Screens maybe not transparent, maybe it's, but we don't known if we have no instance of a screen.
     *
     * */
    int (*set_background)(struct ll_screen_device *device, const Rgba8 *color);

    int (*get_background)(struct ll_screen_device *device, Rgba8 *color);

    /*
     *
     * Screen must have their own dimension
     *
     * */
    int (*get_width)(struct ll_screen_device *device, unsigned int *output);

    int (*get_height)(struct ll_screen_device *device, unsigned int *output);

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
    unsigned char colorDepth;
    void *data;
} ll_screen_device;

#endif //LDPF4L_COM_SCREEN_DEVICE_H
