
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
#include "globj.h"
#include "glscan.h"
#include "gltoken.h"
#include "dxfvals.h"


static void print_token(gl_token token, GL_scanner &GL_scan);


/* The GL_object class supplies the class functions to read a DXF file
   consisting of 3DFACE and POLYLINE "entities".  These "entities"
   describe the polygon faces that make up a 3D object.  The GL_object
   class was written to support the display of DXF files using openGL
   under windows NT.

   DXF files are text files that describe 3D models built with
   AutoCAD, a program from AutoDesk.  Since AutoCAD was one of the
   first modelers that ran on PC, the DXF file format that it used
   became a defacto standard among the programs that later compeated
   with AutoDesk.  Also, many models are provided in DXF format.  

   The DXF file format has all the hallmarks of something designed by
   a bunch of Fortran programmers.  The format is complicated and only
   partially documented.  A DXF file consists of a number of sections,
   not all of which may be used by a given application.  For example,
   many DXF files contain color information, but GL_object does not
   use it.  Since a given application is likely to ignore at least
   some of the information that may be present in a DXF file, a DXF
   reader must be written so that it skips over sections that it does
   not need.  This design is followed by GL_object too.

   The structure of a DXF file is defined by text strings (which
   this program turns into enumeration "tokens") and numeric values.
   For example, all sections start with a zero (a DXF_start, below) and
   the token "SECTION".  There may also be names.  For example, each
   object or object section has a name (e.g., "Cube3" or "ENGINE").
   GL_object reads the file and breaks it up into "tokens", which are
   special words in the DXF file, and special integers (for example,
   DXF_name).  Points are described by single precision "float" values.

   A copy of the enumeration describing the "special integer values"
   is included for reference below (this is from dxfvals.h).

typedef enum { DXF_start         = 0, 
	       DXF_textval       = 1, 
	       DXF_name          = 2, 
	       DXF_othername1    = 3, 
	       DXF_othername2    = 4,
	       DXF_entity_handle = 5,
	       DXF_line_type     = 6,
	       DXF_text_style    = 7,
	       DXF_layer_name    = 8,
	       DXF_var_name      = 9,
	       DXF_primary_X     = 10,
	       DXF_other_X_1     = 11,
               DXF_primary_Y     = 20,
	       DXF_other_Y_1     = 21,
	       DXF_primary_Z     = 30,
	       DXF_other_Z       = 31,
	       DXF_elevation     = 38,
	       DXF_thickness     = 39,
	       DXF_floatvals     = 40,
	       DXF_repeat        = 49,
	       DXF_angle1        = 50,  DXF_angle1        = 51,
	       DXF_angle1        = 52,  DXF_angle1        = 53,
	       DXF_angle1        = 54,  DXF_angle1        = 55,
	       DXF_angle1        = 56,  DXF_angle1        = 57,
	       DXF_angle1        = 58,
	       DXF_colornum      = 62,
	       DXF_entities_flg  = 66,
	       DXF_ent_ident     = 67,
	       DXF_view_state    = 69,
               DXF_SeventyFlag   = 70,  // I guess I have a horror of literal
	       DXF_SeventyOneFlag= 71,  // constants.
	       DXF_SeventyTwoFlag= 72,
               DXF_comment       = 999 } DXF_values;

*/



// 
// GL_GetFaceList is called to process the DXF file and return a pointer 
// to a FaceList object.
//
// The DXF file format was not designed so that it could be easily
// described as a grammer and/or a finite state machine.  In fact, one
// almost suspects that the format is kept purposely obscure, in an
// attemp to stifle any competition for AutoDesk.  An attempt is made
// below to describe the DXF file format in terms of a grammer.  Truth
// to tell, it seems rather dicey.  John Walker and his colleagues may
// have been geniuses, but they also had clay feet.  Below is an attempt
// at describing the file format in grammerical form.  As anyone who has
// read the DXF file format description can see, this is only a fraction
// of the sections that can be in a DXF file.  Since some DXF file
// information is application dependent and is not needed by all
// applications, unneeded sections are simply skipped.
//
//   Notes: Words that are in capitols are tokens (see tokens.h),
//          that is, values returned by the scanner.
//          Words that start with "DXF_" are DXF integer values
//          with special meanings.  Curley braces ("{" and "}")
//          mean "one or more occurances".
//
// DXF_Section = DXF_start SECTION DXF_name SecName SecData SecEnd
// SecName = HEADER | TABLES | ENTITIES
// SecData = DXF_start SecStuff 
// SecEnd = DXF_start ENDSEC
// SecStuff = TableSec | EntitiesSec
// TableSec = TABLE TableGroup DXF_start ENDTAB
// TableGroup = LayerGroup | OtherGroup
// OtherGroup = DXF_name GroupName GroupData
// LayerGroup = DXF_name LAYER DXF_SeventyFlag MaxItems {LayerDesc}
// LayerDesc = DXF_start LAYER DXF_name LayerName 
//             DXF_SeventyFlag FlagVals DXF_colornum
//             ColorNum DXF_line_type LineType
// LineType = CONTINUOUS
// EntitiesSec = {DXF_start EntType DXF_layer_name EntName EntData}
// EntTyp = THREE_D_FACE 
// EntData = ThreeD_data
// ThreeD_data = {Face}
// Face = Set1 Set2 Set3 Set4 EndSet
// EndSet = 62 0 
// 
//
FaceList *GL_object::GL_GetFaceList(void)
{
  FaceList *CurObj;
  gl_token token;
  
  CurObj = NULL;
  token = GL_scan->get_next_token();

  while (token != end_of_file) {
    switch (token) {
      
    case header:
      // Skip the header section
      SkipSection();
      break;
      
    case tables:
      // This program skips the table section.  Some of the
      // grammar for tables is outlined in the comment above.
      while (IsTable()) {
	if (IsName()) {
	  
	  // Following the DXF_name token there will be the
	  // table name.  Currently all table information is
	  // skipped by reading tokens until the ENDTAB token
	  // is encounterd.
	  
	  // Get the table name
	  token = GL_scan->get_next_token();
	  while (token != endtab && token != end_of_file) {
	    token = GL_scan->get_next_token();
	  }
	  if (token == end_of_file) {
	    printf("GL_GetFaceList: unexpected end of file\n");
	  }
	} // IsName
      } // while table
      break;
      
    case blocks:     // terminated by an endblk
    case block:
    case entities: { // ended by an endsec 
      
      token = GL_scan->get_next_token();
      while (token != endsec && token != endblk && 
	     token != end_of_file) {
	
	switch (token) {
	  
	case three_d_face:
	  Get3DFACE( CurObj );
	  break;
	  
	case polyline:
	  GetPolyLine( CurObj );
	  break;
	} // switch
	
	if (token != end_of_file)
	  token = GL_scan->get_next_token();
      } // while
    } // case blocks && entities
      
    } // end switch
    if (token != end_of_file)
      token = GL_scan->get_next_token();


  } // while
  return CurObj;
} // GL_GetFaceList




/* GetPolyLine

   Process a DXF file POLYLINE section and enter the points in the
   polygon face list.

   An attempt at explaining the POLYLINE format is shown below:

     PolyLineHeader PolyVert MeshVals
     PolyLineHeader = DXF_start POLYLINE DXF_name EntName
     PolyVert = DXF_entities_flg PolyElev
     PolyElev = 10 0 20 0 30 Elevation
     MeshVals = 70 0 71 PolygonCount 72 PointCount
     EntName = string

   The code below skips the POLYLINE header and the POLYLINE vertices
   section.  It looks for the "72" marker, which indicates the number
   of points per polygon.  Or at least that is what I think it means
   and what this object uses it for.

   Following the 71 and 72 groups is a sequence of VERTEX objects, where
   each VERTEX describes one 3D point.  It is these points that are
   entered into the polygon face list.

   I have not been able to get POLYLINE objects to display properly, so
   there is clearly something wrong with my interpretation of the
   DXF file format spec.
   
 */
void GL_object::GetPolyLine(FaceList *&CurObj)
{
  gl_token token;
  int polypoint = 3;

  if (CurObj == NULL)
    CurObj = new FaceList;
  
  token = GL_scan->get_next_token();
  // skip down to the first vertex in the poly line
  while (token != vertex && token != seqend && token != end_of_file) {
    token = GL_scan->get_next_token();
    // Look for the 72 marker - the number of points in the polygon
    // If this is not found, then the default is three
    if (token == integer &&
	GL_scan->get_gl_int() == DXF_SeventyTwoFlag) {
      token = GL_scan->get_next_token();
      if (token == integer) {
	polypoint = GL_scan->get_gl_int();
      }
      else {
	printf("GetPolyLine: int value expected after '72' polycount mark\n");
      }
    }
  }
  if (token == vertex) {

    // allocate storage for one polygon in the polyline object
    CurObj->get_new_poly_face(polypoint);  

    while (token != seqend && token != end_of_file) {

      token = GL_scan->get_next_token(); // get the token following the vertex
      if (GL_scan->get_gl_int() == DXF_layer_name) {
	token = GL_scan->get_next_token(); // read the layer name

	token = GL_scan->get_next_token(); // try for colornum
	if (GL_scan->get_gl_int() == DXF_colornum) {
	  token = GL_scan->get_next_token(); // read the color value
	}
	else // it not the layer name, so put it back in the stack.
	  undo(token);

      }
      else { // it not the layer name, but it the start of a point list,
	     // push it back on the stack
	if (GL_scan->get_gl_int() == 10)
	  undo(token);
      }

      // We should now be at the start of the point description
      get_poly_point( CurObj);  // get points {10, 20, 30}
      token = GL_scan->get_next_token(); // get the token following point desc.
      while (token != seqend && 
	     token != end_of_file && 
	     GL_scan->get_gl_int() != DXF_start) {
	token = GL_scan->get_next_token();
      }
      if (token != seqend && token != end_of_file)
	token = GL_scan->get_next_token(); // should be either vertex or seqend
    } // while
  } // if
} // GetPolyLine




/* Get3DFACE

   Process a DXF file 3DFACE section and enter the face into the face
   list.

   A 3DFACE section consists of either three or four 3D points.  Each
   "face" is read in and added to the face list for the object.

   DXF files that consist entirely of 3DFACE "entities" will be interpreted
   properly and will display properly using openGL.
 */

void GL_object::Get3DFACE(FaceList *&CurObj)
{
  gl_token token;
      
  // The last token fetched should be 3DFACE.  This is followed by some
  // optional information about the face (which this program ignores),
  // followed by the point descriptions.
  //
  // The points are preceeded by the integers {10, 20,  30}, {11, 21, 31},
  // {12, 22, 32},  {13, 23, 33}.  There will always be four points.
  // if the polygon is a triangle, the third and fourth points will be
  // the same.
  //
  // Skip down to the start of the first set:
  //
  token = GL_scan->get_next_token();
  while (token != integer || (token == integer && GL_scan->get_gl_int() != 10))
    token = GL_scan->get_next_token();
  // push the "10" back into the stack
  GL_scan->put_token_in_stack( integer, GL_scan->get_gl_int() );

  // We should not be at the start of the point list
  if (CurObj == NULL)
    CurObj = new FaceList;
  
  CurObj->get_new_3DFACE();  // allocate storage for a new face
  get_3DFACE_point( CurObj );    // get points {10, 20, 30}
  get_3DFACE_point( CurObj );    // get points {11, 21, 31}
  get_3DFACE_point( CurObj );    // get points {12, 22, 32}
  get_3DFACE_point( CurObj );    // get points {13, 23, 33}
}  // Get3DFACE



/* GetCoord

   Read in a 3D point coordinate.  Each coordiniate is a "float" and
   will be preceeded by an integer value.  For example, the X
   coordinate for the first point will be preceeded by a 10, the Y
   coordinate by a 20, and the Z coordinate by a 30.
*/
   
float GL_object::GetCoord(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token != integer)
    printf("GL_object::GetCoord: integer point marker expected\n");
  
  token = GL_scan->get_next_token();
  if (token != real)
    printf("GL_object::GetCoord: real expected\n");
  return GL_scan->get_gl_float();
}



/* get_3DFACE_point

   Read in the three coordinates that make up a 3D point.  The object
   keeps track of the min and max dimensions so that the object can
   be properly displated by openGL.
 */
void GL_object::get_3DFACE_point(FaceList *FaceObjPtr)
{
  float x, y, z;
  
  x = GetCoord();
  y = GetCoord();
  z = GetCoord();

  FaceObjPtr->add_3DFACE_point(x, y, z );

  PointRange.SetMinMax( x );
  PointRange.SetMinMax( y );
  PointRange.SetMinMax( z );
}



/* get_poly_point

   Read in a 3D pont form a POLYLINE object.
 */
void GL_object::get_poly_point(FaceList *FaceObjPtr)
{
  float x, y, z;

  x = GetCoord();
  y = GetCoord();
  z = GetCoord();

  FaceObjPtr->add_poly_point(x, y, z);

  PointRange.SetMinMax( x );
  PointRange.SetMinMax( y );
  PointRange.SetMinMax( z );
}


/* ---------------------------------------------------------

   The functions below are used by the DXF file scanner to recognize
   various parts of the DXF file.  An input token stack is used
   so that the scanner can be "backed up", when an item is not found
   and a try can be made for another item.

   --------------------------------------------------------- */


BOOLEAN GL_object::IsEndSet(void)
{
  gl_token token;
  BOOLEAN retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == 62) {
    if (IsStart())
      retval = TRUE;
  }
  else
    printf("IsEndSet: value 62 expected\n");
  return retval;
}



BOOLEAN GL_object::IsEndSec(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == endsec)
    return TRUE;
  else {
    printf("ENDSEC token expected\n");
    return FALSE;
  }
}


void GL_object::undo(gl_token token)
{
  if (token == integer)
    GL_scan->put_token_in_stack( integer,  GL_scan->get_gl_int());
  else
    GL_scan->put_token_in_stack( token );
}


BOOLEAN GL_object::Is3DFace(void)
{
  gl_token token;
  int retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start) {
    token = GL_scan->get_next_token();
    if (token == three_d_face) {
      retval = TRUE;
    }
    else {
      GL_scan->put_token_in_stack( token );
      GL_scan->put_token_in_stack( integer, DXF_start );
    }
  }
  else {
    undo(token);
  }

  return retval;
}



BOOLEAN GL_object::IsTable(void)
{
  gl_token token;
  int retval = FALSE;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start) {
    token = GL_scan->get_next_token();
    if (token == table) {
      retval = TRUE;
    }
    else {
      GL_scan->put_token_in_stack( token );
      GL_scan->put_token_in_stack( integer, DXF_start );
    }
  }
  else {
    undo(token);
  }

  return retval;
}




BOOLEAN GL_object::IsLayerName(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_layer_name)
    return TRUE;
  else {
    printf("DXF_layer_name expected\n");
    return FALSE;
  }
}



BOOLEAN GL_object::IsStart(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_start)
    return TRUE;
  else {
    printf("DXF_start expected\n");
    return FALSE;
  }
}


BOOLEAN GL_object::IsName(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  if (token == integer && GL_scan->get_gl_int() == DXF_name)
    return TRUE;
  else {
    printf("DXF_name expected\n");
    return FALSE;
  }
}



//
// SkipSection
//
// Not all DXF sections are relevant to a given application and some
// sections are AutoDesk application specific.  This function will skip
// a section.  The function reads tokens until it finds the end of section
// sequence.  It then backs up, leaving the end of section sequence in place.
// If the EOF marker is found first, an error is printed.
//
void GL_object::SkipSection(void)
{
  gl_token token;

  token = GL_scan->get_next_token();
  while (token != endsec && token != end_of_file) {
    token = GL_scan->get_next_token();
  }
  // put the end section marker back in the stack
  if (token == end_of_file) {
    printf("GL_object::SkipSection: unexpected end of file\n");
    exit(1);
  }
} // SkipSection



static void print_token(gl_token token, GL_scanner &GL_scan)
{
  GL_scan.print_token( token );
  if (token == integer)
    printf(" %d", GL_scan.get_gl_int());
  else
    if (token == real)
      printf(" %f", GL_scan.get_gl_float());
    else
      if (token == name)
	printf(" %s", GL_scan.get_gl_name());
  printf("\n");  
}
