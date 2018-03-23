#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include "dt.h"

#include <stdio.h>
#include <stdlib.h>

// local functions
static void printStrat (BM_BufferPool *const bm);

// external functions
void
printPoolContent (BM_BufferPool *const bm)
{
	PageNumber *frameContent;
	bool *dirty;
	int *fixCount;
	int i;

	frameContent = getFrameContents(bm);
	dirty = getDirtyFlags(bm);
	fixCount = getFixCounts(bm);

	printf("{");
	printStrat(bm);
	printf(" %i}: ", bm->numPages);

	for (i = 0; i < bm->numPages; i++)
		printf("%s[%i%s%i]", ((i == 0) ? "" : ",") , frameContent[i], (dirty[i] ? "x": " "), fixCount[i]);
	printf("\n");
}

char *
sprintPoolContent (BM_BufferPool *const bm)
{
	PageNumber *frameContent;
	bool *dirty;
	int *fixCount;
	int i;
	char *message;
	int pos = 0;

	message = (char *) malloc(256 + (22 * bm->numPages));
	frameContent = getFrameContents(bm);
	dirty = getDirtyFlags(bm);
	fixCount = getFixCounts(bm);

	for (i = 0; i < bm->numPages; i++)
		pos += sprintf(message + pos, "%s[%i%s%i]", ((i == 0) ? "" : ",") , frameContent[i], (dirty[i] ? "x": " "), fixCount[i]);

	return message;
}


void
printPageContent (BM_PageHandle *const page)
{
	int i;

	printf("[Page %i]\n", page->pageNum);

	for (i = 1; i <= PAGE_SIZE; i++)
		printf("%02X%s%s", page->data[i], (i % 8) ? "" : " ", (i % 64) ? "" : "\n");
}

char *
sprintPageContent (BM_PageHandle *const page)
{
	int i;
	char *message;
	int pos = 0;

	message = (char *) malloc(30 + (2 * PAGE_SIZE) + (PAGE_SIZE % 64) + (PAGE_SIZE % 8));
	pos += sprintf(message + pos, "[Page %i]\n", page->pageNum);

	for (i = 1; i <= PAGE_SIZE; i++)
		pos += sprintf(message + pos, "%02X%s%s", page->data[i], (i % 8) ? "" : " ", (i % 64) ? "" : "\n");

	return message;
}

void
printStrat (BM_BufferPool *const bm)
{
	switch (bm->strategy)
	{
	case RS_FIFO:
		printf("FIFO");
		break;
	case RS_LRU:
		printf("LRU");
		break;
	case RS_CLOCK:
		printf("CLOCK");
		break;
	case RS_LFU:
		printf("LFU");
		break;
	case RS_LRU_K:
		printf("LRU-K");
		break;
	default:
		printf("%i", bm->strategy);
		break;
	}
}

/*The getFrameContents function returns an array of PageNumbers
(of size numPages) where the ith element is the number of the page stored in the
ith page frame. An empty page frame is represented using the constant NO_PAGE.*/

PageNumber *getFrameContents (BM_BufferPool *const bm)
{
  int i;
  //PageNumber is the position of the page in the page file
  PageNumber *array = (PageNumber*) malloc(bm->numPages * sizeof(PageNumber)); //create allocated space for the dynamic array_size
  //mgmtData is used here to hold the position of the PageNumbers
  BM_PageHandle *bufferPoolptr = bm->mgmtData;

  for (i=0; i < bm->numPages; i++)
  {
    if ((i + bufferPoolptr)->data == NULL)
    {
      array[i] = NO_PAGE;

    } else {
      array[i] = (i + bufferPoolptr)->pageNum;
    }
  }
} // end getFrameContents

/*The getDirtyFlags function returns an array of bools (?of size numPages?) where
 the ith element is TRUE if the page stored in the ith page frame is dirty.
 Empty page frames are considered as clean.*/
bool *getDirtyFlags (BM_BufferPool *const bm)
{
  bool *array =  (bool*)malloc(bm->numPages * sizeof(PageNumber));
    Frame *bufferPoolptr = (Frame *)bm->mgmtData;
  //BM_PageHandle *bufferPoolptr = bm->mgmtData;

  int i;
  for (i=0; i<bm->numPages; i++)
  {
    array[i] = (i + bufferPoolptr)->dirty;
  }

  return array;
} //end getDirtyFlags

/*The getFixCounts function returns an array of ints (of size numPages) where
 the ith element is the fix count of the page stored in the ith page frame.
 Return 0 for empty page frames.*/
int *getFixCounts (BM_BufferPool *const bm){
  int *array = (int *)malloc(bm->numPages * sizeof(int));
    Frame *bufferPoolptr = (Frame *)bm->mgmtData;
  //BM_PageHandle *bufferPoolptr = bm->mgmtData;

  int i;

  for (i=0; i<bm->numPages; i++)
  {
    array[i] = (i + bufferPoolptr)->fixCount;
  }

  return array;

} //end getFixCounts

/*The getNumReadIO function returns the number of pages that have been read from
 disk since a buffer pool has been initialized. You code is responsible to
 initializing this statistic at pool creating time and update whenever a page
 is read from the page file into a page frame.*/
int getNumReadIO (BM_BufferPool *const bm)
{
  return bm->readNum;
} //end getNumReadIO

/*getNumWriteIO returns the number of pages written to the page file since the
buffer pool has been initialized.*/
int getNumWriteIO (BM_BufferPool *const bm)
{
  return bm->writeNum;

} //end getNumWriteIO
