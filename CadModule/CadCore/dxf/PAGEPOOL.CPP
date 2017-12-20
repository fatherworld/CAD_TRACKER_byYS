
/*===============================<o>=====================================

  This software was written by Ian L. Kaplan, Chief Fat Bear, Bear Products
  International.  Use of this software, for any purpose, is granted on two
  conditions: 

   1. This copyright notice must be included with the software
      or any software derived from it.

   2. The risk of using this software is accepted by the user.  No
      warranty as to its usefulness or functionality is provided
      or implied.  The author and Bear Products International provides
      no support.

 ===============================<o>=====================================*/

#include <windows.h>
#include <stdio.h>
#include "pagepool.h"

//
// Get the system page size by calling the Windows NT function GetSystemInfo.
// This function is system dependant and must be rewritten for a non-Win32
// platform, like UNIX (live free or die).
//
// This function is used to initialize the static class variable page_size.
//
unsigned int page_pool::GetSysPageSize( void )
{
  SYSTEM_INFO sys_info;  // system info structure, defined in windows.h

  GetSystemInfo( &sys_info );
  return sys_info.dwPageSize;
} // page_pool::GetSysPageSize

unsigned int page_pool::page_size = page_pool::GetSysPageSize();


//
// page_pool object constructor.  Set up the page list pointers and allocate
// an initial page of memory.
//
page_pool::page_pool(void)
{
  page_chain *new_link;

   new_link = new_block( page_size );
   page_list_head = new_link;
   current_page = new_link;
} // page_pool constructor


//
// page_pool destructor
//
// This function moves through the link list of memory pages and
// deallocates them.
//
page_pool::~page_pool(void)
{
  page_chain *tmp;
  while (page_list_head != NULL) {
    free( page_list_head->block );
    tmp = page_list_head;
    page_list_head = page_list_head->next_page;
    free( tmp );
  }
}


//
// virtual function add_to_list
//
// User defined ordered list function.  This function is optional.
// If the memory allocater built on top of the page_pool object supports
// allocation, deallocation and reallocation, a free list can be added 
// to reduce fragementation in the memory allocated by the page_pool
// allocator.
//
void page_pool::add_to_list(void *addr, unsigned int size)
{
  // Don't do anything...
} // add_to_free_list



//
// new_block
//
// The new_block function is the "root" memory allocator for the
// page_pool object.  The amount of memory allocated is rounded up
// to the next "page_size" boundary.
//
page_pool::page_chain *page_pool::new_block( unsigned int block_size )
{
  page_chain *new_link = NULL;
  int alloc_amt;

  // round to the nearest page size
  alloc_amt = ((block_size + (page_size-1)) / page_size) * page_size;
  if (alloc_amt <= max_block_size) {
    new_link = (page_chain *)malloc( sizeof( page_chain ) );
    if (new_link != NULL) {
      new_link->block = (void *)malloc( alloc_amt );
      new_link->bytes_used = 0;
      new_link->block_size = alloc_amt;
      new_link->next_page = NULL;
    }
    else {
      printf("page_pool::page_chain: malloc memory allocation error\n");
    }
  }
  else {
    printf("page_pool::new_block: allocation request too large\n");
  }
  return new_link;
} // page_chain::new_block



//
// add_block
//
// This function is called when the amount of memory requested by page_alloc
// will not fit in the current block.  If the amount of free memory in
// the current block is larger than a minimum allocation value
// (min_block_size) this free memory is place in the free list
// by calling add_to_list, which is a virtual function.
//
void *page_pool::add_block( unsigned int block_size )
{
  page_chain *new_page = NULL;
  page_chain *last_page;
  int bytes_free;

  last_page = current_page;
  new_page = new_block( block_size );
  current_page->next_page = new_page;
  current_page = new_page;
  bytes_free = last_page->block_size - last_page->bytes_used;
  // if there is unused memory in the block and it is larger than the minimum
  // allocation block, put it in the free list, to avoid fragmentation.
  if (bytes_free >= min_block_size) {
    void *addr;

    addr = (void *)((unsigned int)last_page->block + last_page->bytes_used);
    last_page->bytes_used += bytes_free;
    add_to_list(addr, bytes_free);  // Add to the ordered free list
  }
  return (void *)new_page;
} // page_chain::add_block



//
// page_alloc
//
// This function is called to allocate memory from the page_pool object
// memory pool.  If there is enough free memory in the current block to
// satisify the memory request, memory is allocated from the current
// block and the amount of free memory is updated.  If the current block
// does not have enough memory, add_block is called to allocate a new memory
// block which will be large enough.
// 
void *page_pool::page_alloc( unsigned int num_bytes )
{
  void *addr = NULL;
  unsigned int amt_free = current_page->block_size - current_page->bytes_used;
  
  if (num_bytes > amt_free) {
    if (add_block( num_bytes ) != NULL) {
      amt_free = current_page->block_size;
    }
  }
  if (amt_free >= num_bytes) {

    addr = (void *)((unsigned int)current_page->block + current_page->bytes_used);
    current_page->bytes_used += num_bytes;
  }
  else {
    printf("page_pool::page_alloc: allocation error\n");
    exit(1);
  }
  return addr;
} // page_pool::page_alloc



//
// print_page_pool_info
//
// Print information about the page pool
//
void page_pool::print_page_pool_info(void)
{
  int total_allocated = 0;
  int total_unused = 0;
  page_chain *ptr = page_list_head;

  printf("[block size, bytes_used]\n");
  while (ptr != NULL) {
    printf("[%4d, %4d]", ptr->block_size, ptr->bytes_used);
    total_allocated += ptr->bytes_used;
    total_unused += (ptr->block_size - ptr->bytes_used);
    if (ptr->next_page != NULL) {
      printf(", ");
    }
    else {
      printf("\n");
    }
    ptr = ptr->next_page;
  } // while
  printf("Total allocated = %5d, total unused = %3d\n", total_allocated,
	 total_unused );
}
