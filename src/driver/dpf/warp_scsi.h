#include <libusb.h>

#ifndef LDPF4L_DPF_WARP_SCSI_H
#define LDPF4L_DPF_WARP_SCSI_H

#include "../../utils/common_types.h"

int warp_scsi_write(
        libusb_device_handle *device,
        const byte *command,
        int commandLength,
        byte *data,
        uint size
);

int warp_scsi_read(
        libusb_device_handle *device,
        const byte *command,
        int commandLength,
        byte *data,
        uint size
);

#endif // LDPF4L_DPF_WARP_SCSI_H