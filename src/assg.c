#include "vector.h"
#include "spheres.h"
#include "color.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define shadowFactor 0.1

Vec3 backGroundColor = {0.0,0.0,0.0}, cameraPos = {0.0,0.0,0.0};
World world;
struct {
    float width;
    float height;
    float focalLen;
} viewPort;
struct {
    Vec3 pos;
    float brightness;
} lightSource;

#ifdef MS1
void ms1Out(FILE * restrict fp2, int numSpheres, Vec3 sphereColor, Vec3 backGroundColor){ 
    Sphere currentSphere;
    printOp(fp2, backGroundColor, lightSource.pos, add(backGroundColor, lightSource.pos), '+');
    printOp(fp2, backGroundColor, lightSource.pos, subtract(backGroundColor, lightSource.pos), '-');
    fprintf(fp2, "%.1f * ", viewPort.width);
    printVec(fp2, lightSource.pos);
    fprintf(fp2, " = ");
    printVec(fp2, scalarMultiply(viewPort.width, lightSource.pos));
    fprintf(fp2, "\n");
    fprintf(fp2, "normalize");
    printVec(fp2, lightSource.pos);
    fprintf(fp2, " = ");
    printVec(fp2, normalize(lightSource.pos));
    fprintf(fp2, "\n");

    for (int i = 0; i < numSpheres; i++) {
        fprintf(fp2, "\n");
        currentSphere = *(world.spheres)[i];
        printVec(fp2, sphereColor);
        fprintf(fp2, " / %.1f = ", currentSphere.r);
        printVec(fp2, scalarDivide(sphereColor, currentSphere.r));
        fprintf(fp2, "\n");
        fprintf(fp2, "dot(");
        printVec(fp2, lightSource.pos);
        fprintf(fp2, ", ");
        printVec(fp2, currentSphere.pos);
        fprintf(fp2, ") = %.1f\n", dot(lightSource.pos, currentSphere.pos));
        fprintf(fp2, "distance(");
        printVec(fp2, lightSource.pos);
        fprintf(fp2, ", ");
        printVec(fp2, currentSphere.pos);
        fprintf(fp2, ") = %.1f\n", distance(lightSource.pos, currentSphere.pos));
        fprintf(fp2, "length");
        printVec(fp2, currentSphere.pos);
        fprintf(fp2, " = %.1f\n", length(currentSphere.pos));
    }
}
#endif


Vec3 pixelColor(Vec3 rayPos, Vec3 rayDir, Sphere lightSourceSphere) {
    Sphere *curSphere, *intersectionSphere;
    Vec3 intersectionPoint, normal, normalizedNormal, d, pixelColor, lightDir;
    float intensity, numerator, t = INFINITY;
    for (int i = 0; i < world.size; i++) {
        curSphere = *(world.spheres + i);
        if (doesIntersect(curSphere, cameraPos, rayDir, &t) == 1) intersectionSphere = curSphere;
    }
    if (t == INFINITY) return backGroundColor;
    intersectionPoint = add(rayPos, scalarMultiply(t, rayDir));
    normal = subtract(intersectionPoint, (*intersectionSphere).pos);
    normalizedNormal = normalize(normal);
    lightDir = subtract(lightSource.pos, intersectionPoint);
    
    d = normalize(lightDir);
    numerator = dot(d, normalizedNormal);
    numerator = (numerator > 0) ? numerator : 0;
    intensity = lightSource.brightness * (numerator/(distance2(lightSource.pos, intersectionPoint)));
    intensity = (intensity < 1) ? intensity : 1;

    pixelColor = scalarMultiply(intensity, (*intersectionSphere).color);

    t = INFINITY;
    d = scalarMultiply(0.001, d);
    intersectionPoint = add(intersectionPoint, d);

    doesIntersect(&lightSourceSphere, intersectionPoint, lightDir, &t);
    for (int i = 0; i < world.size; i++) {
        curSphere = *(world.spheres + i);
        if (doesIntersect(curSphere, intersectionPoint, lightDir, &t) == 1) 
        return scalarMultiply(shadowFactor, pixelColor);
    }

    return pixelColor;
}

#ifdef MS2
void MS2Pipeline(FILE *fp2, int imgWidth, int imgHeight) {
    Vec3 dirVector;
    float unit = viewPort.width/imgWidth;
    dirVector.z = (-1) * viewPort.focalLen;
    fprintf(fp2, "P3\n");
    fprintf(fp2, "%d %d\n", imgWidth, imgHeight);
    fprintf(fp2, "%d\n", 255);
    Sphere lightSourceSphere;
    lightSourceSphere.pos = lightSource.pos;
    lightSourceSphere.r = 0.01;
    lightSourceSphere.color = (Vec3) {1.0, 1.0, 1.0};
    for (float i = viewPort.height/2 - unit/2; i > -viewPort.height/2; i -= unit) {
        for (float j = -viewPort.width/2 + unit/2; j < viewPort.width/2; j += unit) {
            dirVector.x = j;
            dirVector.y = i;
            writeColour(fp2, pixelColor(cameraPos, dirVector, lightSourceSphere));
        }
    }
}
#endif

#ifdef FS
void FSPipeline(FILE *fp2, int imgWidth, int imgHeight) {
    Vec3 dirVector;
    float unit = viewPort.width/imgWidth;
    dirVector.z = (-1) * viewPort.focalLen;
    fprintf(fp2, "P3\n");
    fprintf(fp2, "%d %d\n", imgWidth, imgHeight);
    fprintf(fp2, "%d\n", 255);
    Vec3 total;
    Sphere lightSourceSphere;
    lightSourceSphere.pos = lightSource.pos;
    lightSourceSphere.r = 0.01;
    lightSourceSphere.color = (Vec3) {1.0, 1.0, 1.0};
    for (float i = viewPort.height/2 - unit/2; i > -viewPort.height/2; i -= unit) {
        for (float j = -viewPort.width/2 + unit/2; j < viewPort.width/2; j += unit) {
            total = (Vec3) {0.0,0.0,0.0};
            for (float k = unit/3; k >= -unit/3; k -= unit/3) {
                for (float l = -unit/3; l <= unit/3; l += unit/3) {
                    dirVector.x = j + l;
                    dirVector.y = i + k;
                    total = add(total, pixelColor(cameraPos, dirVector, lightSourceSphere));
                }
            }
            writeColour(fp2, scalarDivide(total,9.0));
        }
    }
}
#endif

int main(int argc, char *argv[]) {
    int imgWidth, imgHeight, numColors, backgroundIdx, numSpheres, sphereIdx;
    float sx, sy, sz, r, aspctRatio;
    FILE *fp1, *fp2;
    Vec3 spherePos, sphereColor = {1.0, 1.0, 1.0};
    worldInit(&world);
    fp1 = fopen(argv[1], "r");
    if (fp1 == NULL) {
        printf("Unable to open file 1\n");
        exit(EXIT_FAILURE);
    }
    fp2 = fopen(argv[2], "w");
    if (fp2 == NULL) {
        printf("Unable to open file 2\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fp1, "%d %d", &imgWidth, &imgHeight);
    aspctRatio = (float) imgWidth / imgHeight;
    fscanf(fp1, "%f %f", &viewPort.height, &viewPort.focalLen);
    viewPort.width = viewPort.height * aspctRatio;
    fscanf(fp1, "%f %f %f %f", &lightSource.pos.x, &lightSource.pos.y, &lightSource.pos.z, &lightSource.brightness);
    fscanf(fp1, "%d", &numColors);
    int colorArr[numColors];
    for (int i = 0; i < numColors; i++) {
        fscanf(fp1, "%x", colorArr + i);
    }
    fscanf(fp1, "%d", &backgroundIdx);
    #ifdef FS
    qsort(colorArr, numColors, sizeof(int), compareColor);
    backGroundColor = unpackRGB(colorArr[backgroundIdx]);
    #endif
    fscanf(fp1, "%d", &numSpheres);
    for (int i = 0; i < numSpheres; i++) {
        fscanf(fp1, "%f %f %f %f %d", &sx, &sy, &sz, &r, &sphereIdx);
        spherePos.x = sx;
        spherePos.y = sy;
        spherePos.z = sz;
        #ifdef FS
        sphereColor = unpackRGB(colorArr[sphereIdx]);
        #endif
        addSphere(&world, createSphere(r, spherePos, sphereColor));  
    }
    fclose(fp1);

    #ifdef MS1
    ms1Out(fp2, numSpheres, sphereColor, backGroundColor);
    #endif

    #ifdef MS2
    MS2Pipeline(fp2, imgWidth, imgHeight);
    #endif

    #ifdef FS
    FSPipeline(fp2, imgWidth, imgHeight);
    #endif

    fclose(fp2);
    freeWorld(&world);
    return 0;
}