
#ifndef FACCESS_H

#define FACCESS_H

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

/* FileAccess - a class for opening and reading ASCII files.

   Public functions:

     FileAccess constructor - The argument to this constructor is
                              the file name.  The constructor will
                              open the file and set the variable
                              FileOpenOK (which can be accessed via
			      FileOK).
     FileAccess destructor  - Close the file
     get_line               - get a line for the file and return it as a null
                              terminated string.
                              
 */

#include <stdio.h>
#include "ianstd.h"

class FileAccess {

  // Variables
  private:
    FILE *FilePtr;
    BOOLEAN FileOpenOK;

  protected:

  public:

  // Class Functions
  private:
    int OpenFile(char *FileName);
    void file_error(const char *msg);
  protected:
  public:
    FileAccess(const char *FileName = "" );
    ~FileAccess(void);
    BOOLEAN FileOK(void) { return FileOpenOK; }
    BOOLEAN get_line(char *LineBuf, const int BufSize);
  
}; // FileAccess

#endif
