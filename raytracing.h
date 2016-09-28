#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

#define THREAD_NUM 8

typedef struct myStruct {

    uint8_t *pixels;
    color background_color;
    rectangular_node rectangulars;
    sphere_node spheres;
    light_node lights;
    const viewpoint *view;
    int width, height, i, j;

} input;

void *raytracing(void* box);

#endif
