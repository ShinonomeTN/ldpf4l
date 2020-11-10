//
// Created by cattenlinger on 20-2-29.
//

#include <libusb.h>

#include <stdio.h>
#include <stdlib.h>

#include "lauxlib.h"
#include "lualib.h"

#include "../utils/log.h"

#include "mod_usb_explorer.h"

static int lf_table_readonly(lua_State *L) {
    luaL_error(L, "error: table readonly");
    return 0;
}


/*
 *
 * Explorer
 *
 * */

#define t_USB_CONTEXT "ldpf4l.type.LibUsbContext"

static int lf_explorer_create_context(lua_State *L) {
    libusb_context **context = (libusb_context **) lua_newuserdata(L, sizeof(libusb_context *));
    *context = NULL;
    int error = libusb_init(&*context);

    if (error < 0) luaL_error(L, "error: open context failed, error %s(%d)", libusb_error_name(error), error);

    if (log_get_level() <= 2) libusb_set_debug(*context, 3);

    luaL_setmetatable(L, t_USB_CONTEXT);
    return 1;
}

#define t_API_VERSION "ldpf4l.type.LibUsbApiVersion"

static int lf_explorer_libusb_api_version(lua_State *L) {
    const struct libusb_version *version = libusb_get_version();
    lua_newtable(L);

    luaU_tablePutObjectField(L, version, integer, major);
    luaU_tablePutObjectField(L, version, integer, minor);
    luaU_tablePutObjectField(L, version, integer, micro);
    luaU_tablePutObjectField(L, version, integer, nano);
    luaU_tablePutObjectField(L, version, string, rc);

    luaL_setmetatable(L, t_API_VERSION);
    return 1;
}

static int lf_libusb_api_version_to_string(lua_State *L) {
    lua_getmetatable(L, -1);
    luaL_getmetatable(L, t_API_VERSION);
    if (!lua_rawequal(L, -1, -2)) luaL_error(L, "error: %s expected", t_API_VERSION);
    lua_pop(L, 2);

    if (lua_getfield(L, -1, "major") != LUA_TNUMBER) luaL_error(L, "number expected");
    int major = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (lua_getfield(L, -1, "minor") != LUA_TNUMBER) luaL_error(L, "number expected");
    int minor = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (lua_getfield(L, -1, "micro") != LUA_TNUMBER) luaL_error(L, "number expected");
    int micro = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (lua_getfield(L, -1, "nano") != LUA_TNUMBER) luaL_error(L, "number expected");
    int nano = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushfstring(L, "%d.%d.%d(%d)", major, minor, micro, nano);
    return 1;
}

LUA_LIB_DEFINE(ldpf4l_UsbExplorer)
    /* Create new t_LibUsbContext */
    MEMBER("newContext", lf_explorer_create_context)

    /* Print LibUsb Api version */
    MEMBER("apiVersion", lf_explorer_libusb_api_version)
LUA_LIB_END

LUA_TYPE_DEFINE(t_API_VERSION)
    MEMBER("__newindex", lf_table_readonly)
    MEMBER("__tostring", lf_libusb_api_version_to_string)
LUA_TYPE_END

/*
 *
 * Libusb Context
 *
 * */

#define t_USB_DEVICE  "ldpf4l.type.LibUsbDevice"

static int lf_context_devices(lua_State *L) {

    libusb_context **context = (libusb_context **) luaU_checkoutSelf(L, t_USB_CONTEXT);

    libusb_device **devices;
    ssize_t count = libusb_get_device_list(*context, &devices);

    if (count < 0)
        luaL_error(L, "error: failed to get device list, error %s(%d)", (int) count, libusb_error_name(count));

    lua_newtable(L); // stack: 0

    // Put all device into new table
    libusb_device *device;
    for (int i = 0; (device = devices[i]) != NULL; i++) {
        // Use current index as the key
        lua_pushinteger(L, i + 1); // stack: 1

        // Store device pointer in lua and give it a type
        libusb_device **handle = (libusb_device **) lua_newuserdata(L, sizeof(libusb_device *)); // stack: 2
        *handle = device;

        luaL_setmetatable(L, t_USB_DEVICE);
        // Put device into table
        lua_settable(L, -3);
    }

    return 1;
}

static int lf_context_gc(lua_State *L) {
    log_trace("[%s gc]", t_USB_CONTEXT);
    libusb_context **context = (libusb_context **) luaU_checkoutSelf(L, t_USB_CONTEXT);
    libusb_exit(*context);
    return 0;
}

LUA_TYPE_DEFINE(t_USB_CONTEXT)
    MEMBER("__newindex", lf_table_readonly)
    MEMBER("__gc", lf_context_gc)

    /* List all device handler */
    MEMBER("devices", lf_context_devices)
LUA_TYPE_END

/*
 *
 * LibUsb device
 *
 * */

#define t_DEVICE_INFO  "ldpf4l.type.LibUsbDeviceInfo"

typedef struct libusb_device_descriptor device_descriptor;

static int lf_usb_device_info(lua_State *L) {

    libusb_device **device = (libusb_device **) luaU_checkoutSelf(L, t_USB_DEVICE);
    device_descriptor d;

    int error = libusb_get_device_descriptor(*device, &d);
    if (error < 0) luaL_error(L, "error: failed to get device list, error %s(%d)", error, libusb_error_name(error));

    lua_newtable(L);

    luaU_tablePutStructField(L, d, integer, bcdDevice);
    luaU_tablePutStructField(L, d, integer, bcdUSB);
    luaU_tablePutStructField(L, d, integer, bDeviceClass);
    luaU_tablePutStructField(L, d, integer, bDeviceProtocol);
    luaU_tablePutStructField(L, d, integer, bDeviceSubClass);
    luaU_tablePutStructField(L, d, integer, bMaxPacketSize0);
    luaU_tablePutStructField(L, d, integer, bNumConfigurations);
    luaU_tablePutStructField(L, d, integer, idProduct);
    luaU_tablePutStructField(L, d, integer, idVendor);
    luaU_tablePutStructField(L, d, integer, iManufacturer);
    luaU_tablePutStructField(L, d, integer, iProduct);
    luaU_tablePutStructField(L, d, integer, iSerialNumber);

    luaL_setmetatable(L, t_DEVICE_INFO);
    return 1;
}

static int lf_usb_device_to_string(lua_State *L) {
    luaU_checkoutSelf(L, t_USB_DEVICE);
    lua_pushfstring(L, "[%s *]", t_USB_DEVICE);
    return 1;
}

/*
 *
 * LibUsb config descriptor
 *
 * */

#define t_DEVICE_CONFIG  "ldpf4l.type.LibUsbDeviceConfig"

typedef struct libusb_config_descriptor config_descriptor;

static int lf_usb_device_configuration(lua_State *L) {
    libusb_device **device = luaU_checkoutSelf(L, t_USB_DEVICE);
    int configIndex = luaL_checkinteger(L, 2);

    config_descriptor **config = (config_descriptor **) lua_newuserdata(L, sizeof(config_descriptor *));
    libusb_get_config_descriptor(*device, configIndex, &*config);
    luaL_setmetatable(L, t_DEVICE_CONFIG);

    return 1;
}

LUA_TYPE_DEFINE(t_USB_DEVICE)
    MEMBER("__tostring", lf_usb_device_to_string)

    /* Return a table contains device information */
    MEMBER("info", lf_usb_device_info)

    /* Get device configuration descriptor */
    MEMBER("config", lf_usb_device_configuration)
LUA_TYPE_END



/*
 *
 * Usb config
 *
 * */

#define t_USB_INTERFACE "ldpf4l.type.LibUsbDeviceInterface"
#define t_USB_ENDPOINT "ldpf4l.type.LibUsbDeviceEndpoint"

typedef struct libusb_interface usb_interface;
typedef struct libusb_interface_descriptor interface_descriptor;

int usb_interface_read_endpoint_list(lua_State *L, const interface_descriptor *descriptor) {
    int count = descriptor->bNumEndpoints;
    lua_newtable(L);

    for (int i = 0; i < count; ++i) {
        const struct libusb_endpoint_descriptor *endpoint = &(descriptor->endpoint[i]);

        lua_pushinteger(L, i + 1);
        lua_newtable(L);

        luaU_tablePutObjectField(L, endpoint, integer, bLength);
        luaU_tablePutObjectField(L, endpoint, integer, bEndpointAddress);
        luaU_tablePutObjectField(L, endpoint, integer, bmAttributes);
        luaU_tablePutObjectField(L, endpoint, integer, wMaxPacketSize);
        luaU_tablePutObjectField(L, endpoint, integer, bInterval);
        luaU_tablePutObjectField(L, endpoint, integer, bRefresh);
        luaU_tablePutObjectField(L, endpoint, integer, bSynchAddress);

        int extra_length = descriptor->extra_length;
        luaU_tablePutObjectFieldLString(L, endpoint, extra, extra_length);

        luaL_setmetatable(L, t_USB_ENDPOINT);

        lua_settable(L, -3);
    }
    return 1;
}

int usb_config_read_interface(lua_State *L, const usb_interface *usbInterface) {
    int count = usbInterface->num_altsetting;

    lua_newtable(L);
    for (int i = 0; i < count; ++i) {
        const interface_descriptor *descriptor = &(usbInterface->altsetting[i]);

        luaU_tablePutObjectField(L, descriptor, integer, bInterfaceNumber);
        luaU_tablePutObjectField(L, descriptor, integer, bAlternateSetting);
        luaU_tablePutObjectField(L, descriptor, integer, bNumEndpoints);
        luaU_tablePutObjectField(L, descriptor, integer, bInterfaceClass);
        luaU_tablePutObjectField(L, descriptor, integer, bInterfaceSubClass);
        luaU_tablePutObjectField(L, descriptor, integer, bInterfaceProtocol);
        luaU_tablePutObjectField(L, descriptor, integer, iInterface);

        int extra_length = descriptor->extra_length;
        luaU_tablePutObjectFieldLString(L, descriptor, extra, extra_length);

        luaU_tableKV(L, lua_pushstring(L, "endpointList"), usb_interface_read_endpoint_list(L, descriptor));
    }

    luaL_setmetatable(L, t_USB_INTERFACE);
    return 1;
}

static int lf_usb_config_interfaces(lua_State *L) {
    config_descriptor **config = (config_descriptor **) luaU_checkoutSelf(L, t_DEVICE_CONFIG);
    int interfaceCount = (*config)->bNumInterfaces;

    lua_newtable(L);
    for (int i = 0; i < interfaceCount; i++) {
        // Create interface
        const usb_interface *interface = &(*config)->interface[i];
        luaU_tableKV(L, lua_pushinteger(L, i + 1), usb_config_read_interface(L, interface));
    }

    return 1;
}

static int lf_usb_device_config_info(lua_State *L) {
    config_descriptor **config = luaU_checkoutSelf(L, t_DEVICE_CONFIG);
    lua_newtable(L);

    luaU_tablePutObjectField(L, *config, integer, bNumInterfaces);
    luaU_tablePutObjectField(L, *config, integer, bNumInterfaces);
    luaU_tablePutObjectField(L, *config, integer, bConfigurationValue);
    luaU_tablePutObjectField(L, *config, integer, iConfiguration);
    luaU_tablePutObjectField(L, *config, integer, bmAttributes);
    luaU_tablePutObjectField(L, *config, integer, MaxPower);

    int extra_length = (*config)->extra_length;
    luaU_tablePutObjectFieldLString(L, *config, extra, extra_length);

    return 1;
}

static int lf_usb_device_config_gc(lua_State *L) {
    log_trace("[%s gc]", t_DEVICE_CONFIG);
    config_descriptor **config = luaU_checkoutSelf(L, t_DEVICE_CONFIG);
    libusb_free_config_descriptor(*config);
    return 0;
}

LUA_TYPE_DEFINE(t_DEVICE_CONFIG)
    MEMBER("__newindex", lf_table_readonly)
    MEMBER("__gc", lf_usb_device_config_gc)
    MEMBER("info", lf_usb_device_config_info)
    MEMBER("interfaces", lf_usb_config_interfaces)
LUA_TYPE_END

LUA_TYPE_DEFINE(t_USB_INTERFACE)
    MEMBER("__newindex", lf_table_readonly)
LUA_TYPE_END

LUA_TYPE_DEFINE(t_USB_ENDPOINT)
    MEMBER("__newindex", lf_table_readonly)
LUA_TYPE_END

/*
 *
 *
 * */

LUA_LIB_EXPORT(ldpf4l_UsbExplorer)
    EXPORT_TYPE(t_USB_CONTEXT)
    EXPORT_TYPE(t_USB_DEVICE)
    EXPORT_TYPE(t_API_VERSION)
    EXPORT_TYPE(t_DEVICE_CONFIG)
    EXPORT_TYPE(t_USB_INTERFACE)
    EXPORT_TYPE(t_USB_ENDPOINT)

    EXPORT_LIB(ldpf4l_UsbExplorer)
LUA_LIB_EXPORT_END