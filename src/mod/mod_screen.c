//
// Created by cattenlinger on 20-3-4.
//

#include "lauxlib.h"
#include "lualib.h"

#include "../utils/lua_utils.h"

#include "mod_usb_explorer.h"
#include "../driver/dpf/dpf_device.h"

#include "mod_screen.h"
#include "../com/com_canvas.h"

#include "mod_canvas.h"
#include "../utils/log.h"

#define t_SCREEN_DEVICE "ldpf4l.type.screen"

#define CLASS_INVOKE(instance, method, ...) (instance->class->method(device, __VA_ARGS__))
#define CLASS_IMPLEMENTED(instance, method) (instance->class->method != NULL)

static int ll_screen_open_dpf(lua_State *L) {
    libusb_device **usbDevice = (libusb_device **) luaU_checkUserDataNotNull(L, MOD_t_USB_DEVICE, 1);
    ll_screen_device *screen = (ll_screen_device *) lua_newuserdata(L, sizeof(ll_screen_device));
    int result = dpf_open_screen_device(*usbDevice, screen);

    if (result != 0) {
        lua_pushnil(L);
        lua_pushfstring(L,
                        "error: could not open screen device using dpf driver, error %s(%d)",
                        libusb_error_name(result),
                        result);

        return 2;
    }

    luaL_setmetatable(L, t_SCREEN_DEVICE);
    return 1;
}

static int ll_screen_gc(lua_State *L) {
    log_trace("[%s gc]", t_SCREEN_DEVICE);
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    if (device != NULL) device->class->destroy(device);
    return 0;
}

static int ll_screen_get_backlight(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    int l;
    CLASS_INVOKE(device, get_backlight_level, &l);
    lua_pushinteger(L, l);
    return 1;
}

static int ll_screen_set_backlight(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    int level = luaL_checkinteger(L, 2);
    if (!CLASS_IMPLEMENTED(device, set_backlight_level)) {
        lua_pushnil(L);
        lua_pushstring(L, "not supported by driver");
        return 2;
    }

    int result = CLASS_INVOKE(device, set_backlight_level, level);
    if (result != 0) {
        lua_pushnil(L);
        lua_pushfstring(L, "error: %s(%d)", libusb_error_name(result), result);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int ll_screen_get_background(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);

    if (!CLASS_IMPLEMENTED(device, get_background)) {
        lua_pushnil(L);
        lua_pushstring(L, "not supported by driver");
        return 2;
    }

    Rgba8 c;
    CLASS_INVOKE(device, get_background, &c);
    lua_newtable(L);
    luaU_tablePutStructField(L, c, integer, red);
    luaU_tablePutStructField(L, c, integer, green);
    luaU_tablePutStructField(L, c, integer, blue);

    lua_pushboolean(L, 1);
    return 1;
}

static int ll_screen_set_background(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);

    if (!CLASS_IMPLEMENTED(device, set_background)) {
        lua_pushnil(L);
        lua_pushstring(L, "not supported by driver");
        return 2;
    }

    luaL_checktype(L, 2, LUA_TTABLE);
    if (lua_getfield(L, 2, "red") != LUA_TNUMBER) luaL_error(L, "error: integer expected");
    unsigned char red = lua_tointeger(L, -1);
    if (lua_getfield(L, 2, "green") != LUA_TNUMBER) luaL_error(L, "error: integer expected");
    unsigned char green = lua_tointeger(L, -1);
    if (lua_getfield(L, 2, "blue") != LUA_TNUMBER) luaL_error(L, "error: integer expected");
    unsigned char blue = lua_tointeger(L, -1);
    Rgba8 color = {
            red,
            green,
            blue
    };

    CLASS_INVOKE(device, set_background, &color);

    lua_pushboolean(L, 1);
    return 1;
}

static int ll_screen_get_driver_name(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    lua_pushstring(L, device->name);
    return 1;
}

static int ll_screen_get_color_depth(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    lua_pushinteger(L, device->colorDepth);
    return 1;
}

static int ll_screen_draw(lua_State *L) {
    int forceFlush = lua_toboolean(L, 3);
    ll_canvas *canvas = (ll_canvas *) luaU_checkUserDataNotNull(L, MOD_t_CANVAS, 2);
    int canvasDirty = ll_canvas_is_dirty(canvas);
    if (!(forceFlush || canvasDirty)) return 0;
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);

    int result;

    if (canvasDirty) {
        result = CLASS_INVOKE(device, flush, ll_canvas_get_buffer(canvas), ll_canvas_get_dirty_rect(canvas));
    } else {
        RectTuple rectTuple = {
                0, 0, ll_canvas_get_width(canvas) - 1, ll_canvas_get_height(canvas) - 1
        };
        result = CLASS_INVOKE(device, flush, ll_canvas_get_buffer(canvas), &rectTuple);
    }

    if (result != 0) {
        lua_pushnil(L);
        lua_pushfstring(L, "error: could not draw screen, error %s(%d)", libusb_error_name(result), result);
        return 2;
    }

    ll_canvas_reset_dirty_rect(canvas);

    lua_pushboolean(L, 1);
    return 1;
}

static int ll_screen_get_size(lua_State *L) {
    ll_screen_device *device = (ll_screen_device *) luaU_checkoutSelf(L, t_SCREEN_DEVICE);
    unsigned int width = 0;
    unsigned int height = 0;

    CLASS_INVOKE(device, get_width, &width);
    CLASS_INVOKE(device, get_height, &height);
    lua_newtable(L);

    lua_pushstring(L, "width");
    lua_pushinteger(L, width);
    lua_settable(L, -3);

    lua_pushstring(L, "height");
    lua_pushinteger(L, height);
    lua_settable(L, -3);

    return 1;
}

LUA_LIB_DEFINE(ldpf4l_Screen)
    FUNCTION("openDpf", ll_screen_open_dpf)
LUA_LIB_END

LUA_TYPE_DEFINE(t_SCREEN_DEVICE)
    MEMBER("__gc", ll_screen_gc)

    MEMBER("getDriverName", ll_screen_get_driver_name)
    MEMBER("getColorDepth", ll_screen_get_color_depth)
    MEMBER("getSize", ll_screen_get_size)

    MEMBER("getBacklightLevel", ll_screen_get_backlight)
    MEMBER("setBackLightLevel", ll_screen_set_backlight)
    MEMBER("getBackground", ll_screen_get_background)
    MEMBER("setBackground", ll_screen_set_background)

    MEMBER("draw", ll_screen_draw)
LUA_TYPE_END

LUA_LIB_EXPORT(ldpf4l_Screen)
    EXPORT_TYPE(t_SCREEN_DEVICE)

    EXPORT_LIB(ldpf4l_Screen)
LUA_LIB_EXPORT_END