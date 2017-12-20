
#ifndef GLFACE_H

#define GLFACE_H

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


#include <stdio.h>
#include "pagepool.h"

/*
  A 3D graphic object can be described as a list of polygons.  Each
  of these polygons consists of three or more points in 3D space
  (e.g., x, y, z coordinates).

  The FaceList object will construct a list of polygon faces read from
  a DXF file.  Two DXF file polygon formats are supported:
 
    - 3DFACE
    - POLYLINE

  The "face" structure member "f" is a pointer to an array of points.
  This point array is dynamically allocated.  The minimum number of
  points that can be allocated for a polygon is four (a 3DFACE polygon
  may only consist of three points, in which case the last point is
  unused).

  Although during creating there is a distinction made between DXF 
  3DFACE objects and POLYLINE objects, the class represents both as
  polygon faces and no such distinction exists when the polygon
  list is read.

  The object supplies the following public functions for creating a
  polygon list:

    add_3DFACE_point  - Add a 3D point to a 3DFACE polygon 
    add_poly_point    - Add a 3D point to a POLYLINE polygon

    get_new_3DFACE    - Create a new 3DFACE polygon
    get_new_poly_face - Create a new POLYLINE polygon 

  Once the polygon list is created, it can be read using the following
  functions:

    GetListStart      - Return a handle for the start of the polygon face
                        list
    GetFace           - Get a polygon face, given a list handle
    GetNextFace       - Get the next polygon face in the list

*/


class FaceList {
  public:
    typedef enum {MinSize = 4} bogus;  // minimum number of points
    typedef enum {Xcoord, Ycoord, Zcoord} bogus2;

    typedef struct { float v[3]; } vect;   // a point in the 3D plane

    typedef struct { vect *f;          // an array of 3D points
		     int point_cnt;    // number of points currently alloc'ed
		     int point_max;    // max points
			 int isFlag[3];
			 vect normal;
		   } face;

	

    typedef void *ListHandle;

  private:
    typedef struct FaceListStruct { face cur_face;
				    struct FaceListStruct *next_face;
				  } FaceElem;
    page_pool mem;
    FaceElem *ListHead;
    FaceElem *ListTail;

  private:
    void add_point(face *cur_face, float x, float y, float z);
    // void GrowFace( face *cur_face );

  public:
    FaceList(void) { ListHead = NULL; 
		     ListTail = NULL; 
		   }
    void add_3DFACE_point(float x, float y, float z );
    void add_poly_point(float x, float y, float z);
    void get_new_3DFACE(void);
    void get_new_poly_face(int polypont);
    ListHandle GetListStart(void) { return (ListHandle)ListHead; }
    face *GetFace( ListHandle handle)
      { return &((FaceElem *)handle)->cur_face; }
    ListHandle GetNextFace( ListHandle handle ) 
      { return (ListHandle)((FaceElem *)handle)->next_face; }
    void print(void);
};

#endif
