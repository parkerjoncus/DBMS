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

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->totalNumPages < pageNum) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    //Set stream in POSIX file descriptor to the pageNumth page
    fseek(fHandle->mgmtInfo, sizeof(char) * PAGE_SIZE * pageNum, SEEK_SET);

    //Read PAGE_SIZE chars from the stream char by char, result in memPage
    fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);

    //Update the fileHandle
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

int getBlockPos(SM_FileHandle *fHandle) {
    return fHandle->curPagePos;
}

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(0, fHandle, memPage);
}

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}