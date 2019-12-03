#include <libusb.h>

int warp_scsi_write(
        libusb_device_handle *device,
        const unsigned char *command,
        int commandLength,
        unsigned char *data,
        unsigned long size
);

int warp_scsi_read(
        libusb_device_handle *device,
        const unsigned char *command,
        int commandLength,
        unsigned char *data,
        unsigned long size
);