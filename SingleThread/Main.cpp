/*
 * Main.cpp
 *
 *  Created on: 13 sept. 2018
 *      Author: arias
 */

#include "CImg.h"
#include <math.h>
#include <stdio.h>
#include <errno.h> 
#include <pthread.h>
using namespace cimg_library;

#define THRESHOLD 127

/**********************************
 * TODO
 * 	- Change the data type returned by CImg.srcImage to adjust the
 * 	requirements of your workgroup
 * 	- Change the data type of the components pointers to adjust the
 * 	requirements of your workgroup
 */

int main() {
	CImg<int> srcImage("bailarina.bmp"); // Open file and object initialization

	int *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
	int *pLnew;
	int *pdstImage; // Pointer to the new image pixels
	int width, height; // Width and height of the image
	int nComp; // Number of image components


	/***************************************************
	 *
	 * Variables initialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	// srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)


	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (int *) malloc (width * height * 1 * sizeof(int));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

	// Pointers to the RGB arrays of the source image
	pRcomp = srcImage.data(); // pRcomp points to the R component
	pGcomp = pRcomp + height * width; // pGcomp points to the G component
	pBcomp = pGcomp + height * width; // pBcomp points to B component

	// Pointers to the RGB arrays of the destination image
	pLnew = pdstImage;

	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 *
	 *	COMPLETE
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
	 * Algorithm.
	 * In this example, the algorithm is a components exchange
	 *
	 * TO BE REPLACED BY YOUR ALGORITHM
	 *
	 * Tipo base de las im�genes: int
	 *
	 * Algoritmo a implementar: Binarizaci�n de im�genes:
	 *		Convierte una imagen en otra que s�lo usa los colores blanco y negro en dos fases:
	 *			- La primera convierte la imagen original a escala de grises:
	 *				L[i] = 0,3*R[i] + 0,59*G[i] + 0,11 *B[i]
	 *			- La segunda realiza la binarizaci�n propiamente dicha.
	 */
	for (int times = 1; times < 50; times++) {
		for (int i = 0; i < width * height; i++){
			*(pLnew + i) = (0.3 * (*(pRcomp + i)) + 0.59 * (*(pGcomp + i)) + 0.11 * (*(pBcomp + i)) < THRESHOLD) ? 0 : 255 ;
		}
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 *
	 * COMPLETE
	 *
	 */

	if (clock_gettime(CLOCK_REALTIME, &finalTime))
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	dElapsedTimeS = (finalTime.tv_sec - initialTime.tv_sec);
	dElapsedTimeS += (finalTime.tv_nsec - initialTime.tv_nsec) / 1e+9;
	printf("Elapsed time    : %f s.\n", dElapsedTimeS);
	
	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	nComp = 1;
	CImg<int> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp"); 
	
	// Display the destination image
	// dstImage.display(); // If needed, show the result image
	return(0);

}


