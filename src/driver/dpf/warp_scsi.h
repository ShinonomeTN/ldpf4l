#include <unitypes.h>
#include <libusb.h>

#ifndef LDPF4L_DPF_WARP_SCSI_H
#define LDPF4L_DPF_WARP_SCSI_H

/**
 * SCSI Wrapper Context Struct
 */
typedef struct warp_scsi_ctx_t warp_scsi_ctx_t;
typedef warp_scsi_ctx_t* warp_scsi_ctx_ptr;

/**
 * Create SCSI Wrapper Context
 *
 * @param scsi_ctx Context Pointer
 * @param device USB device handle
 * @return If success, returns 0
 */
int32_t warp_scsi_create_ctx(warp_scsi_ctx_ptr *scsi_ctx, libusb_device_handle *device);

/**
 * Destroy a SCSI Wrapper Context
 *
 * @param ctx Context Pointer
 */
void warp_scsi_destroy_ctx(warp_scsi_ctx_ptr ctx);

/**
 * Get the related usb device handle
 *
 * @param ctx context
 * @return usb device
 */
libusb_device_handle* warp_scsi_get_device(warp_scsi_ctx_ptr ctx);

/**
 * Execute USB SCSI Write Command
 *
 * @param ctx Context Pointer
 * @param cmd_buf Write command content
 * @param cmd_len Write command content length
 * @param data_buf Data buffer
 * @param data_len Data buffer length
 * @return status code
 */
int32_t warp_scsi_write(
 warp_scsi_ctx_ptr ctx,
 const uint8_t *cmd_buf,
 uint8_t cmd_len,
 uint8_t *data_buf,
 uint32_t data_len
);

/**
 * Execute USB SCSI Read Command
 *
 * @param ctx Context Pointer
 * @param cmd_buf Read command content
 * @param cmd_len Read command content length
 * @param data_buf Data buffer
 * @param data_len Data buffer length
 * @return status code
 */
int32_t warp_scsi_read(
 warp_scsi_ctx_ptr ctx,
 const uint8_t *cmd_buf,
 uint8_t cmd_len,
 uint8_t *data_buf,
 uint32_t data_len
);

#endif // LDPF4L_DPF_WARP_SCSI_H
