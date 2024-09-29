#include "warp_scsi.h"

#include <stdlib.h>
#include <string.h>

#include "../../utils/log.h"

#define DEVICE_ENDPOINT_OUT 1
#define DEVICE_ENDPOINT_IN  0x81

#define GLOBAL_SCSI_BUFFER_SIZE 31
const static uint8_t global_scsi_buffer_template[GLOBAL_SCSI_BUFFER_SIZE] = {
    0x55, 0x53, 0x42, 0x43, // dCBWSignature (0 - 3)
    0xde, 0xad, 0xbe, 0xef, // dCBWTag (4 - 7)
    0x00, 0x80, 0x00, 0x00, // dCBWLength (8 - 11)
    0x00, // bmCBWFlags: 0x80: data in (dev to host), 0x00: Data out (12)
    0x00, // bCBWLUN (13)
    0x10, // bCBWCBLength (14)

    // SCSI cmd: (15 - 30)
    0xcd, 0x00, 0x00, 0x00,
    0x00, 0x06, 0x11, 0xf8,
    0x70, 0x00, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

typedef struct warp_scsi_ctx_t {
    uint8_t buffer[GLOBAL_SCSI_BUFFER_SIZE];
    libusb_device_handle *device;
} warp_scsi_ctx_t;


int32_t warp_scsi_create_ctx(warp_scsi_ctx_ptr *scsi_ctx, libusb_device_handle *device) {
    const warp_scsi_ctx_ptr ctx = malloc(sizeof(warp_scsi_ctx_t));
    if (ctx == NULL) return 0;
    ctx->device = device;

    memcpy(ctx->buffer, global_scsi_buffer_template, sizeof(global_scsi_buffer_template));

    *scsi_ctx = ctx;
    log_debug("SCSI device context created.");
    return 1;
}

void warp_scsi_destroy_ctx(warp_scsi_ctx_ptr ctx) {
    free(ctx);
}

libusb_device_handle *warp_scsi_get_device(const warp_scsi_ctx_ptr ctx) {
    return ctx->device;
}

void warp_scsi_set_command(uint8_t cmd_buf[], const uint8_t cmd[], uint8_t cmd_len);

void warp_scsi_set_data_size(uint8_t *cmd_buf, uint32_t data_len);

int32_t warp_scsi_cmd_prepare(uint8_t *cmd_buf, libusb_device_handle *device);

int32_t warp_scsi_operation_result(libusb_device_handle *device);

int32_t warp_scsi_write(warp_scsi_ctx_t *ctx,
                        const uint8_t *cmd_buf, const uint8_t cmd_len,
                        uint8_t *data_buf, const uint32_t data_len) {
    warp_scsi_set_command(ctx->buffer, cmd_buf, cmd_len);
    warp_scsi_set_data_size(ctx->buffer, data_len);

    libusb_device_handle *device = ctx->device;

    int32_t result = 0;
    result = warp_scsi_cmd_prepare(ctx->buffer, device);

    if (result < 0) return result;

    // Only write data when data buffer pointer was given
    if (data_buf) {
        int transferred = 0;
        result = libusb_bulk_transfer(
            device, DEVICE_ENDPOINT_OUT,
            data_buf, data_len,
            &transferred, 3000
        );

        if (result < 0)
            log_error("Error while write data to device. result: %s(code %d).\n",
                  result < 0 ? libusb_error_name(result) : "",
                  result
        );
        if (transferred != data_len)
            log_error("Write size doesn't equals to expected.");
        // if has error, fail fast
        if (result < 0) return result;
    }

    return warp_scsi_operation_result(device);
}

int32_t warp_scsi_read(warp_scsi_ctx_t *ctx,
                       const uint8_t *cmd_buf, const uint8_t cmd_len,
                       uint8_t *data_buf, const uint32_t data_len) {
    warp_scsi_set_command(ctx->buffer, cmd_buf, cmd_len);
    warp_scsi_set_data_size(ctx->buffer, data_len);

    libusb_device_handle *device = ctx->device;

    int32_t result = 0;

    result = warp_scsi_cmd_prepare(ctx->buffer, device);
    if (result < 0) return result;

    // Only read data when data buffer pointer was given
    if (data_buf) {
        int transferred = 0;
        result = libusb_bulk_transfer(
            device, DEVICE_ENDPOINT_IN,
            data_buf, data_len,
            &transferred, 3000
        );
        if (result < 0)
            log_error(
            "Error while read data from device. result: %s(code %d).",
            libusb_error_name(result),
            result
        );
        if (transferred != data_len)
            log_error("Write size doesn't equals to expected.");

        // if has error, fail fast
        if (result < 0) return result;
    }

    return warp_scsi_operation_result(device);
}

/*

    Private part

 */

void warp_scsi_set_command(uint8_t cmd_buf[], const uint8_t cmd[], const uint8_t cmd_len) {
    cmd_buf[14] = cmd_len;
    memcpy(&cmd_buf[15], cmd, cmd_len);
}

void warp_scsi_set_data_size(uint8_t cmd_buf[], const uint32_t data_len) {
    cmd_buf[8] = data_len;
    cmd_buf[9] = data_len >> 8u;
    cmd_buf[10] = data_len >> 16u;
    cmd_buf[11] = data_len >> 24u;
}

int32_t warp_scsi_cmd_prepare(uint8_t cmd_buf[], libusb_device_handle *device) {
    int transferred = 0;

    const int32_t result = libusb_bulk_transfer(
        device, DEVICE_ENDPOINT_OUT,
        cmd_buf,GLOBAL_SCSI_BUFFER_SIZE,
        &transferred,
        1000
    );

    if (result < 0)
        log_error(
        "Error while write data to device: %s(code %d).",
        libusb_error_name(result),
        result
    );

    return result;
}

int32_t warp_scsi_operation_result(libusb_device_handle *device) {
    // get ACK:
    uint8_t str_buf[13];

    int32_t retry_cnt = 0;
    int32_t timeout = 0;
    int32_t result = 0;

    do {
        const int32_t str_len = sizeof(str_buf);
        timeout = 0;
        int32_t transferred = 0;

        result = libusb_bulk_transfer(device,DEVICE_ENDPOINT_IN, str_buf, str_len, &transferred, 5000);

        if (transferred != str_len) {
            timeout = 1;
            log_error("Read size doesn't equals to expected.");
        }

        if (result < 0)
            log_error(
            "Error while read data from device. result: %s(code %d).",
            libusb_error_name(result),
            result
        );

        retry_cnt++;
    } while (timeout && retry_cnt < 5);

    if (strncmp(str_buf, "USBS", 4) != 0) {
        log_error("Got invalid reply.");
        return -1;
    }

    return str_buf[12];
}
