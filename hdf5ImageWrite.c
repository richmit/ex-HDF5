/* -*- Mode:C; Coding:us-ascii-unix; fill-column:132 -*- */
/* ****************************************************************************************************************************** */
/**
   @file      hdf5ImageWrite.c
   @author    Mitch Richling <https://www.mitchr.me/>
   @Copyright Copyright 1998 by Mitch Richling.  All rights reserved.
   @brief     Create an HDF5 file and write an image file into it.@EOL
   @Keywords  none
   @Std       C89

   This program serves as an example the high level HDF5 image API used to write a truecolor image to an HDF5 file.

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

#define WIDTH  8
#define HEIGHT 4

#define TST_FILE_NAME "imgTestFile.h5"
#define IMG_NAME      "anImage"

/* ****************************************************************************************************************************** */
int main(int argc, char *argv[]) {
  hid_t         fileID;
  herr_t        hErrVal;
  int           i, j;
  unsigned char imgBuf[HEIGHT][WIDTH][3]; /* 'unsigned char' is mandated by the HDF5 library.  */

  /* Create phony data. */
  printf("The image data:\n");
  for(i=0; i<HEIGHT; i++) {
    for(j=0; j<WIDTH; j++) {
      imgBuf[i][j][0] = ((i>=(HEIGHT/2)) ? 0   : 255);  // red   magenta
      imgBuf[i][j][1] = ((i>=(HEIGHT/2)) ? 255 : 0  );  // green cyan
      imgBuf[i][j][2] = ((j>=(WIDTH/2))  ? 255 : 0  );
      printf("%02x/%02x/%02x ", imgBuf[i][j][0], imgBuf[i][j][1], imgBuf[i][j][2]);
    }
    printf("\n");
  }
   
  /* Load the library -- not required on most platforms. */
  hErrVal = H5open();
  mjrHDF5_chkError(hErrVal);

  /* Create a new file using default properties. */
  fileID = H5Fcreate(TST_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  mjrHDF5_chkError(fileID);

  /* Create the data space, create the dataset, and write the data.  WIDTH & HEIGHT converted to hsize_t. */
  hErrVal=H5IMmake_image_24bit(fileID, IMG_NAME, WIDTH, HEIGHT, "INTERLACE_PIXEL", (const unsigned char *)imgBuf);
  mjrHDF5_chkError(hErrVal);
  
  /* Close the file. */
  hErrVal = H5Fclose(fileID);
  mjrHDF5_chkError(hErrVal);

  /* Unload the library and free any remaining resources. */
  hErrVal = H5close();
  mjrHDF5_chkError(hErrVal);

  return 0;
} /* end func main */
