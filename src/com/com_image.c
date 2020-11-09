//
// Created by cattenlinger on 20-3-7.
//

#include "com_image.h"

#include "../lib/stb_image.h"

int ll_image_load(ll_image *self, FILE *file, const int requiredChannel) {
    self->data = stbi_load_from_file(
            file,
            &(self->width), &(self->height), &(self->channels),
            requiredChannel
    );

    if (self->data == NULL) return 1;
    self->requiredChannel = requiredChannel;
    return 0;
}

void ll_image_free(ll_image *self) {
    if (self->data != NULL) stbi_image_free(self->data);
}