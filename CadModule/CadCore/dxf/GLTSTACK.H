
#ifndef GLTSTACK_H

#define GLTSTACK_H

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
   This file contains the stack object definition.  The scanner
   requires two token look ahead to resolve things like the EOF token
   sequence.  If this sequence is checked for, but not found, it is
   necessary to "back up" the input stream.  This is done via the token
   stack.

   Note that a DXF file may contain both integer and float values.
   Two versions of "push_token" are implemented, one with an integer
   value and one with a float value.  The integer version may be used
   with just a token value, in which case the default value will be
   used for the integer.
*/

#include "gltoken.h"
#include "ianstd.h"


class token_stack {
  public:
    typedef union { int intval;
	  	    float realval;
		  } token_union;

    typedef struct { gl_token token;
                     token_union val;
		   } token_stack_type;

 private:
    typedef enum {MaxTokens = 10} bogus1;
    token_stack_type token_stack_buf[ MaxTokens ];
    int token_sp;

  public:
    token_stack(void) { token_sp = MaxTokens; }
    BOOLEAN StackHasItems(void) { return token_sp < MaxTokens; }
    void push_token( gl_token token, int intval = 0 );
    void push_token( gl_token token, float realval );
    token_stack_type pop_token(void);
}; 

#endif
