
#include <stdio.h>
#include "gltstack.h"

//
// push_token : integer version
//
void token_stack::push_token( gl_token token, int intval )
{
  if (token_sp > 0) {
    token_sp--;
    token_stack_buf[ token_sp ].token = token;
    token_stack_buf[ token_sp ].val.intval = intval;
  }
  else
    printf("push_token: stack overflow\n");
}


//
// push_token : float version
//
void token_stack::push_token( gl_token token, float realval )
{
  if (token_sp > 0) {
    token_sp--;
    token_stack_buf[ token_sp ].token = token;
    token_stack_buf[ token_sp ].val.realval = realval;
  }
  else
    printf("push_token: stack overflow\n");
}


//
// pop_token
//
token_stack::token_stack_type token_stack::pop_token(void)
{
  token_stack_type t;
  
  t.token = bad_gl_token;
  t.val.intval = 0;
  if (token_sp < MaxTokens)
    return token_stack_buf[ token_sp++ ];
  else {
    printf("pop_token: stack underflow\n");
	return t;
  }
}
