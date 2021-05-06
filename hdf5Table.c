/* -*- Mode:C; Coding:us-ascii-unix; fill-column:132 -*- */
/* ****************************************************************************************************************************** */
/**
   @file      hdf5Table.c
   @author    Mitch Richling <https://www.mitchr.me/>
   @Copyright Copyright 1998 by Mitch Richling.  All rights reserved.
   @brief     Use of HDF5 high level table API.@EOL
   @Keywords  hdf5 compound types table high level database db
   @Std       C89 HDF5

   This example program demonstrates the use of the HDF5 high level table API to simulate the record/field structure of a database
   table with HDF5.  For more detail regarding how to construct HDF5 compound types from the ground up, see hdf5Compound.c.

   Several functions are illustrated:
     - H5TBwrite_records     
     - H5TBread_records
     - H5TBappend_records
     - H5TBmake_table
     - H5TBread_table

   Even more functions are not illustrated:
     - H5TBadd_records_from    
     - H5TBcombine_tables      
     - H5TBdelete_field      
     - H5TBdelete_record       
     - H5TBget_field_info    
     - H5TBget_table_info    
     - H5TBinsert_field        
     - H5TBinsert_record     
     - H5TBread_fields_index 
     - H5TBread_fields_name  
     - H5TBwrite_fields_index  
     - H5TBwrite_fields_name   

*/
/* ------------------------------------------------------------------------------------------------------------------------------ */

/* ****************************************************************************************************************************** */
#include <stdio.h>              /* I/O lib         ISOC  */
#include <string.h>             /* Strings         ISOC  */
#include <hdf5.h>               /* HDF5 files            */
#include <hdf5_hl.h>            /* High level API  HDF5  */

/* ****************************************************************************************************************************** */
#include "mjrHDF5.h"

/* ****************************************************************************************************************************** */
/* This struct can be "tuned" to have "holes" on just about any hardware platform.  It is thus a good example of how to create HDF5
   compound data types that describe things in memory vs. describing them on disk in a portable way.*/
#define NUM_FIELDS 4
#define PR_NAME_LEN 32
typedef struct personRecordStr {
  char           name[PR_NAME_LEN];
  unsigned char  age;
  int            weight;
  float          IQ;
} personRecord_t;

#define NUM_RECS 10000
#define TST_FILE_NAME "aCpndtestFile.h5"

/* ****************************************************************************************************************************** */
int main(int argc, char *argv[]) {
  personRecord_t outPersonArr[NUM_RECS];
  personRecord_t inPersonArr[NUM_RECS];
  hid_t stringTypeID;
  int i;
  hid_t   fileID;
  herr_t  hErrVal;
  /* We set the chunk size to 10 record lengths.  For slow growth tables, this works just fine. */
  hsize_t    chunk_size          = 10*(sizeof(outPersonArr[0].name)+
                                       sizeof(outPersonArr[0].age)+
                                       sizeof(outPersonArr[0].weight)+
                                       sizeof(outPersonArr[0].IQ));
  /* Now we compute the offsets from the start of the struct */
  size_t pRec_offset[NUM_FIELDS] = { HOFFSET(personRecord_t, name),
                                     HOFFSET(personRecord_t, age),
                                     HOFFSET(personRecord_t, weight),
                                     HOFFSET(personRecord_t, IQ) };
  /* Give everything names so we can get fields by name */
  const char *pRec_names[] = { "name", "age", "weight", "IQ" };
  /* Now we compute the sizes of each record -- needed for reading the data back */
  size_t pRec_sizes[NUM_FIELDS] = { sizeof(outPersonArr[0].name),
                                    sizeof(outPersonArr[0].age),
                                    sizeof(outPersonArr[0].weight),
                                    sizeof(outPersonArr[0].IQ)};
  /* This will hold the types of each record -- set later */
  hid_t field_type[NUM_FIELDS];
  /* Used in several calls later. */
  size_t pRec_size = sizeof(personRecord_t);

  /* Fill in some bogus data for the the array. */
  for(i=0;i<NUM_RECS;i++) {
    strcpy(outPersonArr[i].name, "Mitch Richling");
    outPersonArr[i].age    = i+23;
    outPersonArr[i].weight = 123+i;
    outPersonArr[i].IQ     = 200-(i*2.5+2)/(i+1);
  }
  
  /* Load the library -- not required on most platforms. */
  hErrVal = H5open();
  mjrHDF5_chkError(hErrVal);

  /* Create the string type for the .name part of a personRecord_t */
  stringTypeID = H5Tcopy(H5T_C_S1);
  mjrHDF5_chkError(stringTypeID);
  hErrVal = H5Tset_size(stringTypeID, PR_NAME_LEN);
  mjrHDF5_chkError(hErrVal);
  /* Set the field types for the table */
  field_type[0] = stringTypeID;
  field_type[1] = H5T_NATIVE_UCHAR;
  field_type[2] = H5T_NATIVE_INT;
  field_type[3] = H5T_NATIVE_FLOAT;

  /* Create a new file using default properties. */
  fileID = H5Fcreate(TST_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  mjrHDF5_chkError(fileID);

  /* Print the first element going to disk. */
  printf("1 From Disk:   Name(%s) Age(%d) Weight(%d) IQ(%f)\n", 
         outPersonArr[0].name, (int)outPersonArr[0].age, outPersonArr[0].weight, outPersonArr[0].IQ);

  hErrVal = H5TBmake_table ("The Table", fileID, "dset", NUM_FIELDS, NUM_RECS, 
                            pRec_size, pRec_names, pRec_offset, field_type,      // About the struct
                            chunk_size,                                          // Set above..
                            NULL,                                                // Fill
                            0,                                                   // Don't Compress
                            outPersonArr);
  mjrHDF5_chkError(fileID);

  hErrVal = H5TBread_table(fileID, "dset", pRec_size, pRec_offset, pRec_sizes, inPersonArr);
  mjrHDF5_chkError(fileID);

  /* Print the first element from disk. */
  printf("2 From Disk:   Name(%s) Age(%d) Weight(%d) IQ(%f)\n", 
         inPersonArr[0].name, (int)inPersonArr[0].age, inPersonArr[0].weight, inPersonArr[0].IQ);

  /* Read one record starting at the last record */
  hErrVal = H5TBread_records(fileID, "dset", NUM_RECS-1, 1, pRec_size, pRec_offset, pRec_sizes, inPersonArr);
  mjrHDF5_chkError(fileID);

  /* Print the first element in the array -- which is the last record on disk. */
  printf("3 Last Record: Name(%s) Age(%d) Weight(%d) IQ(%f)\n", 
         inPersonArr[0].name, (int)inPersonArr[0].age, inPersonArr[0].weight, inPersonArr[0].IQ);

  /* Add three records to the end */
  H5TBappend_records (fileID, "dset", 3, pRec_size, pRec_offset, pRec_sizes, outPersonArr);

  /* Read the new last record */
  hErrVal = H5TBread_records(fileID, "dset", NUM_RECS+2, 1, pRec_size, pRec_offset, pRec_sizes, inPersonArr);
  mjrHDF5_chkError(fileID);

  /* Print the first element in the array -- which is the last record on disk. */
  printf("4 Last Record: Name(%s) Age(%d) Weight(%d) IQ(%f)\n", 
         inPersonArr[0].name, (int)inPersonArr[0].age, inPersonArr[0].weight, inPersonArr[0].IQ);

  /* Overwrite the last record with the first record */
  hErrVal = H5TBwrite_records(fileID, "dset", NUM_RECS+2, 1, pRec_size, pRec_offset, pRec_sizes, outPersonArr);
  mjrHDF5_chkError(fileID);

  /* Read the new last record */
  hErrVal = H5TBread_records(fileID, "dset", NUM_RECS+2, 1, pRec_size, pRec_offset, pRec_sizes, inPersonArr);
  mjrHDF5_chkError(fileID);

  /* Print the first element in the array -- which is the last record on disk. */
  printf("5 Last Record: Name(%s) Age(%d) Weight(%d) IQ(%f)\n", 
         inPersonArr[0].name, (int)inPersonArr[0].age, inPersonArr[0].weight, inPersonArr[0].IQ);

  /* Terminate access to the string data type. */ 
  hErrVal = H5Tclose(stringTypeID);
  mjrHDF5_chkError(hErrVal);
  
  /* Close the file. */
  hErrVal = H5Fclose(fileID);
  mjrHDF5_chkError(hErrVal);

  /* Unload the library and free any remaining resources. */
  hErrVal = H5close();
  mjrHDF5_chkError(hErrVal);

  return 0;
} /* end func main */
