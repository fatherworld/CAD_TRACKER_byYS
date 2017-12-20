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

/* File access routines.  

   FileAccess constructor - open the file
   FileAccess destructor  - close the file
   get_line - return a line from the file as a null terminated string
*/

#include <stdio.h>
#include <string.h>
#include "ianstd.h"
#include "faccess.h"


FileAccess::FileAccess(const char *FileName )
{
  FilePtr = fopen(FileName, "r");  // open file for reading
  FileOpenOK = FALSE;
  if (FilePtr != NULL)
    FileOpenOK = TRUE;
  else
    perror(FileName);    // print system error message
} // FileAccess constructor



FileAccess::~FileAccess(void)
{
  if (FileOpenOK)
    fclose(FilePtr);
} // FileAccess destructor


void FileAccess::file_error(const char *msg )
{
  printf("%s\n", msg );
}



//
// Get a line from the file pointed to by the class variable FilePtr.
// When fgets reads the line, it includes the cariage return character.
// This function overwrites this with a null (like the gets() function).
//
BOOLEAN FileAccess::get_line(char *LineBuf, const int BufSize)
{
  BOOLEAN result;

  result = FALSE;
  LineBuf[0] = '\0';
  if (fgets(LineBuf, BufSize, FilePtr)) {
    int len = strlen(LineBuf);

    if (LineBuf[len-1] = '\n')
      LineBuf[len-1] = '\0';
    result = TRUE;
  }
  else {
    if (feof(FilePtr))
      file_error("Unexpected end of file");
    else
      perror("File error:");
  }
  return result;
}
