

#include <stdio.h>
#include <stdlib.h>
#inslude <string.h>
#include "storage_mgr.h"
#include "dberror.h"

void initStorageManager (void){
}

RC createPageFile (char *fileName){
    FILE *fpointer;
    fpointer = fopen(fileName,"wb");
    if (fp==NULL){
        return RC_FILE_NOT_FOUND;
    }
    SM_PageHandle newPage = (SM_PageHandle) calloc(PAGE_SIZE, size(char));
    int writePage = fwrite(newPage, sizeof(char), PAGE_SIZE, fpointer);
    if (writePage==0){
        return RC_WRITE_FAILED;
    }
    free(newpage);
    fclose(fpointer);
    return RC_OK;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle){
    FILE *fpointer;
    fpointer = fopen(*fileName,"r+b");
    if (fpointer==NULL){
        return RC_FILE_NOT_FOUND;
    }
    fseek(fpointer, 0, SEEK_END);
    long length = ftell(fpointer);
    int totalPages = (int)length/PAGE_SIZE;
    fHandle->fileName = fileName;
    fHandle->totalNumPages = totalPages;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fpointer;
    return RC_OK;
}

RC closePageFile (SM_FileHandle *fHandle){
    if(fclose(*fHandle->mgmtInfo)==0){
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}

RC destroyPageFile (char *fileName){
    if(remove(fileName)==0){
        return RC_OK;
    }
    return RC_FILE_NOT_FOUND;
}
