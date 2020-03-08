//
// Created by cattenlinger on 20-3-4.
//

#include <stdlib.h>

#include "../utils/log.h"

#include "com_canvas.h"

void _report_dirty_rect(ll_canvas *self, RectTuple *drawRect) {
    if (self->isDirty) rect_tuple_enlarge_bound(&(self->dirtyRect), drawRect);
    else
        rect_tuple_copy(drawRect, &self->dirtyRect);
    self->isDirty = 1;
}

#define _clear_dirty_flag(self) (self->isDirty = 0)

int ll_canvas_create(ll_canvas *self, int width, int height) {

    self->width = width;
    self->height = height;

    const unsigned long size = width * height;

    unsigned int *buffer = (unsigned int *) malloc(sizeof(unsigned int) * size);
    if (buffer == NULL) {
        log_fatal("Could not create buffer for canvas");
        return -1;
    }

    self->buffer = buffer;
    self->bufferSize = size;

    self->background.red = 255;
    self->background.green = 255;
    self->background.blue = 255;
    self->background.alpha = 0;

    rect_tuple_set(&self->dimension, 0, 0, width - 1, height - 1);
    _clear_dirty_flag(self);

    return 0;
}

void ll_canvas_destroy(ll_canvas *self) {
    free(self->buffer);
//    free(self);
    log_debug("Canvas buffer destroyed");
}

void ll_canvas_clear(ll_canvas *self) {
    unsigned int color = rgba_8_to_int(&self->background);

    for (unsigned long i = 0; i < self->bufferSize; ++i) {
        self->buffer[i] = color;
    }
    _report_dirty_rect(self, &self->dimension);
}

unsigned char ll_canvas_is_dirty(ll_canvas *self) {
    return self->isDirty;
}

const unsigned int *ll_canvas_get_buffer(ll_canvas *self) {
    return self->buffer;
}

const RectTuple *ll_canvas_get_dirty_rect(ll_canvas *self) {
    return &self->dirtyRect;
}

void ll_canvas_set_point(ll_canvas *self, const PointTuple *point, const Rgba8 *color) {
    if (point->x > self->dimension.x1 || point->y > self->dimension.y1) return;

    unsigned long pos = point->y * self->width + point->x;

    Rgba8 originColor;
    rgba_8_from_int(&originColor, self->buffer[pos]);
    rgba_8_on_color(&originColor, color);
    self->buffer[pos] = rgba_8_to_int(&originColor);

    RectTuple area = {
            point->x, point->y, point->x, point->y
    };

    _report_dirty_rect(self, &area);
}

void ll_canvas_fill_color(ll_canvas *self, const RectTuple *rect, const Rgba8 *color) {
    RectTuple area;
    rect_tuple_copy(rect, &area);
    rect_tuple_clip_bound(&area, &(self->dimension));

    const unsigned int lineWidth = self->width;
    const unsigned int rWidth = rect_tuple_width(&area);
    const unsigned int rHeight = rect_tuple_height(&area);

    Rgba8 tempColor;
    unsigned int *buffer = self->buffer;
    for (unsigned int iY = 0; iY < rHeight; iY++) {
        const unsigned int posY = (rect->y0 + iY) * lineWidth;
        for (unsigned int iX = 0; iX < rWidth; iX++) {
            unsigned int pos = (rect->x0 + iX) + posY;

            rgba_8_from_int(&tempColor, buffer[pos]);
            rgba_8_on_color(&tempColor, color);
            buffer[pos] = rgba_8_to_int(&tempColor);
        }
    }

    _report_dirty_rect(self, &area);
}

void ll_canvas_copy_area(ll_canvas *self,
                         const RectTuple *dimension,
                         ll_canvas *sourceCanvas,
                         const unsigned char blend) {
    RectTuple area;
    rect_tuple_copy(dimension, &area);
    rect_tuple_clip_bound(&area, &sourceCanvas->dimension);
    rect_tuple_clip_bound(&area, &self->dimension);

    int areaWidth = rect_tuple_width(&area);
    int areaHeight = rect_tuple_height(&area);

    const int sourceWidth = sourceCanvas->width;
    const int canvasWidth = self->width;

    Rgba8 bottom;
    Rgba8 top;
    for (int iY = 0; iY < areaHeight; iY++) {
        const int sourceY = (iY + area.y0) * sourceWidth;
        const int canvasY = (iY + area.y0) * canvasWidth;
        for (int iX = 0; iX < areaWidth; iX++) {
            unsigned int *sourcePos = &sourceCanvas->buffer[(iX + area.x0) + sourceY];
            unsigned int *canvasPos = &self->buffer[(iX + area.x0) + canvasY];

            rgba_8_from_int(&top, *sourcePos);
            if (blend) {
                rgba_8_from_int(&bottom, *canvasPos);
                rgba_8_on_color(&bottom, &top);
                *canvasPos = rgba_8_to_int(&bottom);
            } else {
                *canvasPos = rgba_8_to_int(&top);
            }
        }
    }

    _report_dirty_rect(self, &area);
}

void ll_canvas_fill_data(ll_canvas *self,
                         const RectTuple *pictureDimension,
                         const unsigned int *buffer,
                         const unsigned char blend) {
    int picWidth = rect_tuple_width(pictureDimension);
//    int picHeight = rect_tuple_height(pictureDimension);

    RectTuple area;
    rect_tuple_copy(pictureDimension, &area);
    rect_tuple_clip_bound(&area, &self->dimension);

    int areaWidth = rect_tuple_width(&area);
    int areaHeight = rect_tuple_height(&area);

    unsigned int *canvas = self->buffer;

    Rgba8 colorBack;
    Rgba8 colorTop;

    for (int iY = 0; iY < areaHeight; iY++) {
        int bufferY = iY * picWidth;
        int canvasY = (iY + (&area)->y0) * self->width;

        for (int x = 0; x < areaWidth; x++) {
            int bufferPos = bufferY + x;
            int canvasPos = canvasY + x + area.x0;

            rgba8_from_int_abgr(&colorTop, buffer[bufferPos]);
//            rgba_8_from_int(&colorTop, buffer[bufferPos]);
            if (blend) {
                rgba_8_from_int(&colorBack, canvas[canvasPos]);
                rgba_8_on_color(&colorBack, &colorTop);
                canvas[canvasPos] = rgba_8_to_int(&colorBack);
            } else {
                colorTop.alpha = 255;
                canvas[canvasPos] = rgba_8_to_int(&colorTop);
            }
        }
    }

    _report_dirty_rect(self, &area);
}

/*
 *
 *
 * */

void ll_canvas_draw_frame(ll_canvas *self, const RectTuple *rect, const Rgba8 *color, const int lineWidth) {
    RectTuple area;
    rect_tuple_copy(rect, &area);
    rect_tuple_clip_bound(&area, &self->dimension);

    int width = rect_tuple_width(&area);
    int height = rect_tuple_height(&area);

    int b = lineWidth * 2;
    if (width <= b || height <= b) return ll_canvas_fill_color(self, &area, color);

    int top = lineWidth - 1;
    int bottom = height - lineWidth - 1;
    int left = lineWidth - 1;
    int right = width - lineWidth - 1;

    Rgba8 tempColor;
    for (int line = 0; line < height; line++) {
        int canvasY = (line + area.y0) * self->width;

        if (line <= top || line > bottom) {
            for (int iX = 0; iX < width; ++iX) {
                int pos = (area.x0 + iX) + canvasY;

                rgba_8_from_int(&tempColor, self->buffer[pos]);
                rgba_8_on_color(&tempColor, color);
                self->buffer[pos] = rgba_8_to_int(&tempColor);
            }
        } else {
            for (int iX = 0; iX < width; ++iX) {
                if (iX <= left || iX > right) {
                    int pos = (area.x0 + iX) + canvasY;

                    rgba_8_from_int(&tempColor, self->buffer[pos]);
                    rgba_8_on_color(&tempColor, color);
                    self->buffer[pos] = rgba_8_to_int(&tempColor);
                }
            }
        }
    }

    _report_dirty_rect(self, &area);
}

void ll_canvas_reset_dirty_rect(ll_canvas *self) {
    _clear_dirty_flag(self);
}

/*
 *
 * */

unsigned int ll_canvas_get_width(ll_canvas *self) {
    return self->width;
}

unsigned int ll_canvas_get_height(ll_canvas *self) {
    return self->height;
}

const Rgba8 *ll_canvas_get_background(ll_canvas *self) {
    return &self->background;
}

void ll_canvas_set_background(ll_canvas *self, const Rgba8 *color) {
    self->background.red = color->red;
    self->background.green = color->green;
    self->background.blue = color->blue;
    self->background.alpha = color->alpha;
}