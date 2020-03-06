//
// Created by cattenlinger on 20-3-4.
//

#ifndef LDPF4L_COM_COMPONENT_H
#define LDPF4L_COM_COMPONENT_H

#include "../utils/common_types.h"

typedef struct ll_component {
    const char *name;
    int type;

    int (*init)(struct ll_component *self);

    int (*draw)(struct ll_component *self, const RectTuple *dimension, unsigned int *canvas);

    int (*destory)(struct ll_component *self);

} ll_component;

static int component_register(ll_component *component);
static int component_unregister(ll_component *component);

#endif //LDPF4L_COM_COMPONENT_H
