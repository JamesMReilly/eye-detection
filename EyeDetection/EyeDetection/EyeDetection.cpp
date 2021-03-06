// EyeDetection.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Image.h"
#include "ValidateArguments.h"
#include "FileIO.h"
#include "Filter.h"
#include "DetectEdges.h"
#include "CircleDetection.h"

#define HOOD_N 3
#define HOOD_M 3

Image runEdgeDetection(Image img);


int main(int argc, char *argv[]) {
    FILE *fp;
    char *inputFile = argv[1];
    int width, height, minRad = 0, range = 60;

    // Jim
    validateInputArguments(argc, argv, &width, &height);
    validateFileExists(inputFile);

    float *pixels = (float *)calloc(sizeof(float), width * height);
    int size = width * height;
    readInputFileIntoArray(inputFile, pixels, size);

    // Colleen

    //construct the original image from the input data
    Image im(height, width, pixels);

    //run edge detection -- edge dectection team
    Image im_edge = runEdgeDetection(im);

    //find circles -- hough team
    CircleDetection circleDetect;
    Circle circle = circleDetect.detectCircle(im_edge, height, width, minRad, range);

    //write circle contents to output file
    char* outputFile = argv[4];
    FILE *outFile = fopen(outputFile, "w");

    fprintf(outFile, "%u, %u, %u", (int)circle.getOriginX(), (int)circle.getOriginY(), (int)circle.getRadius());
    fclose(outFile);
}


// Lauren
Image runEdgeDetection(Image im) {
    float imageThreshold = 0;

    //kernel x
    Image kernel(HOOD_M, HOOD_N);
    kernel.setVal(0, 0, -1);
    kernel.setVal(0, 1, 0);
    kernel.setVal(0, 2, 1);
    kernel.setVal(1, 0, -2);
    kernel.setVal(1, 1, 0);
    kernel.setVal(1, 2, 2);
    kernel.setVal(2, 0, -1);
    kernel.setVal(2, 1, 0);
    kernel.setVal(2, 2, 1);

    EdgeDetection gX(kernel);
    Image imX = gX.process(im, HOOD_M, HOOD_N);

    //kernel y
    kernel.setVal(0, 0, 1);
    kernel.setVal(0, 1, 2);
    kernel.setVal(0, 2, 1);
    kernel.setVal(1, 0, 0);
    kernel.setVal(1, 1, 0);
    kernel.setVal(1, 2, 0);
    kernel.setVal(2, 0, -1);
    kernel.setVal(2, 1, -2);
    kernel.setVal(2, 2, -1);

    EdgeDetection gY(kernel);
    Image imY = gY.process(imX, HOOD_M, HOOD_N);

    //sum X and Y kernel outputs
    int rows = imX.getNumRows();
    int cols = imX.getNumCols();
    Image ImgXY = Image(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double sum = sqrt(pow(imX.getVal(i, j), 2) + pow(imY.getVal(i, j), 2));
            ImgXY.setVal(i, j, sum);
        }
    }

    //determine threshhold
    imageThreshold = ImgXY.getAverageValue();
    imageThreshold /= 0.35;

    //create binary edge map
    ImgXY.createEdgeMap(imageThreshold);

    return ImgXY;
}

