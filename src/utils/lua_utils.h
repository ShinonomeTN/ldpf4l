//
// Created by cattenlinger on 19-11-22.
//

#ifndef LUA_TEST_LUA_UTILS_H
#define LUA_TEST_LUA_UTILS_H

#define luaU_checkoutType(L, typeName) luaU_checkUserDataNotNull(L, typeName, 1)

#define LUA_LIB_FUNCTION(name) \
static const struct luaL_Reg name##_function[] =

#define LUA_TYPE_MEMBERS(name) \
static const struct luaL_Reg name##_method[] =

/*
 * Lua library define macro
 */
#define LUA_LIB_DEFINE(name) \
static const struct luaL_Reg name##_function[] = {

#define FUNCTION(lua_name, c_method) \
    { lua_name, c_method },

#define LUA_LIB_END \
    {NULL, NULL} \
};

/*
 * Lua type define macro
 */

#define LUA_TYPE_DEFINE(name) \
static const struct luaL_Reg name##_method[] = {

#define MEMBER(field_name, function) \
{ field_name, function },

#define LUA_TYPE_END \
{ NULL, NULL } \
};

/*
 *
 *
 */

#define LUA_LIB_EXPORT(name) \
LUAMOD_API int luaopen_##name(lua_State *L) {

#define EXPORT_TYPE(name) do {\
    luaL_newmetatable(L, name);\
    lua_pushvalue(L, -1);\
    lua_setfield(L, -2, "__index");\
    luaL_setfuncs(L, name ## _method, 0);\
} while(0);

#define EXPORT_LIB(name) \
    luaL_newlib(L, name##_function);

#define LUA_LIB_EXPORT_END \
    return 1;\
}

/*
 *
 */

#define LUA_LIB_EXPORT_DEFINE(name) \
LUAMOD_API int (luaopen_##name)(lua_State *L);

/*
 *
 */
#define luaU_registerType(L, name) do {\
    luaL_newmetatable(L, #name );\
    lua_pushvalue(L, -1);\
    lua_setfield(L, -2, "__index");\
    luaL_setfuncs(L, name ## _method, 0);\
} while(0)

#define luaU_tablePutStructField(L, _struct, type, name) do {\
    lua_pushstring(L, #name);\
    lua_push##type(L, _struct.name);\
    lua_settable(L, -3);\
} while (0)

#define luaU_tablePutObjectField(L, _obj, type, name) do {\
    lua_pushstring(L, #name);\
    lua_push##type(L, (_obj)->name);\
    lua_settable(L, -3);\
} while (0)

#define luaU_tablePutObjectFieldLString(L, _obj, name, len) do {\
    lua_pushstring(L, #name);\
    lua_pushlstring(L, ((const char *)(_obj)->name), len);\
    lua_settable(L, -3);\
} while (0)

#define luaU_tableKV(L, kStatement, vStatement) do {\
    kStatement;\
    vStatement;\
    lua_settable(L, -3);\
}while(0)

void *luaU_checkUserDataNotNull(lua_State *L, const char *typeName, int pos);

#endif //LUA_TEST_LUA_UTILS_H
