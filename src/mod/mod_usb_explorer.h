//
// Created by cattenlinger on 20-2-29.
//

#ifndef LDPF4L_MOD_USB_EXPLORER_H
#define LDPF4L_MOD_USB_EXPLORER_H

#include "lua.h"

#define LUA_USB_EXPLORER_NAME "UsbExplorer"

const static char *MOD_t_USB_DEVICE = "t_LibUsbDevice";

const static char *MOD_t_USB_CONTEXT = "t_LibUsbContext";

LUAMOD_API int (luaopen_usb_explorer)(lua_State *L);

#endif //LDPF4L_MOD_USB_EXPLORER_H
