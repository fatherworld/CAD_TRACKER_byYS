
#ifndef GLTOKEN_H

#define GLTOKEN_H

//
// The gl_token enumeration defines the tokens that are returned by the
// scanner and are used by the parser in attempting to recognize the
// components of a DXF file.
//

typedef enum { bad_gl_token,
	       section,
               header,
               endsec,
	       seqend,
               blocks,
	       block,
               endblk,
               vertex,
               tables,
               table,
	       layer,
               contin,
               endtab,
	       entities,
               three_d_face,
               end_of_file,
               polyline,
               integer,
               real,
               name,
               last_gl_token } gl_token;

#endif
