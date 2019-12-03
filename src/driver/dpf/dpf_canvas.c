//
// Created by cattenlinger on 7/3/19.
//

#include <stdlib.h>
#include <stdio.h>

#include "dpf_canvas.h"
#include "dpf_device.h"
#include "../../utils/common_types.h"
#include "../../utils/log.h"

struct dpf_canvas {
    dpf_device *device;

    unsigned int width;
    unsigned int height;
    RectTuple canvasRect;

    // The background color's alpha channel was ignored.
    Rgba8 backgroundColor;

    unsigned char rotate;
    unsigned char flip;

    // Buffer
    // Screen buffer is RGB8888 with unsigned int
    unsigned int *screenBuffer;
    unsigned long screenBufferSize;

    // The area need to be refresh
    RectTuple dirtyRect;
    int isDirty;

};

// Report dirty region using this function
void _report_dirty_rect(dpf_canvas *canvas, RectTuple *drawRect) {
    rect_tuple_enlarge_bound(&(canvas->dirtyRect), drawRect);
    canvas->isDirty = 1;
}

void _reset_dirty_flag(dpf_canvas *canvas) {
    canvas->isDirty = 0;
}

dpf_canvas *dpf_canvas_create(dpf_device *device) {
    dpf_canvas *canvas = (dpf_canvas *) malloc(sizeof(dpf_canvas));

    canvas->device = device;

    canvas->width = dpf_device_screen_width(device);
    canvas->height = dpf_device_screen_height(device);
    rect_tuple_set(&(canvas->canvasRect), 0, 0, canvas->width - 1, canvas->height - 1);

    canvas->rotate = 0;
    canvas->flip = 0;

    canvas->isDirty = 0;
    rect_tuple_set(&(canvas->dirtyRect), 0, 0, 0, 0);
    dpf_canvas_set_background_color(canvas, 0, 0, 0);

    unsigned long screenBufferSize = canvas->width * canvas->height;
    canvas->screenBufferSize = screenBufferSize;
    canvas->screenBuffer = (unsigned int *) malloc(sizeof(unsigned int) * screenBufferSize);
    if (!canvas->screenBuffer) {
        log_error("could not allocate memory for canvas.");
        return NULL;
    }

    log_debug("canvas for dpf created.\n");
    return canvas;
}

void dpf_canvas_destroy(dpf_canvas *canvas) {
    free(canvas->screenBuffer);
    free(canvas);
    log_debug("canvas destroyed.");
}

void dpf_canvas_flush(dpf_canvas *canvas) {
    if (!canvas->isDirty) return;

    log_trace(
            "dirty region [%3d,%3d] -> [%3d,%3d].",
            (int)canvas->dirtyRect.x0,
            (int)canvas->dirtyRect.y0,
            (int)canvas->dirtyRect.x1,
            (int)canvas->dirtyRect.y1
    );

    unsigned int *canvasBuffer = canvas->screenBuffer;
//    unsigned char *deviceBuffer = dpf_device_get_buffer(canvas->device);
    unsigned short *deviceBuffer = (unsigned short*)dpf_device_get_buffer(canvas->device);

    Rgba8 color;
    unsigned int canvasWidth = canvas->width;
    unsigned int rectX = canvas->dirtyRect.x0;
    unsigned int rectY = canvas->dirtyRect.y0;
    unsigned int rectWidth = rect_tuple_width(&(canvas->dirtyRect)) + 1;
    unsigned int rectHeight = rect_tuple_height(&(canvas->dirtyRect)) + 1;

    unsigned long devicePointer = 0;
    for (unsigned int y = 0; y < rectHeight; y++) {
        unsigned int canvasY = (y + rectY) * canvasWidth;
        for (unsigned int x = 0; x < rectWidth; x++) {

            rgba_8_from_int(&color, canvasBuffer[canvasY + (x + rectX)]);
            rgba_8_apply_alpha(&color, &(canvas->backgroundColor));

            // attention: Because dpf is 2 bytes per pixel, so here the gap is 2
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL)] = TO_RGB565_H(color);
//            deviceBuffer[(devicePointer * DPF_BYTE_PRE_PIXEL) + 1] = TO_RGB565_L(color);
            deviceBuffer[devicePointer] = rgba_8_to_rgb_565_reverse(&color);
            devicePointer++;
        }
    }

    RectTuple transferRect;
    rect_tuple_set(&transferRect, canvas->dirtyRect.x0, canvas->dirtyRect.y0, canvas->dirtyRect.x1 + 1,
                   canvas->dirtyRect.y1 + 1);
    int result = dpf_device_flush(canvas->device, &transferRect);
    if (result < 0) log_warn("could not flush data to screen.");

    _reset_dirty_flag(canvas);
}

void dpf_canvas_force_flush(dpf_canvas *canvas) {
    unsigned int *canvasBuffer = canvas->screenBuffer;
    unsigned short *deviceBuffer = (unsigned short *)dpf_device_get_buffer(canvas->device);
    unsigned long bufferSize = canvas->screenBufferSize;

    Rgba8 color;
    for (unsigned long i = 0; i < bufferSize; i++) {
        rgba_8_from_int(&color, canvasBuffer[i]);
        rgba_8_apply_alpha(&color, &(canvas->backgroundColor));
        deviceBuffer[i] = rgba_8_to_rgb_565_reverse(&color);
    }

    RectTuple deviceScreenRect = {
            0,
            0,
            dpf_device_screen_width(canvas->device) - 1,
            dpf_device_screen_height(canvas->device) - 1
    };

    int result = dpf_device_flush(canvas->device, &deviceScreenRect);
    if (result < 0) log_warn("could not flush data to screen.");

    _reset_dirty_flag(canvas);
}

void dpf_canvas_clear(dpf_canvas *canvas) {
    unsigned long bufferSize = canvas->screenBufferSize;
    unsigned int *buffer = canvas->screenBuffer;

    unsigned int color = rgba_8_to_int(&(canvas->backgroundColor));
    for (unsigned long i = 0; i < bufferSize; i++) {
        buffer[i] = color;
    }
    _report_dirty_rect(canvas, &(canvas->canvasRect));
}

void dpf_canvas_set_point(dpf_canvas *canvas, PointTuple *point, Rgba8 *color) {
    if (point->x > canvas->canvasRect.x1 || point->y > canvas->canvasRect.y1) return;

    unsigned int *buffer = canvas->screenBuffer;
    unsigned int pointer = point->y * canvas->width + point->x;

    Rgba8 originColor;
    rgba_8_from_int(&originColor, buffer[pointer]);
    rgba_8_on_color(&originColor, color);
    buffer[pointer] = rgba_8_to_int(&originColor);

    RectTuple dirtyArea = {point->x, point->y, point->x, point->y};
    if (!canvas->isDirty) rect_tuple_set(&(canvas->dirtyRect), point->x, point->y, point->x, point->y);
    _report_dirty_rect(canvas, &dirtyArea);
}

void dpf_canvas_fill_color(dpf_canvas *canvas, const RectTuple *rectTuple, Rgba8 *color) {
    RectTuple fillingTuple;
    rect_tuple_copy(rectTuple, &fillingTuple);
    rect_tuple_clip_bound(&fillingTuple, &(canvas->canvasRect));

    Rgba8 oldColor;
    unsigned int rectWidth = rect_tuple_width(&fillingTuple) + 1;
    unsigned int rectHeight = rect_tuple_height(&fillingTuple) + 1;
    unsigned int canvasWidth = canvas->width;
    unsigned int *buffer = canvas->screenBuffer;
    for (unsigned int y = 0; y < rectHeight; y++) {
        unsigned int canvasY = (y + rectTuple->y0) * canvasWidth;
        for (unsigned int x = 0; x < rectWidth; x++) {
            unsigned int canvasPointer = canvasY + (x + rectTuple->x0);

            rgba_8_from_int(&oldColor, buffer[canvasPointer]);
            rgba_8_on_color(&oldColor, color);
            buffer[canvasPointer] = rgba_8_to_int(&oldColor);
        }
    }

    if (!canvas->isDirty) {
        rect_tuple_set(&(canvas->dirtyRect), rectTuple->x0, rectTuple->y0, rectTuple->x0, rectTuple->y0);
    }

    _report_dirty_rect(canvas, &fillingTuple);
}

void dpf_canvas_fill_binary(dpf_canvas *canvas, const unsigned int *buffer, PointTuple *position, unsigned int width,
                            unsigned int height) {
    RectTuple drawingRect;
    rect_tuple_set(&drawingRect, position->x, position->y, position->x + width - 1, position->y + height - 1);
    rect_tuple_clip_bound(&drawingRect, &(canvas->canvasRect));

    unsigned int *canvasBuffer = canvas->screenBuffer;

    Rgba8 colorA;
    Rgba8 colorB;
    for (unsigned long y = 0; y < height; y++) {
        unsigned int bufferY = y * width;
        unsigned int canvasY = (y + drawingRect.y0) * canvas->width;

        for (unsigned long x = 0; x < width; x++) {

            unsigned int bufferPointer = bufferY + x;
            unsigned int canvasPointer = canvasY + (x + drawingRect.x0);
            rgba_8_from_int(&colorA, canvasBuffer[canvasPointer]);
            rgba_8_from_int(&colorB, buffer[bufferPointer]);
            rgba_8_on_color(&colorA, &colorB);
            canvasBuffer[canvasPointer] = rgba_8_to_int(&colorA);
        }
    }

    _report_dirty_rect(canvas, &drawingRect);
}

unsigned int dpf_canvas_get_width(dpf_canvas *canvas) {
    return canvas->width;
}

unsigned int dpf_canvas_get_height(dpf_canvas *canvas) {
    return canvas->height;
}

void dpf_canvas_set_background_color(dpf_canvas *canvas, unsigned char r, unsigned char g, unsigned char b) {
    Rgba8 *backgroundColor = &(canvas->backgroundColor);
    backgroundColor->red = r;
    backgroundColor->green = g;
    backgroundColor->blue = b;

    backgroundColor->alpha = 0xFFu;
}

Rgba8 *dpf_canvas_get_background_color(dpf_canvas *canvas) {
    return &(canvas->backgroundColor);
}