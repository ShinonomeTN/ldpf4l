#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dpf_device.h"
#include "warp_scsi.h"

#include "../../utils/log.h"

#define USB_COMMAND_BLIT         0x12
#define USB_COMMAND_SET_PROPERTY 0x01

#define DPF_DEV_CMD_BUFFER_SIZE 16

#define DEVICE_INTERFACE_ID 0

const static uint8_t dpf_cmd_buffer_template[DPF_DEV_CMD_BUFFER_SIZE] = {
    0xcd, 0x00, 0x00, 0x00,
    0x00, 0x06, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

typedef struct dpf_device_t {
    warp_scsi_ctx_t *scsi_device;

    uint8_t* cmd_buf[DPF_DEV_CMD_BUFFER_SIZE];

    // Basic info
    uint32_t screen_width;
    uint32_t screen_height;

    uint32_t buffer_len;
    uint8_t *buffer;

    Rgba8 bg_color;

    // Properties
    uint32_t brightness;
} dpf_device_t;


int32_t dpf_device_open(libusb_device *device, dpf_device_ptr_t *new_device) {

    struct libusb_device_descriptor descriptor;
    libusb_get_device_descriptor(device, &descriptor);
    const int id_vendor = descriptor.idVendor;
    const int id_product = descriptor.idProduct;

    // Check the vendor id and product id (AX206 is vid=0x1908 pid=0x0102)
    if (id_vendor != 0x1908 || id_product != 0x0102) {
        log_fatal("Attempt to open an unsupported device [vendor: %x, product: %x]", id_vendor, id_product);
        return DPF_ERROR_DEVICE_NOT_SUPPORTED;
    }

    int32_t result = 0;

    libusb_device_handle *handle;

    result = libusb_open(device, &handle);
    if (result < 0) {
        log_fatal("Unable to open device: %s", libusb_error_name(result));
        return result;
    }

    result = libusb_claim_interface(handle, DEVICE_INTERFACE_ID);
    if (result < 0) {
        log_fatal("Could not claim device interface %d", DEVICE_INTERFACE_ID);
        return result;
    }

    warp_scsi_ctx_ptr ctx;
    if(warp_scsi_create_ctx(&ctx, handle) < 0) {
        log_fatal("Failed to create scsi context, error code %d", result);
        return -1;
    }

    const dpf_device_ptr_t dpf = malloc(sizeof(dpf_device_t));
    if(dpf == NULL) {
        log_fatal("Out of memory");
        return -1;
    }
    dpf->scsi_device = ctx;

    memcpy(dpf->cmd_buf, dpf_cmd_buffer_template, sizeof(dpf_cmd_buffer_template));

    log_debug("Dpf device created.");

    *new_device = dpf;

    dpf_device_set_background_color(dpf, 0, 0, 0);

    dpf->buffer = NULL;

    return result;
}

void dpf_device_set_background_color(const dpf_device_ptr_t device, const uint8_t r, const uint8_t g, const uint8_t b) {
    Rgba8 *backgroundColor = &device->bg_color;
    backgroundColor->red = r;
    backgroundColor->green = g;
    backgroundColor->blue = b;

    backgroundColor->alpha = 0xFFu;
}

Rgba8 *dpf_device_get_background_color(dpf_device_t *device) {
    return &device->bg_color;
}

static const uint8_t SCSI_CMD_READ[16] = {
    0xcd, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

int32_t dpf_device_acquire_dimensions(dpf_device_t *device) {
//#define BUFFER_GET_WIDTH(buffer) (buffer[0] | (buffer[1] << 8U))
//#define BUFFER_GET_HEIGHT(buffer) (buffer[2] | (buffer[3] << 8U))

    uint8_t buffer[5];
    const int32_t result = warp_scsi_read(device->scsi_device, SCSI_CMD_READ, sizeof(SCSI_CMD_READ), buffer, 5);
    if(result != 0) {
        log_warn("Get device dimensions failed.");
        return -1;
    }

    // Device use short type, do the trick
    const uint16_t* data = (uint16_t *) buffer;
    const uint32_t width = data[0];
    const uint32_t height = data[1];
    //       unsigned int width = BUFFER_GET_WIDTH(buffer);
    //       unsigned int height = BUFFER_GET_HEIGHT(buffer);

    device->screen_width = width;
    device->screen_height = height;

    const uint32_t buffer_len = width * height * DPF_BYTE_PRE_PIXEL;
    device->buffer_len = buffer_len;
    if (device->buffer != NULL) free(device->buffer);
    device->buffer = (uint8_t *) malloc(buffer_len);
    if (!device->buffer) {
        log_error("Could not allocate screen buffer.");
        return -1;
    }

    log_debug("Screen dimension read success, screen properties also initialized.");

    return 0;
}

#define DPF_SET_USB_COMMAND(cmd_buf, cmd) cmd_buf[6] = cmd
#define DPF_PROPERTY_BRIGHTNESS 0x0001

int32_t dpf_device_set_brightness(dpf_device_t *device, uint32_t brightness) {

    int brightness_hint = 0;
    if (brightness <= 0) {
        brightness_hint = 1;
        brightness = 0;
    }
    if (brightness > 7) {
        brightness_hint = 1;
        brightness = 7;
    }

    if (brightness_hint) log_warn("%d is invalid value, brightness for dpf should between 0 and 7.", brightness);


    uint8_t *cmd_buf = device->cmd_buf;
    DPF_SET_USB_COMMAND(cmd_buf, USB_COMMAND_SET_PROPERTY);

    // Do the trick
    uint16_t *data = (uint16_t *) &cmd_buf[7];
    data[0] = DPF_PROPERTY_BRIGHTNESS;
    data[1] = brightness;

    const int result = warp_scsi_write(device->scsi_device, cmd_buf, sizeof(DPF_DEV_CMD_BUFFER_SIZE), NULL, 0);
    if (result < 0) {
        log_error("Failed to send brightness setting command to device.\n");
        return result;
    }
    device->brightness = brightness;
    return result;
}

uint32_t dpf_device_get_buffer_size(const dpf_device_t *device) {
    return device->buffer_len;
}

uint8_t *dpf_device_get_buffer(const dpf_device_t *device) {
    return device->buffer;
}

int32_t dpf_device_get_brightness(const dpf_device_t *device) {
    return device->brightness;
}

uint32_t dpf_device_screen_width(const dpf_device_t *device) {
    return device->screen_width;
}

uint32_t dpf_device_screen_height(const dpf_device_t *device) {
    return device->screen_height;
}

int32_t dpf_device_flush(dpf_device_t *device, const RectTuple *rectTuple) {
    return dpf_device_bulk_transfer(device, device->buffer, rectTuple);
}

int32_t dpf_device_bulk_transfer(dpf_device_t *device, const uint8_t *buffer, const RectTuple *rectTuple) {
    #define TRANSFER_GET_DATA_SIZE(pixels) (pixels << 1U) // x2 to short

    const uint32_t pixels = rect_tuple_width(rectTuple) * rect_tuple_height(rectTuple);
    const uint32_t data_len = TRANSFER_GET_DATA_SIZE(pixels);

    uint8_t *command = device->cmd_buf;
    DPF_SET_USB_COMMAND(command, USB_COMMAND_BLIT);

    uint16_t *params = (uint16_t *) &command[7];
    params[0] = rectTuple->x0;
    params[1] = rectTuple->y0;
    params[2] = rectTuple->x1;
    params[3] = rectTuple->y1;

    command[15] = 0;

    log_trace("Sending %dpx (%d Bytes) to device.", pixels, data_len);

    const int result = warp_scsi_write(device->scsi_device, command, DPF_DEV_CMD_BUFFER_SIZE, (uint8_t *) buffer,
                                       data_len);
    return result < 0 ? result : 0;
}

void dpf_destroy(dpf_device_t *device) {
    const warp_scsi_ctx_ptr scsi_ctx = device->scsi_device;
    libusb_device_handle *usb_dev = warp_scsi_get_device(scsi_ctx);

    libusb_release_interface(usb_dev, DEVICE_INTERFACE_ID);
    log_trace("Release device interface %d.", DEVICE_INTERFACE_ID);
    libusb_close(usb_dev);
    log_trace("Close usb device.");

    warp_scsi_destroy_ctx(scsi_ctx);

    if (device->buffer != NULL) free(device->buffer);
    free(device);
    log_debug("Dpf Device destroyed.");
}
