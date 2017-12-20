
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


#include <stdlib.h>
#include "glface.h"

/* A 3D graphic object can be described by a list of polygon faces.  Each
   face is itself a list of 3D points (e.g., x, y, z coordinates).
   This class supports the construction of polygon faces for a 3D object
   that is read from a DXF file.  Two DXF polygon types are supported:

    - 3DFACE
    - POLYLINE

   A 3DFACE polygon is composed of either three points (e.g., its a triangle)
   or four points (its a rectangle).

   A POLYLINE object is composed of one or more polygons (what AutoDesk
   calls a polygon mesh).  These may have a variable number of points,
   although the number of points per polygon may be specified in the
   POLYLINE definition.

*/


// add_point
//
// Add a point to a polygon.  If the point being added is the same
// as the last point (which can occur in 3DFACE polygons), skip it.
//
void FaceList::add_point(face *cur_face,
			 float x, float y, float z)
{
  int PointCnt = cur_face->point_cnt;
  float *points = cur_face->f[PointCnt].v;
  float *LastPts = cur_face->f[ PointCnt-1 ].v;

  // If the point being added is not the same as the last point,
  // then add it to the point list.
  if (cur_face->point_cnt == 0 ||
      (LastPts[Xcoord] != x || LastPts[Ycoord] != y || LastPts[Zcoord] != z)) {
    points[Xcoord] = x;
    points[Ycoord] = y;
    points[Zcoord] = z;
    cur_face->point_cnt++;
  }
} // add_point


//
// add_3DFACE_point
//
// This function adds a 3D point to a 3DFACE polygon.  The number of
// points in a 3DFACE object is defined when the polygon face is allocated
// and is fixed at no more than four.
//
void FaceList::add_3DFACE_point(float x, float y, float z )
{

  if (ListTail != NULL) {
    face *cur_face = &ListTail->cur_face;
    int PointCnt = cur_face->point_cnt;

    if (PointCnt >= cur_face->point_max) {
      printf("FaceList::add_3DFACE_point: Attempted to add too many points\n");
    }
    else {
      add_point(cur_face, x, y, z);
    }
  }
  else
    printf("FaceList::add_3DFACE_point: Can't add point to unalloced list\n");
} // add_3DFACE_point



/* 
  add_poly_point

  A DXF POLYLINE object is composed of a "polygon mesh", which
  contains one or more polygons.  These polygons may have a variable
  number of points.  According to the spec, the "71" marker following
  the POLYLINE marker gives the number of polygons and the "72" marker
  gives the number of points.  But the spec also says that these
  numbers are not required to be correct (got to love it).  Apparently
  the DXF reader is supposed to figure it out.  How this is done, is
  not stated (at least so that I could understand).

  So the code below depends on the "72" marker being correct for the
  "point maximum" of an object.  This value set when the polygon
  face is allocated.

  A POLYLINE object consists of a sequence of VERTEXs.  When "point_max"
  VERTEXs are read, a new POLYLINE face is allocated (get_new_poly_face).

 */
void FaceList::add_poly_point(float x, float y, float z )
{
  if (ListTail != NULL) {
    face *cur_face = &ListTail->cur_face;
    int PointCnt = cur_face->point_cnt;

    // Hit the end of the current polygon, start a new one
    if (cur_face->point_cnt >= cur_face->point_max) {
      get_new_poly_face( cur_face->point_max );
      cur_face = &ListTail->cur_face;
    }

    add_point(cur_face, x, y, z);
    
  }
  else
    printf("FaceList::add_poly_point: Can't add point to unalloced list\n");
} // add_poly_point




//
// get_new_3DFACE
//
// Allocate storage for a new 3DFACE (e.g., a polygon face consisting
// of three or four points).
//
void FaceList::get_new_3DFACE(void)
{
  FaceElem *tmp;
  vect *VectPtr;

  tmp = (FaceElem *)mem.page_alloc( sizeof( FaceElem ) );
  VectPtr = (vect *)mem.page_alloc( sizeof( vect ) * MinSize );
  tmp->next_face = NULL;
  tmp->cur_face.point_cnt = 0;
  tmp->cur_face.f = VectPtr;
  tmp->cur_face.point_max = MinSize;
  if (ListHead == NULL) {
    ListHead = tmp;
    ListTail = tmp;
  }
  else {
    ListTail->next_face = tmp;
    ListTail = tmp;
  }
}



//
// get_new_poly_face
//
// Allocate memory for a polygon face from a POLYLINE object.  The
// number of points is given by the polypoint argument.
//
void FaceList::get_new_poly_face(int polypoint)
{
  FaceElem *tmp;
  vect *VectPtr;

  tmp = (FaceElem *)mem.page_alloc( sizeof( FaceElem ) );
  VectPtr = (vect *)mem.page_alloc( sizeof( vect ) * polypoint );
  tmp->next_face = NULL;
  tmp->cur_face.point_cnt = 0;
  tmp->cur_face.f = VectPtr;
  tmp->cur_face.point_max = polypoint;
  if (ListHead == NULL) {
    ListHead = tmp;
    ListTail = tmp;
  }
  else {
    ListTail->next_face = tmp;
    ListTail = tmp;
  }
}



#ifdef DYNAMIC_ALLOC
/*
  GrowFace

  GrowFace was originally written so that the number of points in a
  POLYLINE object could be dynamically increased.  This is necessary
  because the number of VERTEXs (e.g., 3D points) that follow the
  POLYLINE header varies.  However, the number of points per POLYLINE
  polygon are supposed to be specified in the header.  So, assuming
  that this number is correct, storage for the polygon can be allocated
  once.  This avoids the complicated memory allocation, deallocation and
  reallocation of freed memory that is supported by this function.
  As a result, this function is currently unused.  It is included in this
  object in case it is necessary to support dynamically growable polygon
  faces.

  This function calls mem.mem_alloc, from the memory allocation object.
  If this function is used, the point vector should have been allocated
  with mem_alloc, instead of page_alloc.  This will allow the old
  memory to be deallocated and recovered for later allocation.

*/
void FaceList::GrowFace( face *cur_face )
{
  const int MaxSize = 1024 * 4;
  int new_size;

  if (cur_face->point_cnt < cur_face->point_max)
    printf("FaceList::GrowFace called too soon\n");
  else {
    new_size = cur_face->point_max * 2;
    if (new_size > MaxSize) {
      printf("FaceList::GrowFace: too many points allocated\n");
      exit(1);
    }
    else {
      vect *VectPtr, *tmp;
      int i;

      VectPtr = (vect *)mem.mem_alloc( sizeof(vect) * new_size );
      for (i = 0; i < cur_face->point_cnt; i++) {
	VectPtr[i] = cur_face->f[i];
      }
      tmp = cur_face->f;
      // Free the old memory
      mem.mem_free( tmp );
      cur_face->f = VectPtr;
      cur_face->point_max = new_size;
    }
  }
}  // GrowFace
#endif


//
// print
//
// Print out the face list for a 3D object.  This function is supplied for
// debugging.
//
void FaceList::print(void)
{
  FaceElem *cur_elem = ListHead;

  while (cur_elem != NULL) {
    int i, j;
    face *cur_face = &(cur_elem->cur_face);
    
    printf("{ ");
    for (j = 0; j < cur_face->point_cnt; j++) {
      if (j > 0)
	printf("  ");
      printf("[");
      for (i = Xcoord; i <= Zcoord; i++) {
	printf("%f", cur_face->f[j].v[i] );
	if (i < Zcoord)
	  printf(", ");
      }
      printf("]");
      if (j < cur_face->point_cnt-1)
	printf("\n");
    } // j
    printf("}\n");
    cur_elem = cur_elem->next_face;
  }
}

