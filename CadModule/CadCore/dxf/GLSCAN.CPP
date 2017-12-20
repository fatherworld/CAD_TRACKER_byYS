
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

//
// This file contains the GL_scanner class functions for the DXF file scanner.
// These functions are:
//
//   print_token     print the associated text string for a token
//   gl_name_lookup  check if the word is a DXF reserved word
//   scan_word       pick out words from the input line and catagorized them
//   can_numer       scan words starting with numeric characters, integers
//                   and reals.
//   get_token_from_stack  pop a token and possibly its value from the token
//                         stack
//   put_token_in_stack    push the current token and possibly its value
//                         into the token stack.
//   get_next_token  The main scanner routine, called to return the
//                   next token from the input stream.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ianstd.h"
#include "gltstack.h"
#include "glscan.h"

//
// There is a "hole" in C++.  You are not allowed to
// initialize a static variable inside of a class definition,
// even though at static class member is truely static -
// that is, there is only one copy, for all instances of
// that class.  So the variable is included here.
//
typedef struct { char *GLname;
  gl_token token;
} table_rec;

static table_rec res_word_table[] = {  {"bad_gl_token", bad_gl_token},
				       {"SECTION", section},
				       {"HEADER", header },
				       {"ENDSEC", endsec },
				       {"SEQEND", seqend },
				       {"BLOCKS", blocks },
				       {"BLOCK",  block  },
				       {"ENDBLK", endblk },
				       {"VERTEX", vertex },
				       {"TABLES", tables },
				       {"TABLE", table },
				       {"LAYER", layer },
				       {"CONTINUOUS", contin },
				       {"ENDTAB", endtab },
				       {"ENTITIES", entities},
				       {"3DFACE", three_d_face},
				       {"EOF", end_of_file },
				       {"POLYLINE", polyline },
				       {"integer", integer},
				       {"real", real},
				       {"name", name},
				       { NULL, bad_gl_token }};



//
// print_token
//
// Print the token name, as a character string.
//
void GL_scanner::print_token(const gl_token token)
{
  char *str = "unknown";

  if (token < last_gl_token)
    str = res_word_table[token].GLname;
  printf("%s", str );
} // print_token



//
// gl_name_lookup
//
// gl_name_lookup looks up a word in the reserved word table.  If the
// word is found in the table, the associated enumeration value is
// returned.  Otherwise the function returns "name", meaning that
// the word is an identifier.
//
gl_token GL_scanner::gl_name_lookup(void)
{

  int ix = 0;
  BOOLEAN found = FALSE;
  gl_token return_token;

  return_token = name;
  while (res_word_table[ix].GLname != NULL && !found) {
    if (strcmp(WordBuf, res_word_table[ix].GLname) == 0) {
      return_token = res_word_table[ix].token;
      found = TRUE;
    }
    else
      ix++;
  }
  return return_token;
} // gl_name_lookup




//
// scan_word
//
// Find a word (e.g., a set of alpha-numeric characters bounded by spaces).
// The function is only called if the first character is an alphabetic char.
// The word will either be a reserved word (e.g., header or layer) or
// an identifier.  This function returns either "name" for an identifier
// or the associated enumeration value for a reserved word.
//  
gl_token GL_scanner::scan_word(char *cptr)
{
  char *WordBufPtr = WordBuf;
  gl_token return_token;

  while (isalnum(*cptr))
    *WordBufPtr++ = *cptr++;
  *WordBufPtr = '\0';
  return_token = gl_name_lookup();

  return return_token;
} // scan_word



//
// scan_number
//
// DXF files may contain reserved words (or perhaps identifiers) that
// start with integer values.  For example, the reserved word 3DFACE.
// This function scans the word and returns either "name" or the
// associated enumeration for an identifier that starts with a number.
// The function returns integer or real for numeric values and bad_gl_token
// if there is an error.
//
// The token is either a name or reserved word begining with a number
// (e.g., like the reserved word 3DFACE) or its an integer or a float.
// The grammar is:
//
//    {digit}alpha ==> name or reserved word
//    {digit}(space|null) ==> integer
//    [+|-]{digit}.{digit} ==> float
//
gl_token GL_scanner::scan_number(char *cptr)
{
  char *WordBufPtr = WordBuf;
  gl_token return_token = bad_gl_token;

  if (*cptr == '+')
    cptr++;
  if (*cptr == '-') {
    *WordBufPtr++ = *cptr++;
  }
  while (isdigit(*cptr))
    *WordBufPtr++ = *cptr++;
  if (isalpha(*cptr)) {
    // its a reserved word or a name
    while (isalnum(*cptr))
      *WordBufPtr++ = *cptr++;
    *WordBufPtr = '\0';    
    return_token = gl_name_lookup();
  }
  else {
    if (*cptr == '.') {
      return_token = real;
      *WordBufPtr++ = *cptr++;  // put the '.' into the string
      while (isdigit(*cptr))
	*WordBufPtr++ = *cptr++;
      *WordBufPtr = '\0';
      sscanf(WordBuf, "%f", &gl_float);
    }
    else { // its and integer
      return_token = integer;
      *WordBufPtr = '\0';
      sscanf(WordBuf, "%d", &gl_int);
    }
  }
  return return_token;
} // scan_number


//
// get_token_from_stack
//
// The scanner uses two token look ahead when checking for things like
// the EOF token sequence.  If EOF is not found then the tokens are
// pushed back into the token stack.  This function pops a token and
// possibly an integer or real value from the token stack and puts the
// values in the scanner object.  This allows the rest of the scanner
// to behave as if the token were fetched from the input stream.
//
gl_token GL_scanner::get_token_from_stack()
{
  gl_token token;

  token = bad_gl_token;
  if (token_in_stack()) {
    token_stack::token_stack_type t;

    t = stack.pop_token();
    token = t.token;
    if (token == integer)
      gl_int = t.val.intval;
    else
      if (token == real)
	gl_float = t.val.realval;
  }
  else
    printf("Error: get_token_from_stack called on empty stack\n");
  return token;
}


//
// put_token_in_stack
//
// This function pushs the current token into the token stack.  This is
// needed to support scanner look ahead and back the token stream up
// (just as ungetc() does in UNIX).  The push_token function is
// overloaded to support either a token, a token and an integer or a
// token and a real.
//
void GL_scanner::put_token_in_stack(const gl_token token)
{
  
  if (token == integer)
    stack.push_token(token, gl_int);
  else
    if (token == real)
      stack.push_token(token, gl_float);
    else
      stack.push_token(token);
}



//
// get_next_token
//
// Scan the DXF file for tokens.  This function assume that there is one 
// DXF token per line.  This is the main scanner routine which is called
// to return the next token.
//
// The functions that call the scanner need a way to check for the
// end of file token sequence and, if the token sequence is not the end 
// of file sequence to push the tokens back into the input stream, 
// so that they will be gotten the next time get_next_token is called.  
// This is supported by the token stack object.
//
gl_token GL_scanner::get_next_token(void)
{
  const int LineSize = 80;
  char line[LineSize];
  gl_token return_token;

  return_token = bad_gl_token;
  gl_int = -1;
  if (token_in_stack()) {
    return_token = get_token_from_stack();
  }
  else {
    if (MyFileObj->get_line( line, sizeof(line) )) {
      char *cptr = line;
	  //static int aaa = 0;
	 // printf("aaa = %d  \n", aaa++);
      while (isspace(*cptr))
	cptr++;
      // if its an alphabetic character, then it is either a DXF "reserved
      // word" (e.g., a DXF token) or it a name.
      if (isalpha(*cptr))
	return_token = scan_word(cptr);
      else {
	// Its either a token or name starting with a number or its a
	// floating point or an integer.
	return_token = scan_number(cptr);
      }
    }
  }

  return return_token;
}
