#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "buffer_mgr.h"
#include "buffer_mgr_stat.h"

//initialize the Buffer Pool
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData){
    //creates buffer pool with size of the number of pages in the frame
    Frame *BP = malloc(numPages * sizeof(Frame));
    
    int i=0;
    while (i < numPages){
        BP[i].data = NULL;
        BP[i].frameNum = i; //Set frame number
        BP[i].pageNum = NO_PAGE; //set to no page since initialize
        BP[i].fixCount = 0; //initialize to 0 since no access
        BP[i].dirty = 0; //initialize to false since no change
        BP[i].timeFirstPinned = 0;
        BP[i].timeLastUsed= 0;
        
        //set next and previous pointers
        if (i == 0){
            BP[i].previousFrame = NULL;
        }
        else {
            BP[i].previousFrame = &BP[i-1];
        }
        if (i == numPages - 1){
            BP[i].nextFrame = NULL;
        }
        else{
            BP[i].nextFrame = &BP[i+1];
        }
        i++;
    }
    
    //Buffer Pool information
    bm->firstFrame = &BP[0];
    bm->lastFrame = &BP[numPages-1];
    bm->readNum = 0;
    bm->writeNum = 0;
    
    //Buffer pool struct
    bm->pageFile = (char*)pageFileName;
    bm->numPages= numPages;
    bm->strategy = strategy;
    bm->mgmtData = BP;
    return RC_OK;
}

//Shut down the Buffer Pool
RC shutdownBufferPool(BM_BufferPool *const bm){
    
    int numPages = bm->numPages; //get numebr of pages
    Frame *BP = bm->mgmtData;
    
    forceFlushPool(bm);
    
    //Make sure fix counts is 0, otherwise do not shutdown and return error
    int i = 0;
    while (i < numPages){
        if (BP[i].fixCount != 0){
            return RC_PIN;
        }
        i++;
    }
    
    //free up memory and return
    free(BP);
    bm->mgmtData = NULL;
    return RC_OK;
}

//Force Flush
RC forceFlushPool(BM_BufferPool *const bm){
    
    int numPages = bm->numPages; //get number of pages
    Frame *BP = (Frame *)bm->mgmtData;
    
    int i = 0;
    while (i < numPages){
        if ((BP[i].dirty == 1) && (BP[i].fixCount) == 0){
            //write to file
            SM_FileHandle fileHandle;
            openPageFile(bm->pageFile, &fileHandle);
            writeBlock(BP[i].pageNum, &fileHandle, BP[i].data);
            closePageFile(&fileHandle);
            BP[i].dirty = 0;
            bm->writeNum++;
        }
        i++;
    }
    
    return RC_OK;
}

//Looping helper function
Frame* findPage(BM_BufferPool *const bm, BM_PageHandle *const page){
    //We extract the frame array from our Buffer Pool
    Frame* frames = (Frame*) bm->mgmtData;

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].pageNum == page->pageNum){
            return &frames[i];
        }
    }
}

// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
    //Cycle through the frames until we find the one to mark as dirty
    Frame* frame = findPage(bm, page);
    frame->dirty = 1;

    return RC_OK;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
    Frame* frame = findPage(bm, page);
    frame->fixCount--;

    return RC_OK;
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
    //Use storage_mgr to write the page
    SM_FileHandle filehandle;
    openPageFile(bm->pageFile, &filehandle);
    Frame* frame = findPage(bm, page);
    writeBlock(frame->pageNum, &filehandle, frame->data);
    closePageFile(&filehandle);
    bm->writeNum++;

    return RC_OK;
}

bool isBufferFull(BM_BufferPool *const bm){
    Frame* frames = (Frame*) bm->mgmtData;

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].pageNum == NO_PAGE){ //If a frame has not been assigned to a page, it's free
            return false;
        }
    }
    return true;
}

Frame* findFreeFrame(BM_BufferPool *const bm){
    Frame* frames = (Frame*) bm->mgmtData;

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].pageNum == NO_PAGE){ //If a frame has not been assigned to a page, it's free
            return &frames[i];
        }
    }
}

Frame* checkExistingFrames(BM_BufferPool *const bm, const PageNumber pageNum){
    Frame* frames = (Frame*) bm->mgmtData;

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].pageNum == pageNum){
            return &frames[i];
        }
    }
    return NULL;
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
            const PageNumber pageNum){

    //If frame already exists, return it
    Frame* frame = checkExistingFrames(bm, pageNum);
    if(frame){
        page->pageNum = pageNum;
        frame->fixCount++;
        bm->readNum++;
        frame->timeLastUsed = (long) time(NULL); //Page is used, record time
        page->data = frame->data;
        return RC_OK;
    }

    if(isBufferFull(bm)){
        //Replacement strategy comes into play
        if(bm->strategy == RS_FIFO){
            return FIFO(bm, page, pageNum);
        } else if(bm->strategy == RS_LRU){
            return LRU(bm, page, pageNum);
        }
        bm->readNum++;
    } else {
        //Otherwise, create it
        frame = findFreeFrame(bm);
        frame->data = (SM_PageHandle) malloc(PAGE_SIZE);
        SM_FileHandle filehandle;
        openPageFile(bm->pageFile, &filehandle);
        readBlock(pageNum, &filehandle, frame->data);
        frame->timeFirstPinned = (long) time(NULL); //First time the frame is used
        frame->timeLastUsed = (long) time(NULL); //Page is used, record time
        frame->pageNum = pageNum;
        frame->fixCount++;
        bm->readNum++;
        page->pageNum = pageNum;
        page->data = frame->data;
        return RC_OK;
    }
}

RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum){
    //Replacement logic
    Frame* frames = (Frame *) bm->mgmtData;
    Frame* firstIn = &frames[0];

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].timeFirstPinned < firstIn->timeFirstPinned){
            if((frames[i].dirty == false)&(frames[i].fixCount == 0)){
                firstIn = &frames[i];
            }
        }
    }

    SM_FileHandle filehandle;
    firstIn->data = (SM_PageHandle) malloc(PAGE_SIZE);
    openPageFile(bm->pageFile, &filehandle);
    readBlock(pageNum, &filehandle, firstIn->data);
    firstIn->timeFirstPinned = (long) time(NULL); //First time the frame is used
    firstIn->timeLastUsed = (long) time(NULL); //Page is used, record time
    firstIn->pageNum = pageNum;
    firstIn->fixCount++;
    page->pageNum = pageNum;
    page->data = firstIn->data;

    return RC_OK;
}

RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page, PageNumber pageNum){
    //Replacement logic
    Frame* frames = (Frame *) bm->mgmtData;
    Frame* leastRecUsed = &frames[0];

    for(int i=0;i<bm->numPages;++i){
        if(frames[i].timeLastUsed < leastRecUsed->timeLastUsed){
            if((frames[i].dirty == false)&(frames[i].fixCount == 0)){
                leastRecUsed = &frames[i];
            }
        }
    }

    SM_FileHandle filehandle;
    leastRecUsed->data = (SM_PageHandle) malloc(PAGE_SIZE);
    openPageFile(bm->pageFile, &filehandle);
    readBlock(pageNum, &filehandle, leastRecUsed->data);
    leastRecUsed->timeFirstPinned = (long) time(NULL); //First time the frame is used
    leastRecUsed->timeLastUsed = (long) time(NULL); //Page is used, record time
    leastRecUsed->pageNum = pageNum;
    leastRecUsed->fixCount++;
    page->pageNum = pageNum;
    page->data = leastRecUsed->data;

    return RC_OK;
}
