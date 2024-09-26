cmake_minimum_required(VERSION 3.13)
project(ldpf4l)
set(CMAKE_C_STANDARD 11)


find_package(PkgConfig REQUIRED)
pkg_check_modules(Lua53 REQUIRED lua5.3)

#[[ Libusb ]]
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

include_directories(/usr/local/include ${Lua53_INCLUDE_DIRS} ${LIBUSB_INCLUDE_DIRS})

list(APPEND SRC_LIB
        src/lib/stb_image.h
        )

list(APPEND SRC_COM
        src/com/com_image.h
        src/com/com_image.c
        src/com/com_canvas.h
        src/com/com_canvas.c
        src/com/com_screen_device.h
        src/com/com_screen_device.c
        )

list(APPEND SRC_MOD
        src/mod/mod_image.h
        src/mod/mod_image.c
        src/mod/mod_canvas.h
        src/mod/mod_canvas.c
        src/mod/mod_screen.h
        src/mod/mod_screen.c
        src/mod/mod_buffer.h
        src/mod/mod_buffer.c
        src/mod/mod_host_log.h
        src/mod/mod_host_log.c
        src/mod/mod_num_array.h
        src/mod/mod_num_array.c
        src/mod/mod_usb_explorer.h
        src/mod/mod_usb_explorer.c
        )

list(APPEND SRC_UTILS
        src/utils/log.h
        src/utils/log.c
        src/utils/lua_utils.h
        src/utils/lua_utils.c
        src/utils/common_types.h
        src/utils/common_types.c
        )

list(APPEND SRC_DPF_DRIVER
        src/driver/dpf/warp_scsi.h
        src/driver/dpf/warp_scsi.c
        src/driver/dpf/dpf_device.h
        src/driver/dpf/dpf_device.c
        )

add_compile_definitions(
        LOG_USE_COLOR
        #        LOGGER_DISABLE_DEBUG_INFO
        STB_IMAGE_IMPLEMENTATION
        STB_IMAGE_STATIC
        STBI_NO_PSD
        STBI_NO_TGA
        STBI_NO_GIF
        STBI_NO_HDR
        STBI_NO_PIC
        STBI_NO_PNM

        STBI_NO_SIMD
)

add_library(
        ldpf4l
        MODULE
        ${SRC_UTILS}
        ${SRC_LIB}
        ${SRC_COM} ${SRC_MOD} ${SRC_DPF_DRIVER}
)

set_target_properties(ldpf4l PROPERTIES PREFIX "")
target_compile_options(ldpf4l PUBLIC ${LIBUSB_CFLAGS_OTHER} ${Lua53_CFLAGS_OTHER})

target_link_libraries(ldpf4l ${LIBUSB_LIBRARIES})
