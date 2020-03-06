//
// Created by cattenlinger on 20-3-5.
//

#include "mod_canvas.h"

#include "lprefix.h"
#include "lauxlib.h"
#include "lualib.h"

#include "../utils/lua_utils.h"

#include "../com/com_canvas.h"
#include "../utils/log.h"

#define t_CANVAS "t_Canvas"

static int _canvas_create(lua_State *L) {
    unsigned int width = luaL_checkinteger(L, 1);
    unsigned int height = luaL_checkinteger(L, 2);

    ll_canvas *canvas = (ll_canvas *) lua_newuserdata(L, sizeof(ll_canvas));

    int result = ll_canvas_create(canvas, width, height);

    if (result != 0) luaL_error(L, "error: could not create canvas (code %d)", result);

    luaL_setmetatable(L, t_CANVAS);

    return 1;
}

static int _canvas_gc(lua_State *L) {
    log_trace("[%s gc]", t_CANVAS);
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    ll_canvas_destroy(canvas);
    return 0;
}

static int _canvas_clear(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    ll_canvas_clear(canvas);

    return 0;
}

static int _canvas_get_dirty_rect(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    const RectTuple *rect = ll_canvas_get_dirty_rect(canvas);
    lua_newtable(L);
    luaU_tablePutObjectField(L, rect, integer, x0);
    luaU_tablePutObjectField(L, rect, integer, y0);
    luaU_tablePutObjectField(L, rect, integer, x1);
    luaU_tablePutObjectField(L, rect, integer, y1);

    return 1;
}

static int _canvas_get_is_dirty(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    lua_pushboolean(L, ll_canvas_is_dirty(canvas));

    return 1;
}

static int _canvas_set_point(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    PointTuple point = {
            luaL_checkinteger(L, 2),
            luaL_checkinteger(L, 3)
    };

    Rgba8 color = {
            luaL_checkinteger(L, 4),
            luaL_checkinteger(L, 5),
            luaL_checkinteger(L, 6),
            luaL_checkinteger(L, 7)
    };

    ll_canvas_set_point(canvas, &point, &color);

    return 0;
}

static int _canvas_fill_color(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    RectTuple rect = {
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

    ll_canvas_fill_color(canvas, &rect, &color);

    return 0;
}

static int _canvas_get_size(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    lua_newtable(L);
    lua_pushstring(L, "width");
    lua_pushinteger(L, ll_canvas_get_width(canvas));
    lua_settable(L, -3);

    lua_pushstring(L, "height");
    lua_pushinteger(L, ll_canvas_get_height(canvas));
    lua_settable(L, -3);

    return 1;
}

static int _canvas_get_background(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    const Rgba8 *color = ll_canvas_get_background(canvas);

    lua_newtable(L);
    luaU_tablePutObjectField(L, color, integer, red);
    luaU_tablePutObjectField(L, color, integer, green);
    luaU_tablePutObjectField(L, color, integer, blue);
    luaU_tablePutObjectField(L, color, integer, alpha);

    return 1;

}

static int _canvas_set_background(lua_State *L) {
    ll_canvas *canvas = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    Rgba8 color = {
            luaL_checkinteger(L, 2),
            luaL_checkinteger(L, 3),
            luaL_checkinteger(L, 4),
            luaL_checkinteger(L, 5)
    };

    ll_canvas_set_background(canvas, &color);

    return 0;
}

LUA_TYPE_MEMBERS(t_CANVAS) {
        {"__gc",          _canvas_gc},

        {"clear",         _canvas_clear},
        {"getDirtyRect",  _canvas_get_dirty_rect},
        {"isDirty",       _canvas_get_is_dirty},

        {"setPoint",      _canvas_set_point},
        {"fillColor",     _canvas_fill_color},

        {"getSize",       _canvas_get_size},

        {"setBackground", _canvas_set_background},
        {"getBackground", _canvas_get_background},

        {NULL, NULL}
};

LUA_LIB_FUNCTION(t_CANVAS) {
        {"new", _canvas_create},
        {NULL, NULL}
};

LUAMOD_API int luaopen_canvas(lua_State *L) {
    luaU_registerType(L, t_CANVAS);

    luaL_newlib(L, t_CANVAS_function);
    return 1;
}