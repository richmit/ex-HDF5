/* -*- Mode:C; Coding:us-ascii-unix; fill-column:132 -*- */
/* ****************************************************************************************************************************** */
/**
   @file      hdf5ImageRead.c
   @author    Mitch Richling <https://www.mitchr.me/>
   @Copyright Copyright 1998 by Mitch Richling.  All rights reserved.
   @brief     Create an HDF5 file and write an image file into it.@EOL
   @Keywords  none
   @Std       C89

   This program serves as an example the high level HDF5 image API used to read a truecolor image from an HDF5 file.

   Aside from the error checking functions in mjrHDF5.[hc], this source code is self contained.
              
*/
/* ------------------------------------------------------------------------------------------------------------------------------ */

/* ****************************************************************************************************************************** */
#include <stdio.h>              /* I/O lib         ISOC  */
#include <stdlib.h>             /* Standard Lib    ISOC  */
#include <string.h>             /* Strings         ISOC  */
#include <hdf5.h>               /* Low level API   HDF5  */
#include <hdf5_hl.h>            /* High level API  HDF5  */

/* ****************************************************************************************************************************** */
#include "mjrHDF5.h"

/* ****************************************************************************************************************************** */
#define TST_FILE_NAME "imgTestFile.h5"
#define IMG_NAME      "anImage"

/* ****************************************************************************************************************************** */
int main(int argc, char *argv[]) {
  hid_t         fileID;
  herr_t        hErrVal;
  hsize_t       width;
  hsize_t       height;
  hsize_t       planes;
  char          interlace[16];
  hssize_t      npals;
  int           i, j;
  unsigned char *imgBuf; /* 'unsigned char' is mandated by the HDF5 library.  */

  /* Load the library -- not required on most platforms. */
  hErrVal = H5open();
  mjrHDF5_chkError(hErrVal);

  /*  Open an existing file. */
  fileID = H5Fopen(TST_FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
  mjrHDF5_chkError(fileID);

  /* Figure out if the data is an image or not */
  hErrVal = H5IMis_image(fileID, IMG_NAME);
  mjrHDF5_chkError(hErrVal);
  if(hErrVal == 1) {
    printf("The data s an image\n");
  } else {
    printf("The data is NOT an image\n");
    exit(1);
  }

  /* Get info about the image */
  hErrVal = H5IMget_image_info (fileID, IMG_NAME, &width, &height, &planes, interlace, &npals); 
  mjrHDF5_chkError(hErrVal);

  printf("Image Meta-Data:\n");
  printf("  Width:     %d\n",      (int)width);
  printf("  Height:    %d\n",      (int)height);
  printf("  Planes:    %d (%s)\n", (int)planes, (planes==3         ?"OK":"ERROR"));
  printf("  NPals:     %d (%s)\n", (int)npals,  (npals==0          ?"OK":"ERROR"));
  printf("  Interlace: %s (%s)\n", interlace,   (interlace[11]=='I'?"OK":"ERROR"));

  /* If the image is the wrong type, then exit. */
  if( !((planes==3) && (npals==0) && (interlace[11]=='I'))) {
    printf("ERROR: Image is not 24-bit truecolor or pixel layout is not INTERLACE_PIXEL\n");
    exit(1);
  }

  /* Allocate space for the image */
  imgBuf = (unsigned char*) malloc (width * height * planes * sizeof( unsigned char ));
  if (imgBuf == NULL) {
    printf("ERROR: Could not allocate space for image.\n");
    exit(1);
  }

  /* Read the data from the file */
  hErrVal = H5IMread_image (fileID, IMG_NAME, imgBuf);
  mjrHDF5_chkError(hErrVal);

  /* Display the image data. */
  printf("The image data:\n");
  for(i=0; i<height; i++) {
    for(j=0; j<width; j++) {
      printf("%02x/%02x/%02x ", 
             imgBuf[i*planes*width + j*planes + 0],
             imgBuf[i*planes*width + j*planes + 1],
             imgBuf[i*planes*width + j*planes + 2]);
    }
    printf("\n");
  }
  
  /* Close the file. */
  hErrVal = H5Fclose(fileID);
  mjrHDF5_chkError(hErrVal);

  /* Unload the library and free any remaining resources. */
  hErrVal = H5close();
  mjrHDF5_chkError(hErrVal);

  return 0;
} /* end func main */
