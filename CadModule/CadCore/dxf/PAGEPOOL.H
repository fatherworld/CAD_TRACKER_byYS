
#ifndef PAGEPOOL_H

#define PAGEPOOL_H

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

/*

 The page_pool object is designed to support rapid memory allocation
 and deallocation of all allocated memory, once the object is no
 longer needed.  By using a block allocation scheme, the overhead of
 memory allocation is greatly reduced, compared to "malloc" (or new).
 Deallocation of large blocks of memory is also much faster than
 calling free for every allocated block of memory.

 The page_pool allocator allocates blocks of memory in "page_size"
 increments.  The page_size variable is set at start-up and contains
 the system virtual memory page size.

 The page_pool object contains the virtual function "add_to_list"
 which can be implemented by the inherited function
 ordered_list.  Ordered list is a class that supports ordered list
 insertion and deletion.  In this case, this is used to support an
 ordered list of free memory blocks.  The ordered list is only
 needed if the page_pool is used as the base for a memory allocater
 that can allocate from a free list.

*/

class page_pool {
private: // typedefs and variables
  typedef struct page_chain_struct 
  { void *block;
    unsigned int bytes_used;
    unsigned int block_size;
    page_chain_struct *next_page;
  } page_chain;

  // The min_block_size is the smallest block that will be returned by the
  // page_pool allocater.  The max_block_size is the largest memory block 
  // that can be requested. Ideally, this should be a multiple of the page 
  // size.
  typedef enum { min_block_size = 96, max_block_size = 1024 * 8 } bogus;
  page_chain *page_list_head;
  page_chain *current_page;
  static unsigned int page_size;  // system page size, shared by all instances

private: // class functions
  static unsigned int GetSysPageSize(void);
  page_chain *new_block( unsigned int block_size );
  void *add_block( unsigned int block_size );

public: // class functions
  page_pool(void);
  ~page_pool(void);
  void *page_alloc( unsigned int block_size );
  void print_page_pool_info(void);
  virtual void add_to_list(void *addr, unsigned int size);
}; // class page_pool


#endif
