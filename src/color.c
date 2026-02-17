#include "color.h"

void writeColour(FILE *ppmFile, Vec3 color) {
    fprintf(ppmFile, "%d %d %d ", (int) (color.x * 255), (int) (color.y * 255), (int) (color.z * 255));
}

Vec3 unpackRGB(unsigned int packedRGB) {
    Vec3 RGBval;
    int RGBcomp, bitMask = 0xFF;

    RGBcomp = packedRGB & bitMask;
    RGBval.z = RGBcomp;
    packedRGB >>= 8;

    RGBcomp = packedRGB & bitMask;
    RGBval.y = RGBcomp;
    packedRGB >>= 8;

    RGBcomp = packedRGB & bitMask;
    RGBval.x = RGBcomp;

    return scalarDivide(RGBval, 255);
}

int compareColor(const void *a, const void *b)
{
    int a1 = 0, b1 = 0;
    for (int i = 0; i < sizeof(int); i++)
    {
        a1 |= (*((unsigned char*)a + i) & 0x0F) << (i * 8);
        b1 |= (*((unsigned char*)b + i) & 0x0F) << (i * 8);
    }
    
    return (a1 < b1) ? -1 : (b1 < a1) ? 1 : (*((int*)a) < *((int*)b)) ? -1 : (*((int*)a) > *((int*)b)) ? 1 : 0;
}