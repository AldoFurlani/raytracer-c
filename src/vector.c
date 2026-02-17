#include "vector.h"

Vec3 add(Vec3 v1, Vec3 v2) {
    Vec3 newVec;
    newVec.x = v1.x + v2.x;
    newVec.y = v1.y + v2.y;
    newVec.z = v1.z + v2.z;
    return newVec;
}

Vec3 subtract(Vec3 v1, Vec3 v2) {
    Vec3 newVec;
    newVec.x = v1.x - v2.x;
    newVec.y = v1.y - v2.y;
    newVec.z = v1.z - v2.z;
    return newVec;
}

Vec3 scalarMultiply(float s, Vec3 v) {
    Vec3 newVec;
    newVec.x = s * v.x;
    newVec.y = s * v.y;
    newVec.z = s * v.z;
    return newVec;
}

Vec3 scalarDivide(Vec3 v, float d) {
    Vec3 newVec;
    newVec.x = v.x / d;
    newVec.y = v.y / d;
    newVec.z = v.z / d;
    return newVec;
}

Vec3 normalize(Vec3 v) {
    return scalarDivide(v, length(v));
}

float dot(Vec3 v1, Vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float length2(Vec3 v) {
    return dot(v, v);
}

float length(Vec3 v) {
    return sqrtf(dot(v, v));
}

float distance2(Vec3 v1, Vec3 v2) {
    Vec3 newVec = subtract(v1, v2);
    return length2(newVec);
}

float distance(Vec3 v1, Vec3 v2) {
    Vec3 newVec = subtract(v1, v2);
    return length(newVec);
}

void printVec(FILE * restrict stream, Vec3 v) {
    fprintf(stream, "(%.1f, %.1f, %.1f)", v.x, v.y, v.z);
}

void printOp(FILE * restrict stream, Vec3 v1, Vec3 v2, Vec3 res, char op) {
    printVec(stream, v1);
    fprintf(stream, " %c ", op);
    printVec(stream, v2);
    fprintf(stream, " = ");
    printVec(stream, res);
    fprintf(stream, "\n");
}