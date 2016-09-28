#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

/*----- my code -------*/

int rc;
//pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * THREAD_NUM);
pthread_t tid[THREAD_NUM];
//pthread_attr_t attr;

//pthread_attr_init(&attr);
//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


for(int j=0; j<THREAD_NUM; j++){
	

input *box = (input*) malloc (sizeof(input));
box->pixels = pixels;
COPY_COLOR(box->background_color,background);
box->rectangulars = rectangulars;
box->spheres = spheres;
box->lights = lights;
box->view = &view;
box->width = ROWS;
box->height = COLS;


	box->j = j * (COLS / THREAD_NUM);
  	rc = pthread_create(&tid[j], NULL, raytracing, (void*) box);
  	if (rc) {              
    		printf("ERROR; return code from pthread_create() is %d\n", rc);
    		exit(-1);
   	 }
}
for(int j=0; j<THREAD_NUM; j++){	
	rc = pthread_join(tid[j], NULL);
  	if (rc) {
    		printf("ERROR; return code from pthread_join() is %d\n", rc);
    	exit(-1);
    }
}

/*--------------------*/

//    raytracing(pixels, background,
//               rectangulars, spheres, lights, &view, ROWS, COLS);
    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
    free(pixels);
    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    pthread_exit(NULL);
    return 0;
}
