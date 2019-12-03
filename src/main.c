//
// Created by cattenlinger on 19-11-15.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>

//#include "utils/logging.h"
#include "utils/log.h"
#include "utils/string_wrapper.h"
#include "utils/lua_utils.h"
#include "lua_function_register.h"

char *mainScriptLocation = "./main.lua";
lua_State *globalLuaState;

static int processArgs(int argc, char **argv) {
#define isArg(s) (s[0] == '-')
#define readShortParam(s) (param[1])

    for (int i = 0; i < argc; ++i) {
        char *param = argv[i];

        if (isArg(param)) {
            switch (readShortParam(param)) {
                case 'c': {
                    if (isArg(argv[i + 1])) {
                        log_error("Value of param -c is not a value.");
                        return 0;
                    }
                    mainScriptLocation = argv[++i];
                    break;
                }

                case 'v': {
                    int level = 3;
                    int p = 1;
                    while (param[p++] != '\0' && level > 0) level--;
                    log_set_level(level);
                    break;
                }

                default:
                    break;
            }
        }
    }

    return 1;

#undef isArg
#undef readShortParam
}

static void createArgTable(lua_State *L, int argc, char **argv) {
    lua_createtable(L, argc, argc);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setglobal(L, "args");
}

static int messageHandler(lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL) {  /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
            return 1;  /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                                  luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
    return 1;  /* return the traceback */
}

static void luaStop(lua_State *L, lua_Debug *ar) {
    (void) ar;  /* unused arg. */
    lua_sethook(L, NULL, 0, 0);  /* reset hook */
    luaL_error(L, "interrupted!");
}

static void luaOnSignal(int i) {
    signal(i, SIG_DFL); /* if another SIGINT happens, terminate process */
    lua_sethook(globalLuaState, luaStop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static int protectedMain(lua_State *L) {

    int argc = (int) lua_tointeger(L, 1);
    char **argv = (char **) lua_touserdata(L, 2);

    String *table1 = luaUtils_dumpStack(L);
    log_debug("Stack table: \n%s", string_value(table1));
    string_free(table1);

    luaL_checkversion(L);

    createArgTable(L, argc, argv);
    log_debug("Created Argument table.");

    luaL_openlibs(L);
    log_debug("Standard libraries has been initialized.");
    luaL_openExtraLibs(L);
    log_debug("Application libraries has been initialized.");

    int loadFileResult = luaL_loadfile(L, mainScriptLocation);
    if (loadFileResult != LUA_OK) {
        log_error("Error when loading file %s.", mainScriptLocation);
        return 0;
    }

    int status;
    int base = lua_gettop(L);
    lua_pushcfunction(L, messageHandler);
    lua_insert(L, base);
    globalLuaState = L;
    signal(SIGINT, luaOnSignal);
    status = lua_pcall(L, 0, 0, base);
    signal(SIGINT, SIG_DFL);
    lua_remove(L, base);

    if (status != LUA_OK) {
        const char *message = lua_tostring(L, -1);
        lua_writestringerror("%s", message);
        lua_pop(L, 1);
    }
    return status;
}

int main(int argc, char **argv) {
    log_set_level(3);

    if (!processArgs(argc, argv)) return 1;

    log_info("Hello world");

    lua_State *luaState = luaL_newstate();
    if (luaState == NULL) {
        log_error("Cannot create LuaState: Not enough memory.");
        return EXIT_FAILURE;
    }
    log_info("%s: Lua machine initialized.");

    lua_pushcfunction(luaState, &protectedMain);
    lua_pushinteger(luaState, argc);  /* 1st argument */
    lua_pushlightuserdata(luaState, argv); /* 2nd argument */

    int status = lua_pcall(luaState, 2, 1, 0);
    int result = lua_toboolean(luaState, -1);

    lua_close(luaState);
    log_info("Lua machine destroyed with result: %d, status: %d.", result, status);
    return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}