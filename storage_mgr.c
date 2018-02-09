

#include <stdio.h>
#include <stdlib.h>
#inslude <string.h>
#include "storage_mgr.h"
#include "dberror.h"

void initStorageManager (void){
    
}

RC createPageFile (char *filename){
    FILE *fpointer;
    fpointer = fopen(filename,"w");
    if (fp==NULL){
        return RC_FILE_NOT_FOUND;
    }
    SM_PageHandle newpage = (SM_PageHandle) calloc(PAGE_SIZE, size(char));
    int writePage = fwrite(newpage, sizeof(char), PAGE_SIZE, fpointer);
    if (writePage==0){
        return RC_WRITE_FAILED
    }
    free(newpage)
    fclose(fpointer);
    return RC_OK;
}

RC openPageFile(char *filename, ){
    
}

RC closePageFile (){
    
}

RC destroyPageFile (char *filename){
    
}
