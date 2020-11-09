//
// Created by cattenlinger on 20-3-5.
//

#include "mod_canvas.h"

#include "lauxlib.h"

#include "../utils/lua_utils.h"

#include "../com/com_canvas.h"
#include "../com/com_image.h"
#include "../utils/log.h"

#include "mod_image.h"

#define t_CANVAS "ldpf4l.type.Canvas"

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

static int _canvas_draw_image(lua_State *L) {
    ll_canvas *self = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);
    ll_image *image = (ll_image *) luaU_checkUserDataNotNull(L, MOD_T_IMAGE, 2);

    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);
    RectTuple rectTuple = {
            x,
            y,
            x + image->width - 1,
            y + image->height - 1
    };

    unsigned char blend = lua_toboolean(L, 5);

    ll_canvas_fill_data(self, &rectTuple, (const unsigned int *) image->data, blend);

    return 0;
}

static int _canvas_merge(lua_State *L) {
    ll_canvas *self = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);
    ll_canvas *upper = (ll_canvas *) luaU_checkUserDataNotNull(L, t_CANVAS, 2);

    unsigned char onlyDirtyRect = !lua_toboolean(L, 3);
    unsigned char isUpperDirty = upper->isDirty;
    if (onlyDirtyRect && !isUpperDirty) {
        log_debug("Upper canvas is not dirty, skipped.");
        return 0;
    }

    RectTuple *theRect = onlyDirtyRect ? &upper->dirtyRect : &upper->dimension;

    ll_canvas_copy(self, theRect, upper, 1);

    return 0;
}

static int _canvas_area_copy(lua_State *L) {
    ll_canvas *self = (ll_canvas *) luaU_checkoutSelf(L, t_CANVAS);

    PointTuple point = {
            luaL_checkinteger(L, 2),
            luaL_checkinteger(L, 3)
    };

    ll_canvas *another = (ll_canvas *) luaU_checkUserDataNotNull(L, t_CANVAS, 4);

    RectTuple areaSource = {
            luaL_checkinteger(L, 5),
            luaL_checkinteger(L, 6),
            luaL_checkinteger(L, 7),
            luaL_checkinteger(L, 8)
    };

    unsigned char blend = lua_toboolean(L, 9);

    ll_canvas_copy_area(self, &point, another, &areaSource, blend);

    return 0;
}

static int _canvas_draw_frame(lua_State *L) {
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

    int lineWidth = luaL_checkinteger(L, 10);

    ll_canvas_draw_frame(canvas, &rect, &color, lineWidth);

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

LUA_TYPE_DEFINE(t_CANVAS)
    MEMBER("__gc",          _canvas_gc)

    MEMBER("clear",         _canvas_clear)
    MEMBER("getDirtyRect",  _canvas_get_dirty_rect)
    MEMBER("isDirty",       _canvas_get_is_dirty)

    MEMBER("setPoint",      _canvas_set_point)
    MEMBER("fillColor",     _canvas_fill_color)
    MEMBER("drawFrame",     _canvas_draw_frame)
    MEMBER("drawImage",     _canvas_draw_image)
    MEMBER("drawCanvas",    _canvas_merge)
    MEMBER("copyArea",      _canvas_area_copy)

    MEMBER("getSize",       _canvas_get_size)

    MEMBER("setBackground", _canvas_set_background)
    MEMBER("getBackground", _canvas_get_background)
LUA_TYPE_END

LUA_LIB_DEFINE(ldpf4l_Canvas)
    FUNCTION("new", _canvas_create)
LUA_LIB_END

LUA_LIB_EXPORT(ldpf4l_Canvas)
    EXPORT_TYPE(t_CANVAS)

    EXPORT_LIB(ldpf4l_Canvas)
LUA_LIB_EXPORT_END