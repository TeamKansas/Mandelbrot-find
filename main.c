/*
    This program finds an 'interesting' point on the Mandelbrot set and then creates an image of that spot.
    'Interesting' in this context is a spot that has a wide variety of iterations required to leave the Radius 
    2 circle. In order to find spots with high variety, the program samples several points within the current 
    viewpoint, called 'frame' in this program. Note that variables called frame denote the frame radius, and the 
    center point is typically (x, y), (a, b) or (focus[0], focus[1]). After sampling several points (about 1600 
    in this program), it calculates the minimum and the average. The difference between the average and minimum 
    denotes how 'interesting' that frame is. The program will do this for several frames within the current frame
    and select the most interesting frame of those evaluated. This works because most of the time there are going 
    to be more points close to the minimum than close to the maximum, unless it is a spot in the set (painted black).
    Now this works, but produces similar results every time because only the spots with the absolute most variety
    it could find will be selected. To fix this, the program restricts each output with a random modulo between 10 
    and 30, and this lets frames with a smaller variety of iterations to be selected occasionally. After this is 
    done it changes the frame radius by 0.5 and repeats multiple times. The last frame selected is the frame 
    displayed on the output .tiff file
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "tiffDrawingTools.h"


// half dimensions of the generated image (Radius Width, Radius Height)
// actual dimensions are 4 * R_WIDTH * R_HEIGHT
// used to reduce math operations during image building
#define R_WIDTH 1000
#define R_HEIGHT 1000

// calculate number of iterations point a+bi takes to leave circle. maximim iterations denoted with (int) max
int calcPoint(double a, double b, int max) {
    double x, y, tx, ty;
    int i;
    x = a;
    y = b;
    for(i = 0; i < max && x*x + y*y <= 4; i++) {
        tx = x;
        ty = y;
        // (x + yi) -> (x + yi)^2 + (a + bi) 
        // Also noted as Z -> Z^2 + C
        x = tx*tx - ty*ty + a;
        y = 2*tx*ty + b;
    }
    return i;
}

// calculates how interesting frame (a + bi) is.
// interestingness is how diverse the iteration values are within the specified frame.
int interest(double a, double b, double frame, int maxIter) {
    double inc = frame / 20.0;
    double min = maxIter;
    double max = 0;
    int i;
    double avg = 0;
    int dataPoints = 0;
    for(double x = a - frame; x < a + frame; x += inc) {
        for(double y = b - frame; y < b + frame; y += inc) {
            i = calcPoint(x,y,maxIter);
            if(i > max)
                max = i;
            if(i < min)
                min = i;
            avg += i;
            dataPoints++;
        }
    }
    avg /= (double)dataPoints;
    return (int)(avg - min);
}

// finds a random interesting spot on the mandelbrot set.
void findInterest(double out[2], double *zoom, int maxIter) {
    int i;
    int max = 0;
    // add a random factor to avoid getting the same spot more than once
    out[0] += (double)(rand()%2000 - 1000) / 2000.0 * (*zoom);
    out[1] += (double)(rand()%2000 - 1000) / 2000.0 * (*zoom);
    double frame[2] = {out[0],out[1]};
    for(double x = -1; x < 1; x += 0.25) {
        for(double y = -1; y < 1; y += 0.25) {
            // the modulo is to add a random factor to avoid getting the same spot more than once.
            // the modulo allows the program to find a wider variety of shapes. Without it, all of the patterns it finds are very similar, and it gets boring
            i = interest(x*(*zoom) + frame[0],y*(*zoom) + frame[1],(*zoom) / 8.0,maxIter)%(10 + rand()%20);
            if(i > max || (i ==  max && rand()%2)) {
                max = i;
                out[0] = x*(*zoom) + frame[0];
                out[1] = y*(*zoom) + frame[1];
            }
        }
    }
    *zoom *= 0.5;
}

int main(int argc, char **argv) {

    screen *sp = sopen(R_WIDTH*2,R_HEIGHT*2);
    double i, pri;
    uint8_t r, g, b;
    int max = 0;
    double frame = 1.25;
    double focus[2] = {-0.75,0};
    int seed = clock();
    int depth = -1;
    int calcMax = 2000;
    int maxIter = 3000;
    int shading = 0;
    double gradient = 0.01;
    double colorShift = 0;
    char *name = "Mandelbrot.tiff";

    // command line args
    for(int i = 1; i < argc; i++) {
        if(i+1 < argc) {
            if(!strcmp("-i",argv[i]))       // set maximum iterations for the image. Default is 3000
                maxIter = atoi(argv[++i]);
            else if(!strcmp("-S",argv[i]))  // set randomizer seed. Default is the value of clock();
                seed = atoi(argv[++i]);
            else if(!strcmp("-g",argv[i]))  // change the frequency of the color gradient. Default is 0.01
                gradient = atof(argv[++i]);
            else if(!strcmp("-d",argv[i]))  // set the number of zoom iterations, Default is random.
                depth = atoi(argv[++i]);
            else if(!strcmp("-m",argv[i]))  // set the maximum iterations for the interest calculator. Default is 2000
                calcMax = atoi(argv[++i]);
            else if(!strcmp("-c",argv[i]))  // set the color shift for the color gradient. Default is 0.
                colorShift = atof(argv[++i]);
            else if(!strcmp("-x",argv[i]))  // set starting x value (a value in a+bi). Default is -0.75
                focus[0] = atof(argv[++i]);
            else if(!strcmp("-y",argv[i]))  // set starting y value (b value in a+bi). default is 0
                focus[1] = atof(argv[++i]);
            else if(!strcmp("-f",argv[i]))  // set the starting frame radius of the image. default is 1.25
                frame = atof(argv[++i]);
            else if(!strcmp("-n",argv[i]))  // set output file name. default is Mandelbrot.tiff
                name = argv[++i];
        }
        if(!strcmp("-s",argv[i])) // allow shading
            shading = 1;
    }

    // set randomizer
    srand(seed);

    // set depth if not set by user
    if(depth < 0)
        depth = 10 + rand()%25;
    else 
        rand();

    // start zoom and interest finding process.
    for(int i = 0; i < depth; i++) {
        printf("\033cZoom %d of %d\n",i + 1 , depth);
        findInterest(focus,&frame,calcMax);
    }
    // generate the pixel values of the image.
    for(int x = -R_WIDTH; x < R_WIDTH; x++) {
        pri = maxIter;
        for(int y = -R_HEIGHT; y < R_HEIGHT; y++) {
            i = calcPoint((double)x / (double)R_WIDTH * frame + focus[0],(double)y / (double)R_HEIGHT * frame + focus[1],maxIter);
            if(i != maxIter) {
                i = i*gradient + colorShift;
                r = cos(i+2.0)*50 + 150;
                g = cos(i+1)*50 + 150;
                b = cos(i)*63 + 192;
                if(pri > i && shading == 1) {
                    r *= 0.6;
                    g *= 0.6;
                    b *= 0.6;
                }
                setPixelWithColor(sp,x+R_WIDTH,y+R_HEIGHT,r,g,b);
            }
            pri = i;
        }
        printf("\033cBuilding Image: %.2lf%%\n",(x + R_WIDTH + 1) / (double)R_WIDTH * 50.0);
    }
    // write pixel buffer to file.
    writeFile(name,sp);
    // print generation parameters used. 
    printf("Randomizer seed used [-S]: %d\nZoom depth: %lg (%d zoom iterations [-d] )\n(%.10lg, %.10lg) ([-x], [-y])\nCalculation iteration value [-m]: %d\nImage iteration value [-i]: %d\n",seed, frame, depth,focus[0],focus[1],calcMax,maxIter);

    sclose(sp);

}