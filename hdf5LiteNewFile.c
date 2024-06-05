/* -*- Mode:C; Coding:us-ascii-unix; fill-column:132 -*- */
/* ****************************************************************************************************************************** */
/**
   @file      hdf5LiteNewFile.c
   @author    Mitch Richling <https://www.mitchr.me/>
   @Copyright Copyright 1998 by Mitch Richling.  All rights reserved.
   @brief     Create an HDF5 file via the lite API.@EOL
   @Keywords  none
   @Std       C89

   This program serves as an example of the high level API.  You can't do as much with the high level API, but it is good enough for
   many simple applications.
              
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
#define MAX_X 10
#define MAX_Y 20
#define MAX_T 24

#define TST_FILE_NAME "ltTestFile.h5"

/* ****************************************************************************************************************************** */
int main(int argc, char *argv[]) {
  hid_t   fileID;
  hsize_t dims[3] = {MAX_X, MAX_Y, MAX_T};
  herr_t  hErrVal;
  int     id, i, j, k;
  float   temp[MAX_X][MAX_Y][MAX_T];

  /* Create phony data. */
  for(i=0,id=0; i<MAX_X; i++)
    for(j=0; j<MAX_Y; j++)
      for(k=0; k<MAX_T; k++)
        temp[i][j][k] = id++;
   
  /* Load the library -- not required on most platforms. */
  hErrVal = H5open();
  mjrHDF5_chkError(hErrVal);

  /* Create a new file using default properties. */
  fileID = H5Fcreate(TST_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  mjrHDF5_chkError(fileID);

  /* Create and write the dataset all at once */
  hErrVal = H5LTmake_dataset_float(fileID, "/dset", 2, dims, (float *)temp);
  mjrHDF5_chkError(hErrVal);

  /* Create "global" attrbutes.. */
  hErrVal = H5LTset_attribute_string(fileID, "/", "Author", "Mitch Richling");
  mjrHDF5_chkError(hErrVal);
  hErrVal = H5LTset_attribute_string(fileID, "/", "title",  "Example File");
  mjrHDF5_chkError(hErrVal);

  /* Close the file. */
  hErrVal = H5Fclose(fileID);
  mjrHDF5_chkError(hErrVal);

  /* Unload the library and free any remaining resources. */
  hErrVal = H5close();
  mjrHDF5_chkError(hErrVal);

  return 0;
} /* end func main */
