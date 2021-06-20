/*
 * Main.cpp
 *
 *  Created on: 07/12/2019
 *      Author: Ivan Alvarez Lopez & Ignacio Gomez Gasch
 */
#include "CImg.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
using namespace cimg_library;

#define THRESHOLD 127
#define THREADS 4

const float Rfactor = 0.3;
const float Gfactor = 0.59;
const float Bfactor = 0.11;

int sizePerLayer;
int* pLnewOrigin;
int* pComps[3]; // Pointers to the R, G and B components

void* ThreadProcGrayScalise(void* arg) {
	int* pLnew = (int*) arg;
	for (int i = 0; i < (sizePerLayer/THREADS); i++)
		*(pLnew + i) = ((*(pComps[0] + (pLnew - pLnewOrigin) + i)) * Rfactor) 
						+ ((*(pComps[1] + (pLnew - pLnewOrigin) + i)) * Gfactor) 
						+ ((*(pComps[2] + (pLnew - pLnewOrigin) + i)) * Bfactor);
	return NULL;
}

void* ThreadProcBinarise(void* arg) {
	int* pLnew = (int*) arg;
	for (int i = 0; i < (sizePerLayer/THREADS); i++)
		*(pLnew + i) = (*(pLnew + i) < THRESHOLD)? 0 : 255;
	return NULL;
}

int main() {

	CImg<int> srcImage("bailarina.bmp"); // Open file and object initialization

	int *pLnew;
	int *pdstImage; // Pointer to the new image pixels
	int width, height; // Width and height of the image
	int nComp; // Number of image components

	/***************************************************
	 *
	 * Variables inicialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	// srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	sizePerLayer = width*height;
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (int *) malloc (sizePerLayer * 1 * sizeof(int));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

	// Pointers to the RGB arrays of the source image
	pComps[0] = srcImage.data(); // pRcomp points to the R component
	pComps[1] = pComps[0] + sizePerLayer; // pGcomp points to the G component
	pComps[2] = pComps[1] + sizePerLayer; // pBcomp points to B component

	// Pointers to the RGB arrays of the destination image
	pLnew = pdstImage;
	pLnewOrigin = pLnew;

	pthread_t threadGrayScaler[THREADS];
	pthread_t threadBinariser[THREADS];

	/*********************************************
	 * Algorithm start
	 *
	 * Take time to measure
	 *
	 */

	struct timespec initialTime, finalTime;
	double dElapsedTimeS;

	printf("Running task    : ");
	if (clock_gettime(CLOCK_REALTIME, &initialTime))
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	/************************************************
	 *       Algorithm.
	 *
	 * Algoritmo a implementar: Binarizacion de imagenes:
	 *		Convierte una imagen en otra que solo usa los colores blanco y negro en dos fases:
	 *			- La primera convierte la imagen original a escala de grises:
	 *				L[i] = 0,3*R[i] + 0,59*G[i] + 0,11 *B[i]
	 *			- La segunda realiza la binarizacion propiamente dicha.
	 * 
	 */

	for (int times = 1; times < 50; times++) {
		// threads for gray scale conversion
		for (int j = 0; j < THREADS; j++) {
			if (pthread_create(&threadGrayScaler[j], NULL, ThreadProcGrayScalise, (pLnew + (sizePerLayer/THREADS) * j)) != 0) {
				fprintf(stderr, "ERROR: Creating thread adder %d\n", j);
				return EXIT_FAILURE;
			}
		}

		for (int j = 0; j < THREADS; j++)
			pthread_join(threadGrayScaler[j], NULL);

		// Photo converted to grayscale succesfully

		// Binarise values
		for (int j = 0; j < THREADS; j++) {
			if (pthread_create(&threadBinariser[j], NULL, ThreadProcBinarise, (pLnew + (sizePerLayer/THREADS) * j)) != 0) {
				fprintf(stderr, "ERROR: Creating thread adder %d\n", j);
				return EXIT_FAILURE;
			}
		}

		for (int j = 0; j < THREADS; j++)
			pthread_join(threadBinariser[j], NULL);
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Take a new time to measure
	 *
	 * Calculate time spent in the run
	 *
	 */

	if (clock_gettime(CLOCK_REALTIME, &finalTime))
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	dElapsedTimeS = (finalTime.tv_sec - initialTime.tv_sec);
	dElapsedTimeS += (finalTime.tv_nsec - initialTime.tv_nsec) / 1e+9;

	/******************************************************
	 * Store algorithm result in the disk
	 *
	 * Show time spent in the algorithm in the screen
	 *
	 *
	 */

	printf("Elapsed time    : %f s.\n", dElapsedTimeS);

	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	nComp = 1;
	CImg<int> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp"); 

	free(pdstImage);
	
	// Display the destination image
	// dstImage.display(); // If needed, show the result image
	return(0);
}
