//
// Created by cattenlinger on 7/2/19.
//
#include "dpf_usb_utils.h"

#include <stdio.h>
#include <stdlib.h>


libusb_device *dpf_get_device(libusb_context *usbContext) {
    const uint16_t dpfVendor = 0x1908;
    const uint16_t dpfProduct = 0x0102;

    libusb_device **devices;
    ssize_t deviceCount = libusb_get_device_list(usbContext, &devices);
    if (deviceCount < 0) {
        fprintf(stderr, "dpf_get_device: Could not get dpf device.\n");
        return NULL;
    }

    for (ssize_t i = 0; i < deviceCount; i++) {
        libusb_device *currentDevice = devices[i];

        struct libusb_device_descriptor descriptor;

        if (libusb_get_device_descriptor(currentDevice, &descriptor) < 0) {
            fprintf(stderr, "dpf_get_device: Fail to get descriptor for one device. Skipping\n");
            continue;
        }

        uint16_t idVendor = descriptor.idVendor;
        uint16_t idProduct = descriptor.idProduct;

        if (idVendor == dpfVendor && idProduct == dpfProduct) {
            libusb_free_device_list(devices, 1);
            fprintf(stderr, "dpf_get_device: DPF device found.\n");
            return currentDevice;
        }
    }

    libusb_free_device_list(devices, 1);
    fprintf(stderr, "dpf_get_device: DPF device not found.\n");
    return NULL;
}

void dpf_usb_print_device(libusb_device *device) {
    struct libusb_config_descriptor *configDescriptor;
    libusb_get_config_descriptor(device, 0, &configDescriptor);

    printf("Intefaces: %d\n", (int) configDescriptor->bNumInterfaces);
    for (int i = 0; i < (int) configDescriptor->bNumInterfaces; i++) {
        const struct libusb_interface *interface = &configDescriptor->interface[i];
        printf("Number of alternative settings: %3d\n", interface->num_altsetting);
        for (int j = 0; j < (int) interface->num_altsetting; j++) {
            const struct libusb_interface_descriptor *interfaceDescriptor = &interface->altsetting[j];
            printf("\tInterface Number: %3d\n", interfaceDescriptor->bInterfaceNumber);
            printf("\tNumber of Endpoints: %3d\n", interfaceDescriptor->bNumEndpoints);
            for (int k = 0; k < (int) interfaceDescriptor->bNumEndpoints; k++) {
                const struct libusb_endpoint_descriptor *endpointDescriptor = &interfaceDescriptor->endpoint[k];
                printf("\t\tDescriptor Type: %3d\n", endpointDescriptor->bDescriptorType);
                printf("\t\tEndpoint Address: %3d\n", endpointDescriptor->bEndpointAddress);
            }
        }
    }
    libusb_free_config_descriptor(configDescriptor);
}