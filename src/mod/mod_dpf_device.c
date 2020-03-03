//
// Created by cattenlinger on 20-2-28.
//


#define LUA_LIB

#include <stdlib.h>

#include "lprefix.h"
#include "lauxlib.h"
#include "lualib.h"

#include "../utils/log.h"
#include "../driver/dpf/dpf_device.h"
#include "../driver/dpf/dpf_canvas.h"
#include "../utils/lua_utils.h"

#include "mod_usb_explorer.h"

#include "mod_dpf_device.h"

//static int _table_readonly(lua_State *L) {
//    luaL_error(L, "error: table readonly");
//    return 0;
//}

/*
 *
 * Dpf device
 *
 * */

#define t_DPF_DEVICE "t_DpfDevice"

static int _dpf_device_open(lua_State *L) {
    struct libusb_device **device = luaU_checkoutType(L, MOD_t_USB_DEVICE);
    dpf_device **dpf = lua_newuserdata(L, sizeof(dpf_device *));
    int result = dpf_device_open(*device, &*dpf);

    switch (result) {
        case 0:
            break;
        case DPF_ERROR_DEVICE_NOT_SUPPORTED:
            luaL_error(L, "error: attempt to open an unsupported device");
            break;
        default:
            luaL_error(L, "error: failed to open usb device, error %s(%d)", libusb_error_name(result), result);
            break;
    }

    luaL_setmetatable(L, t_DPF_DEVICE);
    return 1;
}

static int _dpf_device_gc(lua_State *L) {
    dpf_device **dpf = luaU_checkoutType(L, t_DPF_DEVICE);
    dpf_destroy(*dpf);

    log_trace("[%s gc]", t_DPF_DEVICE);

    return 0;
}

static int _dpf_device_dimensions(lua_State *L) {
    dpf_device **dpf = luaU_checkoutType(L, t_DPF_DEVICE);

    int result = dpf_device_acquire_dimensions(*dpf);
    if (result != 0) {
        luaL_error(L, "error: failed to acquire device dimension");
    }

    lua_newtable(L);

    lua_pushstring(L, "width");
    int width = dpf_device_screen_width(*dpf);
    lua_pushinteger(L, width);
    lua_settable(L, -3);

    lua_pushstring(L, "height");
    int height = dpf_device_screen_height(*dpf);
    lua_pushinteger(L, height);
    lua_settable(L, -3);

    return 1;
}

static int _dpf_device_set_brightness(lua_State *L) {
    dpf_device **dpf = luaU_checkoutType(L, t_DPF_DEVICE);
    int level = luaL_checkinteger(L, -1);

    int result = dpf_device_set_brightness(*dpf, level);
    if (result != 0) luaL_error(L, "error: set brightness failed");

    lua_pushinteger(L, dpf_device_get_brightness(*dpf));

    return 1;
}

static int _dpf_device_get_brightness(lua_State *L) {
    dpf_device **dpf = luaU_checkoutType(L, t_DPF_DEVICE);
    lua_pushinteger(L, dpf_device_get_brightness(*dpf));
    return 1;
}

static int _dpf_device_get_bufferSize(lua_State *L) {
    dpf_device **dpf = luaU_checkoutType(L, t_DPF_DEVICE);
    lua_pushinteger(L, dpf_device_get_buffer_size(*dpf));
    return 1;
}

LUA_TYPE_MEMBERS(t_DPF_DEVICE) {
        {"__gc",          _dpf_device_gc},

        {"dimensions",    _dpf_device_dimensions},
        {"brightness",    _dpf_device_get_brightness},
        {"bufferSize",    _dpf_device_get_bufferSize},

        {"setBrightness", _dpf_device_set_brightness},
        {NULL, NULL}
};

/*
 *
 * Dpf canvas
 *
 * */

#define t_DPF_CANVAS "t_DpfCanvas"

static int _dpf_canvas_open(lua_State *L) {
    dpf_device **device = (dpf_device **) luaU_checkoutType(L, t_DPF_DEVICE);

    dpf_canvas **canvas = (dpf_canvas **) lua_newuserdata(L, sizeof(dpf_canvas *));
    int result = dpf_canvas_create(*device, &*canvas);
    if (result != 0) luaL_error(L, "error: create canvas failed");
    luaL_setmetatable(L, t_DPF_CANVAS);

    return 1;
}

static int _dpf_canvas_gc(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);
    dpf_canvas_destroy(*canvas);
    log_trace("[%s gc]", t_DPF_CANVAS);
    return 0;
}

static int _dpf_canvas_flush(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);
    dpf_canvas_flush(*canvas);
    return 0;
}

static int _dpf_canvas_force_flush(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);
    dpf_canvas_force_flush(*canvas);
    return 0;
}

static int _dpf_canvas_clear(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);
    dpf_canvas_clear(*canvas);
    return 0;
}

static int _dpf_canvas_set_point(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    int x = luaL_checkinteger(L, 2);
    if (!(x >= 0 && x < dpf_canvas_get_width(*canvas))) luaL_error(L, "error: x out of boundary");
    int y = luaL_checkinteger(L, 3);
    if (!(y >= 0 && y < dpf_canvas_get_height(*canvas))) luaL_error(L, "error: y out of boundary");

    PointTuple tuple = {x, y};

    Rgba8 color = {
            luaL_checkinteger(L, 4),
            luaL_checkinteger(L, 5),
            luaL_checkinteger(L, 6),
            luaL_checkinteger(L, 7)
    };

    dpf_canvas_set_point(*canvas, &tuple, &color);

    return 0;
}

static int _dpf_canvas_fill_color(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    RectTuple rectTuple = {
            luaL_checkinteger(L, 2),
            luaL_checkinteger(L, 3),
            luaL_checkinteger(L, 4),
            luaL_checkinteger(L, 5)
    };

    Rgba8 color = {
            luaL_checkinteger(L, 6),
            luaL_checkinteger(L, 7),
            luaL_checkinteger(L, 8),
            luaL_checkinteger(L, 9)
    };

    dpf_canvas_fill_color(*canvas, &rectTuple, &color);

    return 0;
}

//static int _dpf_canvas_fill_binary(lua_State* L) {
//
//}

static int _dpf_canvas_get_width(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    lua_pushinteger(L, dpf_canvas_get_width(*canvas));
    return 1;
}

static int _dpf_canvas_get_height(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    lua_pushinteger(L, dpf_canvas_get_height(*canvas));

    return 1;
}

static int _dpf_canvas_set_background_color(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    unsigned char r = luaL_checkinteger(L, 2);
    unsigned char g = luaL_checkinteger(L, 3);
    unsigned char b = luaL_checkinteger(L, 4);

    dpf_canvas_set_background_color(*canvas, r, g, b);
}

static int _dpf_canvas_get_background_color(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    Rgba8 *color = dpf_canvas_get_background_color(*canvas);

    lua_newtable(L);
    luaU_tablePutObjectField(L, color, integer, red);
    luaU_tablePutObjectField(L, color, integer, green);
    luaU_tablePutObjectField(L, color, integer, blue);

    return 1;
}

static int _dpf_canvas_get_rotation(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    lua_pushinteger(L, dpf_canvas_get_rotation(*canvas));
    return 1;
}

static int _dpf_canvas_set_rotation(lua_State *L) {
    dpf_canvas **canvas = (dpf_canvas **) luaU_checkoutType(L, t_DPF_CANVAS);

    unsigned char r = luaL_checkinteger(L, 2);
    dpf_canvas_set_rotation(*canvas, r);
    return 0;
}

LUA_TYPE_MEMBERS(t_DPF_CANVAS) {
        {"__gc",          _dpf_canvas_gc},

        {"width",         _dpf_canvas_get_width},
        {"height",        _dpf_canvas_get_height},
        {"background",    _dpf_canvas_get_background_color},
        {"rotation",      _dpf_canvas_get_rotation},

        {"flush",         _dpf_canvas_flush},
        {"forceFlush",    _dpf_canvas_force_flush},
        {"clear",         _dpf_canvas_clear},

        {"setBackground", _dpf_canvas_set_background_color},
        {"setRotation",   _dpf_canvas_set_rotation},
        {"setPoint",      _dpf_canvas_set_point},
        {"fillRect",      _dpf_canvas_fill_color},

        {NULL, NULL}
};
/*
 *
 *
 *
 * */

LUA_LIB_FUNCTION(dpf_device) {
        {"openDevice", _dpf_device_open},
        {"openCanvas", _dpf_canvas_open},
        {NULL, NULL}
};

LUAMOD_API int luaopen_dpf_device(lua_State *L) {
    luaU_registerType(L, t_DPF_DEVICE);
    luaU_registerType(L, t_DPF_CANVAS);

    luaL_newlib(L, dpf_device_function);
    return 1;
}