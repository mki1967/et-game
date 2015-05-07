#include <GL/gl.h>


void drawFrameBox(
		 float minX, float minY, float minZ, 
		 float maxX, float maxY, float maxZ, 
		 float red, float green, float blue 
		 )
{
  /* // EDGES */
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(red, green, blue);

  /* direction Z */

  glVertex3f(minX, minY, minZ);
  glVertex3f(minX, minY, maxZ);
 
  glVertex3f(minX, maxY, minZ);
  glVertex3f(minX, maxY, maxZ);
 
  glVertex3f(maxX, maxY, minZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(maxX, minY, minZ);
  glVertex3f(maxX, minY, maxZ);

  /* direction Y */

  glVertex3f(minX, minY, minZ);
  glVertex3f(minX, maxY, minZ);
 
  glVertex3f(minX, minY, maxZ);
  glVertex3f(minX, maxY, maxZ);
 
  glVertex3f(maxX, minY, maxZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(maxX, minY, minZ);
  glVertex3f(maxX, maxY, minZ);

  /* direction X */

  glVertex3f(minX, minY, minZ);
  glVertex3f(maxX, minY, minZ);
 
  glVertex3f(minX, minY, maxZ);
  glVertex3f(maxX, minY, maxZ);
 
  glVertex3f(minX, maxY, maxZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(minX, maxY, minZ);
  glVertex3f(maxX, maxY, minZ);


  glEnd();    
}


void drawBox(
		 float minX, float minY, float minZ, 
		 float maxX, float maxY, float maxZ, 
		 float red, float green, float blue 
		 )
{

  glColor3f(red, green, blue);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glBegin(GL_TRIANGLES);

  /* PLANES XY */
  glNormal3f( 0,0,1 );

  /* PLANE minZ */
  glVertex3f(minX, minY, minZ);
  glVertex3f(maxX, minY, minZ);
  glVertex3f(maxX, maxY, minZ);

  glVertex3f(maxX, maxY, minZ);
  glVertex3f(minX, maxY, minZ);
  glVertex3f(minX, minY, minZ);

  /* PLANE maxZ */
  glVertex3f(minX, minY, maxZ);
  glVertex3f(maxX, minY, maxZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(maxX, maxY, maxZ);
  glVertex3f(minX, maxY, maxZ);
  glVertex3f(minX, minY, maxZ);


  /* PLANES XZ */
  glNormal3f( 0,1,0 );

  /* PLANE minY */
  glVertex3f(minX, minY, minZ);
  glVertex3f(maxX, minY, minZ);
  glVertex3f(maxX, minY, maxZ);

  glVertex3f(maxX, minY, maxZ);
  glVertex3f(minX, minY, maxZ);
  glVertex3f(minX, minY, minZ);

  /* PLANE maxY */
  glVertex3f(minX, maxY, minZ);
  glVertex3f(maxX, maxY, minZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(maxX, maxY, maxZ);
  glVertex3f(minX, maxY, maxZ);
  glVertex3f(minX, maxY, minZ);

  /* PLANES YZ */
  glNormal3f( 1,0,0 );

  /* PLANE minX */
  glVertex3f(minX, minY, minZ);
  glVertex3f(minX, maxY, minZ);
  glVertex3f(minX, maxY, maxZ);

  glVertex3f(minX, maxY, maxZ);
  glVertex3f(minX, minY, maxZ);
  glVertex3f(minX, minY, minZ);

  /* PLANE maxX */
  glVertex3f(maxX, maxY, minZ);
  glVertex3f(maxX, minY, minZ);
  glVertex3f(maxX, maxY, maxZ);

  glVertex3f(maxX, maxY, maxZ);
  glVertex3f(maxX, minY, maxZ);
  glVertex3f(maxX, minY, minZ);


  glEnd();    

}

/* // TRIANGLES */
/*
  for(i=0; i<gptr->triangle_top; i++)
  {
  glColor3fv( color[gptr->triangle_color[i]] );
  glNormal3fv( &(gptr->triangle_normal[3*i]) );
  glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i]]) );
  glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i+1]]) );
  glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i+2]]) );
  }
  
  glEndList();
  }

*/
