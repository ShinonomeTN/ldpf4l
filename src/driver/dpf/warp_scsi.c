#include "warp_scsi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/log.h"

#define USB_ENDPOINT_OUT 1
#define USB_ENDPOINT_IN  0x81

static unsigned char global_scsi_buffer_cmd[] = {
        0x55,
        0x53,
        0x42,
        0x43, // dCBWSignature
        0xde,
        0xad,
        0xbe,
        0xef, // dCBWTag
        0x00,
        0x80,
        0x00,
        0x00, // dCBWLength
        0x00, // bmCBWFlags: 0x80: data in (dev to host), 0x00: Data out
        0x00, // bCBWLUN
        0x10, // bCBWCBLength

        // SCSI cmd:
        0xcd,
        0x00,
        0x00,
        0x00,
        0x00,
        0x06,
        0x11,
        0xf8,
        0x70,
        0x00,
        0x40,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
};

void warp_scsi_set_command(const unsigned char *command, int commandLength);

void warp_scsi_set_data_size(unsigned long dataSize);

int warp_scsi_cmd_prepare(libusb_device_handle *device);

int warp_scsi_operation_result(libusb_device_handle *device);

int warp_scsi_write(libusb_device_handle *device,
                    const unsigned char *command, int commandLength,
                    unsigned char *data, const unsigned long size) {
    warp_scsi_set_command(command, commandLength);
    warp_scsi_set_data_size(size);

    int prepareResult = warp_scsi_cmd_prepare(device);
    if (prepareResult < 0) return prepareResult;

    // Only write data when data buffer pointer was given
    if (data) {
        int transferred = 0;
        int transferResult = libusb_bulk_transfer(device, USB_ENDPOINT_OUT, data, size, &transferred, 3000);

        if (transferResult < 0)
            log_error("Error while write data to device. result: %s(code %d).\n",
                      transferResult < 0 ? libusb_error_name(transferResult) : "",
                      transferResult
            );
        if (transferred != size) log_error("Write size doesn't equals to expected.");
        // if has error, fail fast
        if (transferResult < 0) return transferResult;
    }

    return warp_scsi_operation_result(device);
}

int warp_scsi_read(
        libusb_device_handle *device,
        const unsigned char *command, const int commandLength,
        unsigned char *data, const unsigned long size) {

    warp_scsi_set_command(command, commandLength);
    warp_scsi_set_data_size(size);

    int prepareResult = warp_scsi_cmd_prepare(device);
    if (prepareResult < 0) return prepareResult;

    // Only read data when data buffer pointer was given
    if (data) {
        int transferred = 0;
        int transferResult = libusb_bulk_transfer(device, USB_ENDPOINT_IN, data, size, &transferred, 3000);
        if (transferResult < 0)
            log_error(
                    "Error while read data from device. result: %s(code %d).",
                    libusb_error_name(transferResult),
                    transferResult
            );
        if (transferred != size) log_error("Write size doesn't equals to expected.");
        // if has error, fail fast
        if (transferResult < 0) return transferResult;
    }

    return warp_scsi_operation_result(device);
}

/*

    Private part

 */

void warp_scsi_set_command(const unsigned char *command, int commandLength) {
    global_scsi_buffer_cmd[14] = commandLength;
    memcpy(&global_scsi_buffer_cmd[15], command, commandLength);
}

void warp_scsi_set_data_size(unsigned long dataSize) {
    global_scsi_buffer_cmd[8] = dataSize;
    global_scsi_buffer_cmd[9] = dataSize >> 8u;
    global_scsi_buffer_cmd[10] = dataSize >> 16u;
    global_scsi_buffer_cmd[11] = dataSize >> 24u;
}

int warp_scsi_cmd_prepare(libusb_device_handle *device) {
    int transferred = 0;

    int transferResult = libusb_bulk_transfer(
            device,
            USB_ENDPOINT_OUT,
            (unsigned char *) global_scsi_buffer_cmd,
            sizeof(global_scsi_buffer_cmd), &transferred, 1000);

    if (transferResult < 0)
        log_error(
                "Error while write data to device: %s(code %d).",
                libusb_error_name(transferResult),
                transferResult
        );

    return transferResult;
}

int warp_scsi_operation_result(libusb_device_handle *device) {
    // get ACK:
    unsigned char stringBuffer[13];
    int stringLength = sizeof(stringBuffer);

    int retryCount = 0;
    int timeout;

    do {
        timeout = 0;
        int transferred = 0;
        int transferResult = libusb_bulk_transfer(
                device,
                USB_ENDPOINT_IN,
                (unsigned char *) stringBuffer,
                stringLength,
                &transferred,
                5000);

        if (transferred != stringLength) {
            timeout = 1;
            log_error("Read size doesn't equals to expected.");
        }
        retryCount++;

        if (transferResult < 0)
            log_error(
                    "Error while read data from device. result: %s(code %d).",
                    libusb_error_name(transferResult),
                    transferResult
            );

    } while (timeout && retryCount < 5);

    if (strncmp((char *) stringBuffer, "USBS", 4) != 0) {
        log_error("Got invalid reply.");
        return -1;
    }

    return stringBuffer[12];
}