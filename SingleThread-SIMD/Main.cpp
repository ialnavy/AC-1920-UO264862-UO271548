/*
 *  Created on: 24 sept. 2019
 *      Author: arias
 */

// Example of use of intrinsic functions
// This example doesn't include any code about image processing
// The image processing code must be added by the students and remove the unnecessary code.

#include "CImg.h"
#include <malloc.h>
#include <errno.h>
#include <stdio.h>

// Required to use intrinsic functions
#include <xmmintrin.h>

using namespace cimg_library;

// #define TAM 18 // Array size. Note: It is not a multiple of 8
#define ELEMENTSPERPACKET (sizeof(__m128)/sizeof(float))
#define THRESHOLD 127.0

int main() {
    CImg<float> srcImage("bailarina.bmp"); // Open file and object initialization

	float *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
    float *pLnew;
	int width, height; // Width and height of the image
	int nComp; // Number of image components

    struct timespec initialTime, finalTime;
	double dElapsedTimeS;

    /********************************
     * Here: variable initialization for handling images
     */

    // srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

    int TAM = width * height;

    // Data arrays to sum. Might be o not memory aligned to __m128 size (32 bytes)

    // Pointers to the RGB arrays of the source image
	pRcomp = srcImage.data(); // pRcomp points to the R component
	pGcomp = pRcomp + TAM; // pGcomp points to the G component
	pBcomp = pGcomp + TAM; // pBcomp points to B component

    // Calculation of the size of the results array
    // How many 128 bit packets fit in the array?
    int NPackets = (TAM * sizeof(float)/sizeof(__m128));
    // If is not a exact number we need to add one more packet
    if ( ((TAM * sizeof(float))%sizeof(__m128)) != 0)
        NPackets++;
   
    // Create an array aligned to 16 bytes (128 bits) memory boundaries to store the sum.
    // Aligned memory access improves performance    
    pLnew = (float *)_mm_malloc(sizeof(__m128) * NPackets, sizeof(__m128));
    if (pLnew == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

    // 16 bytes (128 bits) packets. Used to stored aligned memory data
    __m128 vRcomp, vGcomp, vBcomp;
	__m128 vRgrayscale, vGgrayscale, vBgrayscale;
	__m128 vGrayScale;
	__m128 rFactor, gFactor, bFactor;
	
	rFactor = _mm_set1_ps(0.3);
	gFactor = _mm_set1_ps(0.59);
	bFactor = _mm_set1_ps(0.11);

    printf("Running task    : ");
	if (clock_gettime(CLOCK_REALTIME, &initialTime)) {
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
    for (int times = 1; times < 50; times++) {
        for (int i = 0; i < NPackets; i++) {
			vRcomp = _mm_load_ps(&pRcomp[i*ELEMENTSPERPACKET]);
			vGcomp = _mm_load_ps(&pGcomp[i*ELEMENTSPERPACKET]);
			vBcomp = _mm_load_ps(&pBcomp[i*ELEMENTSPERPACKET]);
			
            vRgrayscale = _mm_mul_ps(vRcomp,rFactor);
			vGgrayscale = _mm_mul_ps(vGcomp,gFactor);
			vBgrayscale = _mm_mul_ps(vBcomp,bFactor);

            vGrayScale = _mm_add_ps(vRgrayscale,_mm_add_ps(vGgrayscale,vBgrayscale));

            __m128 auxPackage;

			for (int k = 0; k < ELEMENTSPERPACKET; k++)
				auxPackage[k] = (vGrayScale[k] > THRESHOLD) ? 255:0;

            *(__m128 *) (pLnew + i*ELEMENTSPERPACKET) = auxPackage;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &finalTime)) {
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
	CImg<float> dstImage(pLnew, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp"); 

    _mm_free(pLnew);
	
	// Display the destination image
	// dstImage.display(); // If needed, show the result image
	return(0);
}
