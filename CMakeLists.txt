cmake_minimum_required(VERSION 3.13)
project(ldpf4l)
set(CMAKE_C_STANDARD 11)

#[[ Libusb ]]
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

#[[ Lua 5.3 , named "lua_static" ]]
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/lua-5.3.3)

#[[ Find all source files ]]
#file(GLOB_RECURSE SRC src/*.c src/*.h)

#[[ Main entry files ]]
list(APPEND SRC_MAIN
        src/main.c
        src/lua_function_register.h
        src/lua_function_register.c
        )

list(APPEND SRC_MOD
        src/mod/mod_host_log.h
        src/mod/mod_host_log.c
        )

list(APPEND SRC_UTILS
        src/utils/log.h
        src/utils/log.c
        src/utils/lua_utils.h
        src/utils/lua_utils.c
        src/utils/string_wrapper.h
        src/utils/string_wrapper.c
        )

list(APPEND SRC_DPF_DRIVER
        src/utils/common_types.h
        src/utils/common_types.c
        src/driver/dpf/warp_scsi.h
        src/driver/dpf/warp_scsi.c
        src/driver/dpf/dpf_device.h
        src/driver/dpf/dpf_device.c
        src/driver/dpf/dpf_canvas.h
        src/driver/dpf/dpf_canvas.c
        src/driver/dpf/dpf_usb_utils.h
        src/driver/dpf/dpf_usb_utils.c
        )

add_executable(ldpf4l ${SRC_MAIN} ${SRC_MOD} ${SRC_UTILS} ${SRC_DPF_DRIVER})

#target_link_libraries(ldpf4l ${LIBUSB_LIBRARIES} ${PROJECT_SOURCE_DIR}/lib/liblua.a -lm)
# -lm for quick-fixing the compile problem
target_link_libraries(ldpf4l ${LIBUSB_LIBRARIES} lua_static -lm)

target_include_directories(ldpf4l PUBLIC ${LIBUSB_INCLUDE_DIRS})
target_compile_options(ldpf4l PUBLIC ${LIBUSB_CFLAGS_OTHER})