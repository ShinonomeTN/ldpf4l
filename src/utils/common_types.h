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

typedef struct RectTuple {
    int x0;
    int y0;
    int x1;
    int y1;
} RectTuple;

//int rect_tuple_width(RectTuple* rectTuple);
#define rect_tuple_width(rect) ((rect)->x1 - (rect)->x0 + 1)
//int rect_tuple_height(RectTuple* rectTuple);
#define rect_tuple_height(rect) ((rect)->y1 - (rect)->y0 + 1)
//void rect_tuple_set(RectTuple* rectTuple, unsigned int x0, unsigned int y0, unsigned int x1, unsigned y1);
#define rect_tuple_set(rect, _x0, _y0, _x1, _y1) do {\
    (rect)->x0 = (_x0);\
    (rect)->y0 = (_y0);\
    (rect)->x1 = (_x1);\
    (rect)->y1 = (_y1);\
} while(0)

void rect_tuple_clip_bound(RectTuple *target, RectTuple *by);

void rect_tuple_enlarge_bound(RectTuple *target, RectTuple *by);

//void rect_tuple_copy(const RectTuple *source, RectTuple *target);
#define rect_tuple_copy(source, target) do{\
    (target)->x1 = (source)->x1;\
    (target)->y1 = (source)->y1;\
    (target)->x0 = (source)->x0;\
    (target)->y0 = (source)->y0;\
}while(0)

/*
 * Point
 *
 * */

typedef struct Vector {
    int x;
    int y;
} PointTuple;

#define point_tuple_equals(a, b) ((a)->x == (b)->x && (a)->y == (b)->y)

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

unsigned int rgba_8_to_int(Rgba8 *color);

void rgba_8_from_int(Rgba8 *color, unsigned int rgba);

#define rgba8_from_int_abgr(/*(Rgba8 *)*/color, /*(unsigned int)*/ argb) (*(unsigned int *) color = argb)
//void rgba8_from_int_abgr(Rgba8 *color, unsigned int argb);

void rgba_8_on_color(Rgba8 *color, const Rgba8 *anotherColor);

void rgba_8_apply_alpha(Rgba8 *color, Rgba8 *backgroundColor);

//unsigned short rgba_8_to_rgb_565(Rgba8 *color);
#define rgba_8_to_rgb_565(color) \
((unsigned short) (((color)->red & 0xF8U) << 8U) + (((color)->green & 0xFCU) << 3U) + (((color)->blue & 0xF8U) >> 3U))

//unsigned short rgba_8_to_rgb_565_reverse(Rgba8 *color);
#define rgba_8_to_rgb_565_reverse(color) \
((unsigned short) (((((color)->green & 0x1cU) << 3U) | (((color)->blue & 0xf8U) >> 3U)) << 8U) | ((((color)->red) & 0xf8U) | ((color)->green & 0xe0U) >> 5U) )

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
