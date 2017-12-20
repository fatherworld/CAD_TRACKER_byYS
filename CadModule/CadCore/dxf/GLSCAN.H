
#ifndef GLSCAN_H

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
  This file contains the scanner object that tokenizes the input
  stream.  The scanner either returns a reserved word (e.g., section
  or three_d_face), "name" (an identifier, such as a layer name or
  face name), integer or real.  The various DXF markers (e.g., the 70
  marker or 0 for start) are returned as integers.  This scanner is
  designed as a front end for a program that reads a DXF file and
  displays it as a 3D wire frame, using openGL (hense then "GL"
  naming).

  Scanning and parsing a DXF file is difficult, because the DXF file
  format seems to have grown over time in an ad hoc manner.  Further,
  the documentation is obscure and/or incomplete.  While parsing the
  file, it is sometimes necessary to "back up".  To support this, a
  stack is maintained.  Input tokens and values can be "pushed" onto
  this stack and re-read.  The token_stack class supports this feature.

*/

#define GLSCAN_H

#include "faccess.h"
#include "gltoken.h"
#include "gltstack.h"
#include "ianstd.h"


class GL_scanner {
  // variables
  private:
    token_stack stack;
    float gl_float;
    int gl_int;
    FileAccess *MyFileObj;

    typedef enum {WordBufSize = 40} bogus2;
    char WordBuf[ WordBufSize ];
  protected:
  public:

  // class functions
  private:
    gl_token scan_number(char *cptr);
    gl_token scan_word(char *cptr);
    gl_token gl_name_lookup(void);
  protected:
  public:
    GL_scanner( FileAccess *FileObj ) { MyFileObj = FileObj; }
    float get_gl_float() { return gl_float; }
    int get_gl_int() { return gl_int; }
    char *get_gl_name() { return WordBuf; }
    gl_token get_next_token(void);
    BOOLEAN token_in_stack(void) { return stack.StackHasItems(); }
    gl_token get_token_from_stack();
    void put_token_in_stack(const gl_token token);
    void put_token_in_stack(const gl_token token, int DX_val)
      { stack.push_token(token, DX_val); }
    void print_token(const gl_token token);
};

#endif
