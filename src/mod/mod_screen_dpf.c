#include "../driver/dpf/dpf_device.h"
#include "../utils/log.h"

#include "mod_screen_dpf.h"
/*
 *
 *
 *
 * */

uint32_t dpf_open_screen_device(libusb_device *usbDevice, ll_screen_device *screenDevice) {
    dpf_device_t *dpf = NULL;
    int result = dpf_device_open(usbDevice, &dpf);

    if (result != 0) return result;

    screenDevice->data = dpf;
    screenDevice->name = "dpf";
    screenDevice->class = &ll_dpf_screen_CLASS;
    screenDevice->colorDepth = 16;

    result = dpf_device_acquire_dimensions(dpf);

    return result;
}

int32_t _flush_impl(const ll_screen_device *device, const uint32_t *pixels, const RectTuple *dimension) {
    const uint32_t rWidth = rect_tuple_width(dimension);
    const uint32_t rHeight = rect_tuple_height(dimension);

    dpf_device_t *dpf = device->data;

    uint16_t *buffer = (uint16_t *) dpf_device_get_buffer(dpf);

    Rgba8 color;
    const uint32_t lineWidth = dpf_device_screen_width(dpf);
    const uint32_t rectX = dimension->x0;
    const uint32_t rectY = dimension->y0;

    uint32_t pos = 0;
    for (uint32_t iY = 0; iY < rHeight; iY++) {
        const uint32_t canvasY = (rectY + iY) * lineWidth;
        for (uint32_t iX = 0; iX < rWidth; iX++) {

            rgba_8_from_int(&color, pixels[(rectX + iX) + canvasY]);
            if (color.alpha != 255) rgba_8_apply_alpha(&color, (Rgba8 *)dpf_device_get_background_color(dpf));

//             attention: Because dpf is 2 bytes per pixel, so here the gap is 2
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL)] = TO_RGB565_H(color);
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL) + 1] = TO_RGB565_L(color);

            const uint16_t pixel = rgba_8_to_rgb_565_reverse(&color);
            buffer[pos++] = pixel;
        }
    }

    const int32_t result = dpf_device_flush(dpf, dimension->x0, dimension->y0, dimension->x1, dimension->y1);
    if (result < 0) log_warn("could not flush data to screen.");
    return result;
}

int32_t _set_backlight_level_impl(const ll_screen_device *device, const int32_t level) {
    const int32_t result = dpf_device_set_brightness(device->data, level);
    return result;
}

int32_t _get_backlight_level_impl(const ll_screen_device *device, int *level) {
    *level = dpf_device_get_brightness(device->data);
    return 0;
}

int32_t _set_background_impl(const ll_screen_device *device, const Rgba8 *color) {
    dpf_device_set_background_color(device->data, color->red, color->green, color->blue);
    return 0;
}

int32_t _get_background_impl(const ll_screen_device *device, Rgba8 *color) {
    const Rgba8 *c = (Rgba8 *) dpf_device_get_background_color(device->data);
    color->red = c->red;
    color->green = c->green;
    color->blue = c->blue;
    color->alpha = 255;
    return 0;
}

int32_t _get_width_impl(const ll_screen_device *device, unsigned int *output) {
    *output = dpf_device_screen_width(device->data);
    return 0;
}

int32_t _get_height_impl(const ll_screen_device *device, unsigned int *output) {
    *output = dpf_device_screen_height(device->data);
    return 0;
}

void _destroy_impl(const ll_screen_device *device) {
    dpf_destroy(device->data);
}

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