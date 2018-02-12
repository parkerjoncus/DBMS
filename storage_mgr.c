

#include <stdio.h>
#include <stdlib.h>
#inslude <string.h>
#include "storage_mgr.h"
#include "dberror.h"

void initStorageManager (void){
}
//Creates a Page File with fileName
RC createPageFile (char *fileName){

    //Creating File with write binary (wb)
    FILE *fpointer;
    fpointer = fopen(fileName,"wb");

    //Check and make sure it worked and the file exists
    if (fpointer==NULL){
        return RC_FILE_NOT_FOUND;
    }

    //Creating the new Page handle and initializing it
    SM_PageHandle newPage = (SM_PageHandle) calloc(PAGE_SIZE, size(char));
    int writePage = fwrite(newPage, sizeof(char), PAGE_SIZE, fpointer);

    //Check that the write worked
    if (writePage==0){
        return RC_WRITE_FAILED;
    }

    //Deallocate memory and close pointer.
    free(newpage);
    fclose(fpointer);
    return RC_OK;
}

//Opens a file given a file name and file handle
RC openPageFile(char *fileName, SM_FileHandle *fHandle){
    //initialize pointer and open file with r+b
    FILE *fpointer;
    fpointer = fopen(*fileName,"r+b");

    //Check to see if the file exists
    if (fpointer==NULL){
        return RC_FILE_NOT_FOUND;
    }

    //Gets to the end of the file
    fseek(fpointer, 0, SEEK_END);
    //Get the length of the file
    long length = ftell(fpointer);
    //calculating the total number of pages in the file
    int totalPages = (int)length/PAGE_SIZE;

    //Setting the file handle attributes
    fHandle->fileName = fileName;
    fHandle->totalNumPages = totalPages;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fpointer;
    return RC_OK;
}

//Closes a file given a file handle
RC closePageFile (SM_FileHandle *fHandle){
    //Close file as well as checking that it was found
    if(fclose(*fHandle->mgmtInfo)==0){
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}

//Destroys a file given a file name
RC destroyPageFile (char *fileName){
    //Destroy the file as well as checking that it was found
    if(remove(fileName)==0){
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}



/* writing blocks to a page file Absolute position. It is already open when I get it*/
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  int placement = fseek(fHandle->mgmtInfo, pageNum * PAGE_SIZE * sizeof(char), SEEK_SET);

  int writtenSize = fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
  fHandle->curPagePos = pageNum;


  return RC_OK;
};

/*write blocks to a page at the current address*/
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  writeBlock (fHandle->curPagePos, fHandle, memPage);

  return RC_OK;
};

/*Appends an empty block to the binary file filled with 0*/
extern RC appendEmptyBlock (SM_FileHandle *fHandle)
{
  FILE * fpointer = fHandle->mgmtInfo; //open the file we are reading from "ab" signifies append binary

  int i; //counter to reach 2000 bytes or chars AKA 1 page
  char zero_bytes = ' '; //symbol to fill the empty space with

  for (i=0; i<PAGE_SIZE; i++)
  {

    fwrite(&zero_bytes, sizeof(zero_bytes), 1,fpointer); //writing 2000 chars (check with ls -l in linux)

  }
  fHandle->totalNumPages++; //increment the total number of pages

  return RC_OK;
};
/*Checks to see if there is enough 0 space. If there is not call appendEmptyBlock*/
extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{

   FILE *fpointer;

   fpointer = fHandle->mgmtInfo;

   if (fHandle->totalNumPages>numberOfPages) //not enough space
   {
      fHandle->totalNumPages = numberOfPages;
      appendEmptyBlock(fHandle); //appen space
      printf("Appending empty block....\n");

    }

  return RC_OK;
};
