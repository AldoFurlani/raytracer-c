#include "spheres.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

void worldInit(World *world) {
    (*world).spheres = calloc(1, sizeof(Sphere *));
    if ((*world).spheres == NULL) {
        printf("Allocation error");
        exit(EXIT_FAILURE);
    }
    (*world).size = 0;
    (*world).capacity = 1;
}

void freeWorld(World *world) {
    for (int i = 0; i < (*world).size; i++) {
        free(*((*world).spheres + i));
    }
    free((*world).spheres);
    (*world).size = 0;
    (*world).capacity = 1;
}

void addSphere(World *world, Sphere *sphere) {
    (*world).size += 1;
    if ((*world).size > (*world).capacity) {
        (*world).capacity *= 2;
        (*world).spheres = realloc((*world).spheres, sizeof(Sphere *) * (*world).capacity);
        if ((*world).spheres == NULL) {
            printf("Reallocation failure");
            exit(EXIT_FAILURE);
        }
    }
    *((*world).spheres + ((*world).size - 1)) = sphere;
}

Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    Sphere *newSphere = malloc(sizeof(Sphere));
    (*newSphere).r = radius;
    (*newSphere).pos = position;
    (*newSphere).color = color;
    return newSphere;
}

int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t) {
    Vec3 spherePos = (*sphere).pos;
    float sphereRadius = (*sphere).r;
    
    Vec3 V = subtract(rayPos, spherePos);
    float a = dot(rayDir, rayDir);
    float b = 2 * dot(rayDir, V);
    float c = dot(V, V) - sphereRadius * sphereRadius;
    float discriminate = b*b - 4*a*c;

    if (discriminate < 0) return 0;

    float t1, t2;
    t1 = (b * (-1) + sqrtf(discriminate)) / (2 * a);
    t2 = (b * (-1) - sqrtf(discriminate)) / (2 * a);

    float temp;
    if (t1 < 0 && t2 < 0) {
        return 0;
    } else if (t2 < 0) {
        temp = t1;
    } else {
        temp = t2;
    }

    if (temp < *t) {
        *t = temp;
        return 1;
    }

    return 0;
}