//
// Created by cattenlinger on 7/2/19.
//

#ifndef DPF_DISPLAY_COMMON_TYPES_H
#define DPF_DISPLAY_COMMON_TYPES_H

/*
 *
 * Dimension
 *
 * */

typedef struct {
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;
} RectTuple;

int rect_tuple_width(RectTuple* rectTuple);
int rect_tuple_height(RectTuple* rectTuple);
void rect_tuple_set(RectTuple* rectTuple, unsigned int x0, unsigned int y0, unsigned int x1, unsigned y1);
void rect_tuple_clip_bound(RectTuple *target, RectTuple *by);
void rect_tuple_enlarge_bound(RectTuple *target, RectTuple *by);
void rect_tuple_copy(const RectTuple* source, RectTuple *target);

typedef struct Vector {
    unsigned int x;
    unsigned int y;
} PointTuple, Size;

/*
 *
 * Colors
 *
 * */

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} Rgba8;

unsigned int rgba_8_to_int(Rgba8* color);
void rgba_8_from_int(Rgba8* color, unsigned int rgba);
void rgba_8_on_color(Rgba8* color, const Rgba8* anotherColor);
void rgba_8_apply_alpha(Rgba8 *color, Rgba8 *backgroundColor);
unsigned short rgba_8_to_rgb_565(Rgba8 * color);
unsigned short rgba_8_to_rgb_565_reverse(Rgba8 *color);

#define TO_RGB565_H(color) ((((color.red) & 0xf8U)) | (((color.green) & 0xe0U) >> 5U))
#define TO_RGB565_L(color) ((((color.green) & 0x1cU) << 3U) | (((color.blue) & 0xf8U) >> 3U))

/*
 *
 * Numbers
 *
 * */

#define SPLIT_TO_2_BYTE_L(num) (num & 0x00FFU)
#define SPLIT_TO_2_BYTE_H(num) ((num & 0xFF00U) >> 8U)

#endif //DPF_DISPLAY_COMMON_TYPES_H
