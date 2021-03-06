//Faisal Alnahhas      //
//OS - Project 3 part 2//
//Threads              //
//UTA - Fall 2017      //
/////////////////////////

#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );

//since pthread_create takes one argument for the function passed
//we change compute_image to accept a single argument, a structure
void * compute_image( void *arguments );

void show_help()
{
    printf("Use: mandel [options]\n");
    printf("Where options are:\n");
    printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
    printf("-x <coord>  X coordinate of image center point. (default=0)\n");
    printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
    printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
    printf("-W <pixels> Width of the image in pixels. (default=500)\n");
    printf("-H <pixels> Height of the image in pixels. (default=500)\n");
    printf("-o <file>   Set output file. (default=mandel.bmp)\n");
    printf("-h          Show this help text.\n");
    printf("\nSome examples are:\n");
    printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
    printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
    printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

struct args_struct {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int max;
    int start2;
    int end2;
    int height;
    int width;
    const char *outfile;
}args;

int main( int argc, char *argv[] )
{
    char c;
    int i;
    
    //compute duration just like in mandelseries
    struct timeval begin, end;
    
    // These are the default configuration values used
    // if no command line arguments are given.
    
    const char *outfile = "mandel.bmp";
    double xcenter = 0;
    double ycenter = 0;
    double scale = 4;
    int    image_width = 500;
    int    image_height = 500;
    int    max = 1000;
    int    nvalue = 1;
    int    start  = 0;
    // For each command line argument given,
    // override the appropriate configuration value.
    
    while((c = getopt(argc,argv,"x:y:s:W:H:m:o:n:h"))!=-1) {
        switch(c) {
            case 'x':
                xcenter = atof(optarg);
                break;
            case 'y':
                ycenter = atof(optarg);
                break;
            case 's':
                scale = atof(optarg);
                break;
            case 'W':
                image_width = atoi(optarg);
                break;
            case 'H':
                image_height = atoi(optarg);
                break;
            case 'm':
                max = atoi(optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'n':
                //get the value from the command line argument, -n
                nvalue = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(1);
                break;
        }
    }
    //create an array of thread IDs to use in pthread_join
    pthread_t tids[nvalue];
    int    new_height = image_height/nvalue;
    int    new_end = new_height;
    
    // Display the configuration of the image.
    printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);
    
    //here we access and update the values of the structure
    args.xmin = xcenter - scale;
    args.xmax = xcenter + scale;
    args.ymin = ycenter - scale;
    args.ymax = ycenter + scale;
    args.max = max;
    args.start2 = start;
    args.end2 = image_height;
    
    //use the values from the structure to create the bitmap
    args.width = image_width;
    args.height = image_height;
    args.outfile = outfile;
    
    
    // Create a bitmap of the appropriate size.
    //struct bitmap *bm = bitmap_create(image_width,image_height);
    
    // Fill it with a dark blue, for debugging
    
    // Create a bitmap of the appropriate size.
    //struct bitmap *bm = bitmap_create(image_width,image_height);
    
    // Fill it with a dark blue, for debugging
    //bitmap_reset(bm,MAKE_RGBA(0,0,255,0));
    
    // Compute the Mandelbrot image
    //compute_image(&args);

    gettimeofday(&begin, NULL);
    
    //create n threads using thread IDs
    for (i=0; i< nvalue; i++)
    {
        printf("start= %d\n", start);
        printf("end= %d\n", new_end);
        //passing in thread IDs, no attributes, the function
        //we wish to spawn threads from, and a pointer to its
        //parameter, i.e. the structure defined above
        pthread_create(&tids[i], NULL, &compute_image, (void *)&args);
        start += new_height;
        new_end += new_height;
        printf("thread created\n");
        
    }
    
    for(i=0; i<nvalue; i++)
    {
        pthread_join(tids[i], NULL);
    }
    
    // Save the image in the stated file.
   /* if(!bitmap_save(bm,outfile)) {
        fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
        return 1;
    }*/
    
    gettimeofday(&end, NULL);
    int time_duration = ((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec));
    printf("Duration is %d\n", time_duration);
    pthread_exit(NULL);
    return 0;
}

/*
 Compute an entire Mandelbrot image, writing each point to the given bitmap.
 Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
 */


void * compute_image( void *arguments )
{
    struct args_struct *args = (struct args_struct *) arguments;
    int i,j;
    const char* outputfile = args->outfile;
    
    struct bitmap *bm = bitmap_create(args->width, args->end2);
    bitmap_reset(bm,MAKE_RGBA(0,0,255,0));
    
    int width = args->width;
    //int height = args->end2;
    
    // For every pixel in the image...
    
    for(j=args->start2;j<args->end2;j++) {
        
        for(i=0;i<width;i++) {
            
            // Determine the point in x,y space for that pixel.
            //access values of args to define the x,y space
            //for each pixel
            double x = args->xmin + i*(args->xmax-args->xmin)/width;
            double y = args->ymin + j*(args->ymax-args->ymin)/args->end2;
            
            // Compute the iterations at that point.
            int iters = iterations_at_point(x,y,args->max);
            
            // Set the pixel in the bitmap.
            bitmap_set(bm,i,j,iters);
            //printf("end2 is: %d\n", args->end2);
            
        }
    
    }
    
    
    if(!bitmap_save(bm,outputfile)) {
        fprintf(stderr,"mandel: couldn't write to %s: %s\n",outputfile,strerror(errno));
        //return 1;
    }
    return 0;
}

/*
 Return the number of iterations at point x, y
 in the Mandelbrot space, up to a maximum of max.
 */


int iterations_at_point( double x, double y, int max )
{
    double x0 = x;
    double y0 = y;
    
    int iter = 0;
    
    while( (x*x + y*y <= 4) && iter < max ) {
        
        double xt = x*x - y*y + x0;
        double yt = 2*x*y + y0;
        
        x = xt;
        y = yt;
        
        iter++;
    }
    
    return iteration_to_color(iter,max);
}

/*
 Convert a iteration number to an RGBA color.
 Here, we just scale to gray with a maximum of imax.
 Modify this function to make more interesting colors.
 */

int iteration_to_color( int i, int max )
{
    int gray = 255*i/max;
    return MAKE_RGBA(gray,gray,gray,0);
}

