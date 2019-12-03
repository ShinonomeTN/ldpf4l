//
// Created by cattenlinger on 7/3/19.
//

#ifndef DPF_DISPLAY_DPF_CANVAS_H
#define DPF_DISPLAY_DPF_CANVAS_H

#include "../../utils/common_types.h"
#include "dpf_device.h"

/*
 *
 * Structure that hold canvas info
 *
 * */
typedef struct dpf_canvas dpf_canvas;

/*
 *
 * Life cycle
 *
 * */

// Create canvas structure
dpf_canvas* dpf_canvas_create(dpf_device* device);

// Destroy canvas
void dpf_canvas_destroy(dpf_canvas* canvas);


/********************************************
 *
 * Buffer management
 *
 * */

// Flush canvas content to device
void dpf_canvas_flush(dpf_canvas* canvas);

// Clear the canvas buffer
void dpf_canvas_clear(dpf_canvas* canvas);

// Force flush the buffer to canvas, ignore dirty rectangle
void dpf_canvas_force_flush(dpf_canvas* canvas);

/************************************
 *
 * Painting
 *
 * */

// Set single point
void dpf_canvas_set_point(dpf_canvas* canvas, PointTuple* point, Rgba8* color);

// Fill color at area
void dpf_canvas_fill_color(dpf_canvas* canvas, const RectTuple* rect, Rgba8* color);

// Fill binary data to an area. Data should be a Rgba8 matrix.
void dpf_canvas_fill_binary(dpf_canvas *canvas, const unsigned int *buffer, PointTuple* position, unsigned int width, unsigned int height);

/********************************
 *
 * Meta info
 *
 * */

// Get canvas width
unsigned int dpf_canvas_get_width(dpf_canvas* canvas);

// Get canvas height
unsigned int dpf_canvas_get_height(dpf_canvas* canvas);

// Set default canvas background color
void dpf_canvas_set_background_color(dpf_canvas* canvas, unsigned char r, unsigned char g, unsigned char b);

// Get canvas background color
Rgba8* dpf_canvas_get_background_color(dpf_canvas* canvas);

#endif //DPF_DISPLAY_DPF_CANVAS_H
