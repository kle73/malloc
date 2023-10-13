/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 *
 */

void* heap_start;
void* heap_end;

int mm_init(void)
{
	void *p = mem_sbrk(256);
    if (p == (void *)-1) return -1;

	heap_end = mem_heap_hi();
	heap_start = mem_heap_lo();

	*((size_t*)heap_start) = 256 << 1;
	//*((size_t*)heap_end-1) = 256 << 1;

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{		
    int newsize = ALIGN(size + SIZE_T_SIZE);
		void* current = heap_start;
		size_t current_size = *((size_t*)current) >> 1;
		while (1){
			//first check if block is even free:
			if ( *(size_t*)current % 2 == 0){
				if  (current_size < newsize)
				{
					if ( (char*) current + current_size >=  (char*)heap_end){
							void* p = mem_sbrk(newsize - current_size + SIZE_T_SIZE);
							if (p == (void *)-1){
								return NULL;
							}else {
								*(size_t *)current = (size << 1) + 1;
								heap_end = mem_heap_hi();
								//*(size_t*) heap_end = (newsize << 1) + 1;
								//assert((char *)current + newsize <= (char*) heap_end );
								return (void *)((char *)current + SIZE_T_SIZE);
							}
					} else {	
						current = ((char*)current) + current_size;
					}
				}
				else if (current_size >= newsize) 
				{
					void* to_return = (size_t*)current + SIZE_T_SIZE;
					*(unsigned*) current = (newsize << 1) + 1;
					//current = (char*)current + newsize-1;
					//*(unsigned*) current = (newsize << 1) + 1;
					current = (char*)current + newsize; // changed, was SIZE_T_SIZE before
					if (current > heap_end){
						return to_return;
					}
					*(unsigned*) current = ((current_size - newsize) << 1); //changed, was +1 before
					//current = (char*)current + current_size - newsize -1;
					//*(unsigned*) current = ((current_size - newsize) << 1) + 1;	
					return to_return;
				}
			}
			if ((char*) current + current_size >=  (char*)heap_end){
					void* p = mem_sbrk(newsize);
					if (p == (void *)-1){
						return NULL;
					}else {
						*(size_t *)p = (size << 1) + 1;
						heap_end = mem_heap_hi();
						//*(size_t*) heap_end = (newsize << 1) + 1;
						return (void *)((char *)current + SIZE_T_SIZE);
					}
			} else {
				current = ((char*)current) + current_size;
			}
		}
   /*
    */
}

/*
 * mm_free - Freeing a block does nothing.
 */

void mm_free(void *ptr)
{
  if (ptr == heap_end || ptr == NULL){
		return;
	}
	//size_t size = *(size_t*) ptr;
	
	//void* next = (char*)ptr + size-1;
	//*(size_t*) next -= 1;
	//next = (size_t*)next + 1;
	/*
	if (*(size_t*)next % 2 == 0){
		size += *(size_t*)next;
		next = (char*)next + *(size_t*)next -1;
		*(size_t*)next -= 1; 
	}*/
  ptr = (size_t*) ptr - 1;
  *(size_t*) ptr = *(size_t*)ptr -1;

  //if (ptr != heap_start){
		//coaless backwards
	//}
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void* mm_realloc(void *ptr, size_t size)
{
  int newsize = ALIGN(size + SIZE_T_SIZE);
	if (ptr == NULL || ptr == heap_end) return NULL;
	
	ptr = (size_t*) ptr - SIZE_T_SIZE;
	size_t current_size = *(size_t*) ptr >> 1;
  
	size_t cache_size = current_size;

	while (cache_size < newsize){
		void* next = ptr + newsize;
		if (next >= heap_end || next == NULL) break;

		if (*(size_t*)next % 2 == 0){
			
			cache_size += *(size_t*)next;
      
			if (cache_size >= newsize){
				*(size_t*) ptr = (cache_size << 1) + 1;
				return ptr;
		  }
	  } else {
			break;
		}
	}
	//did not work, allocate new memory
	void* oldptr = ptr;
	void* newptr;
	size_t copySize;
	
	newptr = mm_malloc(size);
	if (newptr == NULL)
		return NULL;

	copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
	if (size < copySize)
		copySize = size;
	memcpy(newptr, oldptr, copySize);
	mm_free(oldptr);
	return newptr;
}




int main(){
	
	void* p = mm_malloc(32);
	printf("%p", p);
	return 0;

}









