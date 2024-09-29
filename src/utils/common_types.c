//
// Created by cattenlinger on 7/2/19.
//
#include "common_types.h"

/*
 * RectTuple
 * */

// Clip a rect, make it in bound.
void rect_tuple_clip_bound(RectTuple *target, RectTuple *by) {
    if (target->x0 < by->x0) target->x0 = by->x0;
    if (target->y0 < by->y0) target->y0 = by->y0;
    if (target->x0 > by->x1) target->x0 = by->x1;
    if (target->y0 > by->y1) target->y0 = by->y1;

    if (target->x1 < by->x0) target->x1 = by->x0;
    if (target->y1 < by->y0) target->y1 = by->y0;
    if (target->x1 > by->x1) target->x1 = by->x1;
    if (target->y1 > by->y1) target->y1 = by->y1;
}

// add area by another rect
void rect_tuple_enlarge_bound(RectTuple *target, RectTuple *by) {
    if (target->x0 > by->x0) target->x0 = by->x0;
    if (target->y0 > by->y0) target->y0 = by->y0;
    if (target->x1 < by->x1) target->x1 = by->x1;
    if (target->y1 < by->y1) target->y1 = by->y1;
}

/*
 * RGB8888 color
 * */

//unsigned short rgba_8_to_rgb_565(Rgba8 *color) {
//return ((color->red & 0xF8U) << 8U) + ((color->green & 0xFCU) << 3U) + ((color->blue & 0xF8U) >> 3U);
//}

/*
#define TO_RGB565_H(color) ((((color.red) & 0xf8U)) | (((color.green) & 0xe0U) >> 5U))
#define TO_RGB565_L(color) ((((color.green) & 0x1cU) << 3U) | (((color.blue) & 0xf8U) >> 3U))

unsigned short rgba_8_to_rgb_565_reverse(Rgba8 *color) {
     unsigned char data[2] = {
            ((color->red) & 0xf8U) | (color->green & 0xe0U) >> 5U,
            ((color->green & 0x1cU) << 3U) | ((color->blue & 0xf8U) >> 3U),
    };
    return ((unsigned short *) data)[0];
}
*/

//unsigned short rgba_8_to_rgb_565_reverse(Rgba8 *color) {
//    return ((((color->green & 0x1cU) << 3U) | ((color->blue & 0xf8U) >> 3U)) << 8U) | (((color->red) & 0xf8U) | (color->green & 0xe0U) >> 5U);
//}

unsigned int rgba_8_to_int(Rgba8 *color) {
//    unsigned int* c = (unsigned int *) color;
//    return *c;
    return (color->red << 24U) + (color->green << 16U) + (color->blue << 8U) + (color->alpha);
}

void rgba_8_from_int(Rgba8 *color, const unsigned int rgba) {
    color->red = rgba >> 24U;
    color->green = (rgba & 0x00FF0000U) >> 16U;
    color->blue = (rgba & 0x0000FF00U) >> 8U;
    color->alpha = rgba & 0x000000FFU;
}

//void rgba8_from_int_abgr(Rgba8 *color, const unsigned int argb) {
//    color->red = argb & 0x000000FFU;
//    color->green = (argb & 0x0000FF00U) >> 8U;
//    color->blue = (argb & 0x00FF0000U) >> 16U;
//    color->alpha = argb >> 24U;
//}

void rgba_8_on_color(Rgba8 *color, const Rgba8 *anotherColor) {
    // Just the blending formula
#define RGBA_ALPHA_BLEND(colorA, alphaA, colorB, alphaB) \
    (unsigned char) ((((float) colorA * alphaA) + (colorB * alphaB) * (1 - alphaA)) / (alphaA + alphaB * (1 - alphaA)))

    float alphaA = (float) anotherColor->alpha / 255;
    float alphaB = (float) color->alpha / 255;

    unsigned char red = RGBA_ALPHA_BLEND(anotherColor->red, alphaA, color->red, alphaB);
    unsigned char green = RGBA_ALPHA_BLEND(anotherColor->green, alphaA, color->green, alphaB);
    unsigned char blue = RGBA_ALPHA_BLEND(anotherColor->blue, alphaA, color->blue, alphaB);
    unsigned char alpha = (unsigned char) ((alphaA + alphaB * (1 - alphaA)) * 255);

    color->red = red;
    color->green = green;
    color->blue = blue;
    color->alpha = alpha;
}

void rgba_8_apply_alpha(Rgba8 *color, Rgba8 *backgroundColor) {
#define RGBA_APPLY_ALPHA(alpha, channel) ((unsigned short)(alpha) * (channel) / 255)

    if (color->alpha < 255) {
        unsigned short red =
                RGBA_APPLY_ALPHA(backgroundColor->alpha, backgroundColor->red) +
                RGBA_APPLY_ALPHA(color->alpha, color->red);
        unsigned short green =
                RGBA_APPLY_ALPHA(backgroundColor->alpha, backgroundColor->green) +
                RGBA_APPLY_ALPHA(color->alpha, color->green);
        unsigned short blue =
                RGBA_APPLY_ALPHA(backgroundColor->alpha, backgroundColor->blue) +
                RGBA_APPLY_ALPHA(color->alpha, color->blue);

        if (red > 255) red = 255;
        if (green > 255) green = 255;
        if (blue > 255) blue = 255;

        color->red = red;
        color->green = green;
        color->blue = blue;
    }

    color->alpha = 255;
}