//
//  storage_mgr.c
//  
//
//  Created by Parker Joncus on 2/9/18.
//

#include "storage_mgr.h"

void

RC createPageFile (char *filename){
    FILE *fpointer;
    fpointer = fopen(filename,"w");
    
    fclose(fpointer);
    return RC_OK
}

RC openPageFile(char *filename, ){
    
}

RC closePageFile (){
    
}

RC destroyPageFile (char *filename){
    
}
