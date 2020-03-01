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
#include "../utils/lua_utils.h"

#include "mod_usb_explorer.h"

#include "mod_dpf_device.h"

#define t_DPF_DEVICE "t_Dpf.Device"

static int _table_readonly(lua_State *L) {
    luaL_error(L, "error: table readonly");
    return 0;
}

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
 *
 *
 * */

LUA_LIB_FUNCTION(dpf_device) {
        {"open", _dpf_device_open},
        {NULL, NULL}
};

LUAMOD_API int luaopen_dpf_device(lua_State *L) {
    luaU_registerType(L, t_DPF_DEVICE);

    luaL_newlib(L, dpf_device_function);
    return 1;
}