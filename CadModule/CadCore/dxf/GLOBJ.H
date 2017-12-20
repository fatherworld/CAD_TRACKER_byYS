
#ifndef GLOBJ_H

#define GLOBJ_H

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


/* This file contains the parser object that processes the DXF file.
   The class function GL_GetFaceList returns the Face list for the
   3D DXF object.
 */

#include "faccess.h"
#include "glscan.h"
#include "glface.h"

class MinMax {
 private:
   float min;
   float max;
 public:
   typedef enum {MAXVAL = 1024*1000} bogus;
   // The initial values of max and min should be way beyond any reasonable
   // range.
   MinMax(void) { min = (float)MAXVAL;  max = (float)(-(MAXVAL)); }
   void SetMinMax( float val ) { if (val < min)
				   min = val;
				 if (val > max)
				   max = val;
			       }
   float GetMin(void) { return min; }
   float GetMax(void) { return max; }
};


class GL_object {
 // variables
 private:
   GL_scanner *GL_scan;
 protected:
 public:
   MinMax PointRange;

 // class functions
 private:
   void set_min_max();
   void undo(gl_token token);
   BOOLEAN IsEndSec(void);
   BOOLEAN IsStart(void);
   BOOLEAN IsName(void);
   BOOLEAN IsTable(void);
   BOOLEAN IsLayerName(void);
   BOOLEAN Is3DFace(void);
   BOOLEAN IsEndSet(void);
   float   GetCoord(void);
   void get_3DFACE_point(FaceList *FaceObjPtr);
   void get_poly_point(FaceList *FaceObjPtr);
   void SkipSection(void);
   void Get3DFACE(FaceList *&CurObj);
   void GetPolyLine(FaceList *&CurObj);
 protected:
 public:
   GL_object(FileAccess *FileObj ) { GL_scan = new GL_scanner( FileObj ); };
   ~GL_object(void) { delete GL_scan; }
   FaceList *GL_GetFaceList(void);
}; // GL_object

#endif
