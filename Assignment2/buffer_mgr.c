#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "buffer_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"
#include "dt.h"

//initialize the Buffer Pool
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStratetgy strategy,
                  void *stratData){
    
    bm->pageFile = (char*) pageFileName;
    bm->numPages= numPages;
    bm->strategy = strategy;
    
    
    return RC_OK
}

//Shut down the Buffer Pool
RC shutdownBufferPool(BM_BufferPool *const bm){
    return RC_OK
}

//Forse
RC forceFlushPool(BM_bufferPool *const bm){
    return RC_OK
}
