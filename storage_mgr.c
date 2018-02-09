

#include <stdio.h>
#include <stdlib.h>
#inslude <string.h>
#include "storage_mgr.h"
#include "dberror.h"

void initStorageManager (void){
    
}

RC createPageFile (char *filename){
    FILE *fpointer;
    fpointer = fopen(filename,"wb");
    if (fp==NULL){
        return RC_FILE_NOT_FOUND;
    }
    SM_PageHandle newpage = (SM_PageHandle) calloc(PAGE_SIZE, size(char));
    int writePage = fwrite(newpage, sizeof(char), PAGE_SIZE, fpointer);
    if (writePage==0){
        return RC_WRITE_FAILED;
    }
    free(newpage);
    fclose(fpointer);
    return RC_OK;
}

RC openPageFile(char *filename, SM_FileHandle *fHandle){
    FILE *fpointer;
    fpointer = fopen(*filename,"r+b");
    if (fpointer==NULL){
        return RC_FILE_NOT_FOUND
    }
    
}

RC closePageFile (SM_FileHandle *fHandle){
    if(fclose(*fHandle->mgmtInfo)==0){
        return RC_OK
    }
    return RC_FILE_NOT_FOUND
}

RC destroyPageFile (char *filename){
    if(remove(filename)==0){
        return RC_OK
    }
    return RC_FILE_NOT_FOUND
}
