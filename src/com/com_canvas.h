//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_COM_CANVAS_H
#define LDPF4L_COM_CANVAS_H

#include "../utils/common_types.h"
#include "com_screen_device.h"

typedef struct ll_canvas {
    unsigned int width;
    unsigned int height;
    RectTuple dimension;

    unsigned int *buffer;
    unsigned long bufferSize;

    RectTuple dirtyRect;
    unsigned char isDirty;

    Rgba8 background;
} ll_canvas;

/*
 * Lifecycle
 *
 * */

int ll_canvas_create(ll_canvas *self, int width, int height);

void ll_canvas_destroy(ll_canvas *self);


/*
 * Buffer, drawing area
 *
 * */

void ll_canvas_clear(ll_canvas *self);

const unsigned int *ll_canvas_get_buffer(ll_canvas *self);

const RectTuple *ll_canvas_get_dirty_rect(ll_canvas *self);

unsigned char ll_canvas_is_dirty(ll_canvas *self);

/*
 * Painting
 *
 * */

void ll_canvas_set_point(ll_canvas *self, const PointTuple *point, const Rgba8 *color);

void ll_canvas_fill_color(ll_canvas *self, const RectTuple *rect, const Rgba8 *color);

void ll_canvas_fill_data(ll_canvas *self,
                         const RectTuple *pictureDimension,
                         const unsigned int *buffer,
                         unsigned char blend);

void ll_canvas_copy_area(ll_canvas *self,
                         const PointTuple *point,
                         ll_canvas *source,
                         const RectTuple *copyArea,
                         unsigned char blend);

void ll_canvas_copy(ll_canvas *self, const RectTuple *dimension, ll_canvas *sourceCanvas, unsigned char blend);

void ll_canvas_draw_frame(ll_canvas *self, const RectTuple *rect, const Rgba8 *color, int lineWidth);

/*
 *
 *
 * */

void ll_canvas_reset_dirty_rect(ll_canvas *self);

/*
 * Info
 *
 * */

unsigned int ll_canvas_get_width(ll_canvas *self);

unsigned int ll_canvas_get_height(ll_canvas *self);

const Rgba8 *ll_canvas_get_background(ll_canvas *self);

void ll_canvas_set_background(ll_canvas *self, const Rgba8 *color);

#endif //LDPF4L_COM_CANVAS_H
