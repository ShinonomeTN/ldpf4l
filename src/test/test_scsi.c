#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unitypes.h>
#include "../driver/dpf/warp_scsi.h"

int main(int argc, char **argv) {
    int32_t result = 0;
    libusb_context *ctx = NULL;

    result = libusb_init(&ctx);
    if (result < 0) {
        fprintf(stderr, "libusb_init failed\n");
        exit(-1);
    }
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);

    libusb_device **list = NULL;
    result = libusb_get_device_list(ctx, &list);
    if (result < 0) {
        fprintf(stderr, "libusb_get_device_list failed\n");
        exit(-1);
    }

    libusb_device *device = NULL;
    int found = 0;
    struct libusb_device_descriptor desc;
    for(int i = 0; (device = list[i]) != NULL; i++) {
        libusb_get_device_descriptor(device, &desc);
        if (desc.idVendor == 0x1908 && desc.idProduct == 0x0102) {
            found = 1;
            break;
        }
    }
    if(!found) {
        fprintf(stderr, "Device not found\n");
        exit(-1);
    }

    fprintf(stderr, "Device found: vid %X pid %X\n", desc.idVendor, desc.idProduct);
    libusb_device_handle *handle = NULL;
    result = libusb_open(device, &handle);
    if (result < 0) {
        fprintf(stderr, "libusb_open failed\n");
        exit(-1);
    }

    warp_scsi_ctx_t *scsi_ctx = NULL;
    result = warp_scsi_create_ctx(&scsi_ctx, handle);
    if (result < 0) {
        fprintf(stderr, "warp_scsi_create_ctx failed\n");
        libusb_close(handle);
        exit(-1);
    }

    static const uint8_t SCSI_CMD_READ[16] = {
        0xcd, 0x00, 0x00, 0x00,
        0x00, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    uint8_t buffer[5];
    result = warp_scsi_read(scsi_ctx, SCSI_CMD_READ, sizeof(SCSI_CMD_READ), buffer, 5);
    if (result < 0) {
        fprintf(stderr, "warp_scsi_read failed\n");
        libusb_close(handle);
        exit(-1);
    }
    const uint16_t* data = (uint16_t*)buffer;
    printf("SCSI command read %04X\n, Width : %d", data[0], data[0]);
    printf("SCSI command read %04X\n, Height: %d", data[1], data[1]);

    libusb_close(handle);
    return 0;
}