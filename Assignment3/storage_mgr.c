#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "storage_mgr.h"
#include "dberror.h"

/************************************************************
 *                    manipulating page files               *
 ************************************************************/
void initStorageManager (void)
{

}
/*Creates a Page File (as a binary file) with fileName and inital size = 1 page
  filled with '\0' bytes*/
RC createPageFile (char *fileName)
{

    //Creating File with write binary (wb)
    FILE *fpointer;
    fpointer = fopen(fileName,"wb"); //wb creates a file if it cannot find one in directory

    //FILE NOT FOUND AND IS NOT WRITTEN;
    if (fpointer==NULL)
    {
        return RC_FILE_NOT_FOUND;
    }

    //Creating the new Page handle and initializing it
    SM_PageHandle newPage = (SM_PageHandle) calloc(PAGE_SIZE, sizeof(char)); //same as malloc except sets mem to 0
    int writePage = fwrite(newPage, sizeof(char), PAGE_SIZE, fpointer); //initalize

    //make sure write occured
    if (writePage==0)
    {
        return RC_WRITE_FAILED;
    }

    //Deallocate memory and close pointer.
    free(newPage);
    fclose(fpointer);

    return RC_OK;
} //end createPageFile

/*Opens a file given a file name and file handle with information about the file inside
  */
RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
    //initialize pointer and open file with r+b
    FILE *fpointer;
    fpointer = fopen(fileName,"r+b"); //read and write binary

    //Check to see if the file exists
    if (fpointer==NULL)
    {
        return RC_FILE_NOT_FOUND;
    }

    fseek(fpointer, 0, SEEK_END); //move file pointer to EOF
    long length = ftell(fpointer); //Get the length of the file. Returns current file pos.
    int totalPages = (int)length/PAGE_SIZE; //calculating the total number of pages in the file

    //Setting the file handle attributes
    fHandle->fileName = fileName;
    fHandle->totalNumPages = totalPages;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fpointer;
    return RC_OK;
} //end openPageFile

/*Closes a file that is open given a file handle */
RC closePageFile (SM_FileHandle *fHandle)
{
    //Close file as well as checking that it was found
    if(fclose(fHandle->mgmtInfo)==0) //close file
    {
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
} // end closePageFile

/*Destroys a file given a file name*/
RC destroyPageFile (char *fileName)
{
    //Destroy the file as well as checking that it was found
    if(remove(fileName)==0)
    {
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}


/************************************************************
 *                    Reading Blocks from a Disk            *
 ************************************************************/

/*reads the pageNumth block from a file and stores its content in the memory
  pointed to by the memPage page handle.
  If the file has less than pageNum pages, the method should return
  RC_READ_NON_EXISTING_PAGE.*/

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    //check for a nonexistent page
    if (fHandle->totalNumPages < pageNum) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    //Set stream in POSIX file descriptor to the pageNumth page (absolute pos)
    fseek(fHandle->mgmtInfo, sizeof(char) * PAGE_SIZE * pageNum, SEEK_SET);

    //Read PAGE_SIZE chars from the stream char by char, result in memPage
    fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);

    //Update the fileHandle
    fHandle->curPagePos = pageNum;

    return RC_OK;
} //end readBlock

/*Return the current page position in a file*/
int getBlockPos(SM_FileHandle *fHandle)
{
    return fHandle->curPagePos;
} //end getBlockPos

/*Read the first Block in a File*/
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(0, fHandle, memPage);
}//end readFirstBlock

/*Read the previous Block in a File from the pointer*/
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}//end readPreviousBlock

/*Read the current Block in a file*/
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}//end readCurrentBlock

/*Read the next Block from the current position in a file*/
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}//end readNextBlock

/*Read the last block in a file*/
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}//end readLastBlock


/************************************************************
 *                    writing blocks to disc                *
 ************************************************************/

/* writing blocks to a page file Absolute position. It is already open when I get it*/
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  ensureCapacity(fHandle->totalNumPages,fHandle);
  int placement = fseek(fHandle->mgmtInfo, pageNum * PAGE_SIZE * sizeof(char), SEEK_SET); //find the correct page

  int writtenSize = fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo); //write o the page
  fHandle->curPagePos = pageNum; //update the current page information to reflect new page after write

  return RC_OK;
}//end writeBlock

/*write blocks to a page at the current address*/
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  writeBlock (fHandle->curPagePos, fHandle, memPage); //use writeblock to locate correct page and write there

  return RC_OK;
}//end writeCurrentBlock

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
}//end appendEmptyBlock

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
}//end ensureCapacity