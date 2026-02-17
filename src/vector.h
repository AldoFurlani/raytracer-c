#ifndef VECTOR_H
#define VECTOR_H
#include <math.h>
#include <stdio.h>

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

Vec3 add(Vec3 v1, Vec3 v2);
Vec3 subtract(Vec3 v1, Vec3 v2);
Vec3 scalarMultiply(float s, Vec3 v);
Vec3 scalarDivide(Vec3 v, float d);
Vec3 normalize(Vec3 v);
float dot(Vec3 v1, Vec3 v2);
float length2(Vec3 v);
float length(Vec3 v);
float distance2(Vec3 v1, Vec3 v2);
float distance(Vec3 v1, Vec3 v2);
void printVec(FILE * restrict stream, Vec3 v);
void printOp(FILE * restrict stream, Vec3 v1, Vec3 v2, Vec3 res, char op);

#endif
