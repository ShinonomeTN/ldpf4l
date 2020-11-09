//
// Created by cattenlinger on 20-3-7.
//

#ifndef LDPF4L_COM_IMAGE_H
#define LDPF4L_COM_IMAGE_H

#include <bits/types/FILE.h>
#include "../utils/common_types.h"

typedef struct ll_image {
    int width;
    int height;
    byte *data;
    int channels;
    int requiredChannel;
} ll_image;

int ll_image_load(ll_image *self, FILE *file, int requiredChannel);

void ll_image_free(ll_image *self);

#endif //LDPF4L_COM_IMAGE_H
