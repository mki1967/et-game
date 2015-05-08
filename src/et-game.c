/**
 *
 *    et-game 
 * 
 *    Copyright (C) 2003, 2013  Marcin Kik
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *    The author can be reached at mki1967@gmail.com
 *       
 *
 **/

#include "et-game.h"

/* //// FUNCTIONS  */


int file_exists(char* fname)
{
  FILE* f;
  int r;
  f=fopen(fname,"r");
  r= (f==NULL)? 0:1;
  if(f!=NULL) fclose(f);
  return r;
}


void stage_names_load(FILE* conf)
{
  /* FILE* f; */
  char fname[500];
  Bool ok=True;


  while(stage_top<STAGE_MAX && ok)
    {
      char fullName[MAX_PATH_LENGTH];

      fscanf(conf, "%490s", fname);
      fullName[0]= (char) 0;
      strcat(fullName, dataDir);
      strcat(fullName, fname);
      printf("TEST stage_names_load: %s\n", fullName);

      ok= file_exists(fullName);
      if( ok)
	{
	  stage_name[stage_top]= 
	    (char*) malloc(sizeof(char)*(strlen(fname)+1));
	  if(stage_name[stage_top]!=NULL)
	    {
	      strcpy(stage_name[stage_top], fname);
	      stage_top++;
	    }
	  else ok=False;
	}
    }

}




void scene_get_range(struct Graph* sptr, 
		     float vmin[3], float vmax[3])
{
  int i=0 /* , used=0 */;
  float_zeroes(3,vmin);
  float_zeroes(3,vmax);

  if(sptr->vsize>0)
    {
      vectorcpy(vmin, &(sptr->vertex[3*0]));
      vectorcpy(vmax, &(sptr->vertex[3*0]));
    }

  while(i<sptr->vsize)
    {
      if(sptr->vertex[3*i]<vmin[0]) vmin[0]=sptr->vertex[3*i];
      else
	if(sptr->vertex[3*i]>vmax[0]) vmax[0]=sptr->vertex[3*i];
      
      if(sptr->vertex[3*i+1]<vmin[1]) vmin[1]=sptr->vertex[3*i+1];
      else
	if(sptr->vertex[3*i+1]>vmax[1]) vmax[1]=sptr->vertex[3*i+1];
      
      if(sptr->vertex[3*i+2]<vmin[2]) vmin[2]=sptr->vertex[3*i+2];
      else
	if(sptr->vertex[3*i+2]>vmax[2]) vmax[2]=sptr->vertex[3*i+2];
      
      i++;
    }
}



void token_try_collect()
{
  int i;
  if(gate_visible &&
     fabs(gate_position[0]-traveler.position[0])<token_eps &&
     fabs(gate_position[1]-traveler.position[1])<token_eps &&
     fabs(gate_position[2]-traveler.position[2])<token_eps 
     )
    {
      printf("TELEPORTING ...\n");
      config_next_scene();
      return;
    }
  
  for(i=0; i< TOKEN_MAX; i++)

    if(
       !token_collected[i] &&
       fabs(token_position[i][0]-traveler.position[0])<token_eps &&
       fabs(token_position[i][1]-traveler.position[1])<token_eps &&
       fabs(token_position[i][2]-traveler.position[2])<token_eps 
       )
      {int s;
	token_collected[i]=True;
	tokens_left--;
	if(sound) printf("\a");
	printf("REMAINING OBJECTS: %d. ",tokens_left); 
	printf("TIME: %d SECONDS\n", s=time(NULL)-token_timer);
	if(tokens_left==0 && !gate_visible)
	  {
	    printf("1000000/(%d*%d)=%d: ", s+1,s+1,1000000/((s+1)*(s+1)));
	    s=1000000/((s+1)*(s+1));
	    printf("YOU HAVE %d+%d=%d POINTS.\n",
		   score,s, score+s);
	    score+=s; 
	    /* // if(score-last_score>= 100) */
	    {
	      gate_visible=True;
	      printf("STAGE FINISHED IN %d SECONDS.\n",
		     s=time(NULL)-stage_timer);
	      if(s<=150)
		{
		  printf("BONUS: 100*(150-%d) = %d.", s, 100*(150-s));
		  s=100*(150-s);
		  printf("YOU HAVE %d+%d=%d POINTS.\n",
			 score,s, score+s);
		  score+=s;
		}

	      printf("FIND TELEPORT TO NEXT STAGE !\n");
	    }
	  } 
      }
}



void token_positions_init()
{
  int i;
  float vmin[3], vmax[3];
  scene_get_range(&scene, 
		  vmin,  vmax);

  srandom(time(NULL));

  for(i=0; i< TOKEN_MAX; i++)
    {
      token_position[i][0]=vmin[0]+random()%((int) floorf(vmax[0]-vmin[0]));
      token_position[i][1]=vmin[1]+random()%((int) floorf(vmax[1]-vmin[1]));
      token_position[i][2]=vmin[2]+random()%((int) floorf(vmax[2]-vmin[2]));
      token_collected[i]=False;
    }
  tokens_left=TOKEN_MAX;
  token_timer=time(NULL);

  printf("COLLECT %d OBJECTS. TIMER STARTED !\n",tokens_left );
}


void gate_init()
{
  float vmin[3], vmax[3];
  scene_get_range(&scene, 
		  vmin,  vmax);
  gate_position[0]=vmin[0]+random()%((int) floorf(vmax[0]-vmin[0]));
  gate_position[1]=vmin[1]+random()%((int) floorf(vmax[1]-vmin[1]));
  gate_position[2]=vmin[2]+random()%((int) floorf(vmax[2]-vmin[2]));
  gate_visible=False;
}


/* ///// TRAVELER */


void traveler_init(struct Traveler * traveler)
{
  traveler->radius=0.5;
  traveler->h_angle=0;
  traveler->v_angle=0;
  float_zeroes(3, traveler->position);
}



void traveler_move(float x, float y, float z, struct Traveler * traveler)
{
  float m[16];

  float_zeroes(16, m);
  m[0]=x;
  m[1]=y;
  m[2]=z;

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glLoadIdentity();
  glRotated(traveler->h_angle, 0,1,0);
  glRotated(traveler->v_angle, 1,0,0);
  glMultMatrixf(m);
  glGetFloatv( GL_MODELVIEW_MATRIX, m );
  glPopMatrix();

  if(boundAlert>0) boundAlert--;
  vector_add(m,traveler->position,traveler->position);


  /* CHECKING FOR BOUNDING LIMITS */

  if(traveler->position[0] < traveler->vmin[0]-boundMargin) {
    traveler->position[0] = traveler->vmin[0]-boundMargin;
    boundAlert=boundAlertInit;
  }
  if(traveler->position[0] > traveler->vmax[0]+boundMargin) {
    traveler->position[0] = traveler->vmax[0]+boundMargin;
    boundAlert=boundAlertInit;
  }

  if(traveler->position[1] < traveler->vmin[1]-boundMargin) {
    traveler->position[1] = traveler->vmin[1]-boundMargin;
    boundAlert=boundAlertInit;
  }
  if(traveler->position[1] > traveler->vmax[1]+boundMargin) {
    traveler->position[1] = traveler->vmax[1]+boundMargin;
    boundAlert=boundAlertInit;
  }

  if(traveler->position[2] < traveler->vmin[2]-boundMargin) {
    traveler->position[2] = traveler->vmin[2]-boundMargin;
    boundAlert=boundAlertInit;
  }
  if(traveler->position[2] > traveler->vmax[2]+boundMargin) {
    traveler->position[2] = traveler->vmax[2]+boundMargin;
    boundAlert=boundAlertInit;
  }


  /* COLLECT TOKEN,  IF POSSIBLE */
  token_try_collect();


  /* OLD VERSION */
  /*
  if(
     m[0]+traveler->position[0]<traveler->vmin[0]-boundMargin ||
     m[0]+traveler->position[0]>traveler->vmax[0]+boundMargin ||
     m[1]+traveler->position[1]<traveler->vmin[1]-boundMargin ||
     m[1]+traveler->position[1]>traveler->vmax[1]+boundMargin ||
     m[2]+traveler->position[2]<traveler->vmin[2]-boundMargin ||
     m[2]+traveler->position[2]>traveler->vmax[2]+boundMargin 
     )
    {
    printf("YOU ARE GETTING OUT OF RANGE !!!\n");
    boundAlert=boundAlertInit;
    }
  else
    {
      if(boundAlert>0) boundAlert--;
      vector_add(m,traveler->position,traveler->position);
      token_try_collect();
    }
  */
}


void traveler_h_rotate(int angle, struct Traveler * traveler)
{
  traveler->h_angle=(traveler->h_angle+angle+360)%360;
}

void traveler_v_rotate(int angle, struct Traveler * traveler)
{
  if(fabs(angle+traveler->v_angle) <= v_max_angle)
    traveler->v_angle+=angle;
}

void traveler_upright(struct Traveler * traveler)
{
  traveler->v_angle=0;
}

/* ////// SCREEN */


void screen_init(struct Screen* scr)
{
  scr->distance=screen_distance;
  scr->clip_min=clip_min;
  scr->clip_max=clip_max;
  scr->width=screen_width;
  scr->height=screen_height;
}


void screen_set_distance(struct Screen* scr,float distance)
{
  scr->distance=distance;
  setfrustum();
  printf("Screen distance is: %f\n", scr->distance);
}

void screen_set_clipping(struct Screen* scr,float clip_min, float clip_max)
{
  if(clip_min_limit<= clip_min && clip_min<= clip_max)
    {
      scr->clip_min=clip_min;
      scr->clip_max=clip_max;
      setfrustum();
    }
  else printf("Screen clipping: bad values !\n");
  printf("Screen: clip_min = %f, clip_max = %f\n", scr->clip_min, scr->clip_max);

}


/* /// VECTOR OPERATIONS */

void float_zeroes(int n, float v[])
{
  int i;
  for(i=0; i<n; i++) v[i]=0.0;
}

void double_zeroes(int n, double v[])
{
  int i;
  for(i=0; i<n; i++) v[i]=0.0;
}


Bool vector_eq(float  a[], float b[])
{
  return a[0]==b[0] && a[1]==b[1] && a[2]==b[2];
}

void vector_add(float a[], float b[], float result[])
{
  result[0]=a[0]+b[0];
  result[1]=a[1]+b[1];
  result[2]=a[2]+b[2];
}

void vector_sub(float a[], float b[], float result[])
{
  result[0]=a[0]-b[0];
  result[1]=a[1]-b[1];
  result[2]=a[2]-b[2];
}

void vector_scale( float s, float v[])
{
  v[0]*=s;  v[1]*=s; v[2]*=s;
}

void vectorcpy(float dst[], float src[])
{
  dst[0]=src[0];
  dst[1]=src[1];
  dst[2]=src[2];
}


void vector_product(float a[], float b[], float result[])
{
  result[0]=a[1]*b[2]-a[2]*b[1];
  result[1]=a[2]*b[0]-a[0]*b[2];
  result[2]=a[0]*b[1]-a[1]*b[0];
}

float scalar_product(float a[], float b[])
{
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void vector_normalize(float v[])
{
  float sp=scalar_product(v,v);
  if(sp==0) return; /* // zero length vector */
  sp=sqrt(sp); 
  v[0]/=sp;  v[1]/=sp;  v[2]/=sp;
}





/* ///// VECTOR I/O */

void vector_fprintf(FILE *stream, float v[3])
{
  fprintf(stream, "%f\n", v[0]);
  fprintf(stream, "%f\n", v[1]);
  fprintf(stream, "%f\n", v[2]);
} 

void vector_fscanf(FILE *stream, float v[3])
{
  fscanf(stream, "%f", &v[0]);
  fscanf(stream, "%f", &v[1]);
  fscanf(stream, "%f", &v[2]);
} 





/* //// LIGHT */

void set_light(float light[16])
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotated(traveler.h_angle, 0,1,0);
  glRotated(traveler.v_angle, 1,0,0);
  glMultMatrixf(basic_light);
  glGetFloatv( GL_MODELVIEW_MATRIX, light);
  glPopMatrix();
  printf("light=( %f, %f, %f, %f )\n", light[0], light[1], light[2], light[3]);
}





/* //// EDGE */





void triangle_normal_vector(float a[], float b[], float c[], float norm[])
{
  float v[3], w[3];
  vector_sub(b,a, v);
  vector_sub(c,a, w);
  vector_product(v,w, norm);
  vector_normalize(norm);
}



/* ///// LOADING */

void graph_free(struct Graph * gptr)
{
  free(gptr->vertex);
  free(gptr->edge);
  free(gptr->edge_color);
  free(gptr->triangle);
  free(gptr->triangle_color);
  free(gptr->triangle_normal);

  graph_init(gptr);

}


void graph_init(struct Graph * gptr)
{
  gptr->vertex=NULL;
  gptr->edge=NULL;
  gptr->edge_color=NULL;
  gptr->triangle=NULL;
  gptr->triangle_color=NULL;
  gptr->triangle_normal=NULL;
}


void config_next_scene()
{
  struct Graph scene1;
  graph_init(&scene1);

  /*
    if(stage_random) stage_current=random()%stage_top;
    else stage_current++;
  */

  stage_current++;

  if(0<stage_current && stage_current<stage_top-2)
    { /* swap with random stage in the range [stage_current, stage_top-2] */
      int r;
      r= random() % (stage_top-2-stage_current) ;
      if(r>0){
	char* tmp;
	tmp=stage_name[stage_current];
	stage_name[stage_current]=stage_name[stage_current+r];
	stage_name[stage_current+r]=tmp;
      }
    }


  if(stage_current<stage_top &&
     scene_load(stage_name[stage_current], 
		&scene1, light, &background ,&traveler)==0
     )
    {
      printf("STAGE: %s\n", stage_name[stage_current]);
      graph_free(&scene);
      scene=scene1;
      graph_to_display_list(scene_list, &scene);
      last_score=score;
      stage_timer=time(NULL);
      printf("STAGE TIMER STARTED.\n");
      gate_init();
      token_positions_init();
    }  
  else
    {
      printf("\nTHERE IS NO NEXT STAGE ...\n");
      printf("YOUR SCORE: %d\n", score);
      exit(0);
    }
}

int scene_load(
	       char* fname,
	       struct Graph * gptr,
	       float light[3],
	       int* background,
	       struct Traveler *travptr
	       )
{
  FILE *stream;
  /* char label[200]; */
  char fullName[MAX_PATH_LENGTH];
  fullName[0]= (char) 0;
  strcat(fullName, dataDir);
  strcat(fullName, fname);
  printf("TEST scene_load: %s\n", fullName);
  stream=fopen(fullName,"r");

  if(stream==NULL) 
    {
      printf("load: can not open '%s'\n", fname);
      return -1; 
    }
  graph_fscanf(stream, gptr);

  traveler_init(travptr);


  remaining_fscanf(
		   stream,
		   light,
		   background,
		   travptr
		   );

  fclose(stream);


  scene_get_range(gptr, travptr->vmin, travptr->vmax);

  if(travptr->position[0] < travptr->vmin[0])
    travptr->position[0]= travptr->vmin[0]; 
  if(travptr->position[1] < travptr->vmin[1])
    travptr->position[1]= travptr->vmin[1]; 
  if(travptr->position[2] < travptr->vmin[2])
    travptr->position[2]= travptr->vmin[2]; 

  if(travptr->position[0] > travptr->vmax[0])
    travptr->position[0]= travptr->vmax[0]; 
  if(travptr->position[1] > travptr->vmax[1])
    travptr->position[1]= travptr->vmax[1]; 
  if(travptr->position[2] > travptr->vmax[2])
    travptr->position[2]= travptr->vmax[2]; 

  /*   printf("loaded from: '%s'\n", fname); */
  return 0;
}



int graph_load(
	       char* fname,
	       struct Graph * gptr

	       )
{
  FILE *stream;
  /*  char label[200]; */

  char fullName[MAX_PATH_LENGTH];
  fullName[0]= (char) 0;
  strcat(fullName, dataDir);
  strcat(fullName, fname);
  printf("TEST graph_load: %s\n", fullName);
  stream=fopen(fullName,"r");

  if(stream==NULL) 
    {
      printf("load: can not open '%s'\n", fname);
      return -1; 
    }
  graph_fscanf(stream, gptr);

  fclose(stream);
  /*   printf("loaded from: '%s'\n", fname); */
  return 0;
}



void graph_fscanf(
		  FILE *stream,
		  struct Graph * gptr
		  )
{
  /*  char label[200]; */
  graph_free(gptr);

  {
    /* // VERTICES */
    int i;
    fscanf(stream,"%d",&(gptr->vsize));
    free(gptr->vertex);
    gptr->vertex=NULL;
    gptr->vertex= (float *) malloc(sizeof(float)*3* gptr->vsize);
    for(i=0; i<gptr->vsize; i++)
      {
	fscanf(stream,"%f", &(gptr->vertex[3*i]));
	fscanf(stream,"%f", &(gptr->vertex[3*i+1]));
	fscanf(stream,"%f", &(gptr->vertex[3*i+2]));

      }
  }


  {
    /* // EDGES */
    int i;
    fscanf(stream,"%d", &(gptr->edge_top));
    gptr->edge= (int*) malloc( sizeof(int)*2* gptr->edge_top);
    gptr->edge_color= (int*) malloc( sizeof(int)* gptr->edge_top);
    for(i=0; i<gptr->edge_top; i++)
      {
	fscanf(stream,"%d", &(gptr->edge[2*i]));
	fscanf(stream,"%d", &(gptr->edge[2*i+1]));
	fscanf(stream,"%d", &(gptr->edge_color[i]));

      }

  }


  {
    /* // TRIANGLES */
    int i;
    fscanf(stream,"%d", &(gptr->triangle_top));
    gptr->triangle= (int*) malloc( sizeof(int)*3* gptr->triangle_top);
    gptr->triangle_color= (int*) malloc( sizeof(int)* gptr->triangle_top);
    gptr->triangle_normal= (float*) malloc( sizeof(float)*3*gptr->triangle_top);
    for(i=0; i<gptr->triangle_top; i++)
      {
	fscanf(stream,"%d", &(gptr->triangle[3*i]));
	fscanf(stream,"%d", &(gptr->triangle[3*i+1]));
	fscanf(stream,"%d", &(gptr->triangle[3*i+2]));
	fscanf(stream,"%d", &(gptr->triangle_color[i]));
	triangle_normal_vector(
			       &(gptr->vertex[3*(gptr->triangle[3*i])]),
			       &(gptr->vertex[3*(gptr->triangle[3*i+1])]),
			       &(gptr->vertex[3*(gptr->triangle[3*i+2])]),
			       &(gptr->triangle_normal[3*i])
			       );
      }

  }
  


} 



void remaining_fscanf(
		      FILE* stream,
		      float light[3],
		      int* background,
		      struct Traveler *travptr
		      )
{
  char label[200];
  /*  float cursor[3]; */


  /* // TRANSFORMATION */
  fscanf(stream, "%s", label);
  if(strcmp(label, transformation_label)==0)
    do
      {   
	fscanf(stream, "%s", label);
      }
    while(strcmp(label, light_label)!=0);

  /* // LIGHT */
  vector_fscanf(stream, light);
    

  /* // BACKGROUND */
  fscanf(stream, "%s", label);
  if(strcmp(label, background_label)==0)
    {
      fscanf(stream, "%d\n", background);
    }

  /* // CURSOR */
  fscanf(stream, "%s", label);
  if(strcmp(label, cursor_label)==0)
    {
      vector_fscanf(stream, travptr->position);
    }

} 
    







/* //////////// X STUFF /////// */


void initglx()
{

  display=XOpenDisplay( NULL );
  if(display==NULL) 
    {
      printf("XOpenDisplay( NULL )== NULL\n");
      exit(-1);
    }


  {
    int glxminor, glxmajor;
    printf("glXQueryExtension: %d\n", glXQueryExtension(display, NULL, NULL));
    glXQueryVersion( display, &glxmajor, &glxminor);
    printf("Version : %d.%d\n", glxmajor, glxminor);
  }

  {
    XVisualInfo vinfo_template;
    vinfo_template.screen=DefaultScreen(display);
    xvisualinfo_array = XGetVisualInfo(display, 
				       VisualScreenMask,
				       &vinfo_template, &xvisualinfo_nitems);
    if(xvisualinfo_array==NULL)
      {
	printf("No visual infos for default screen \n");
	exit(-1);
      }
  }

  {
    int revert_to;
    XGetInputFocus(display, &terminal,&revert_to);
  }

  window= XCreateSimpleWindow( display, DefaultRootWindow(display) , 0,0, 
			       screen_width, screen_height,
			       0, 0, 0 );


  {
    /* /// Enable events for the window */

    unsigned long valuemask=CWEventMask;
    windowattributes.event_mask=
      KeyPressMask   
      | ExposureMask        
      ;
    XChangeWindowAttributes(display, window, valuemask, &windowattributes);    
  }
  XStoreName(display, window, program_name);
  XMapWindow(display, window);
  XFlush(display);

  /* // printf("terminal: %d\n", terminal);  */
  /* // printf("window: %d\n", window);  */

 
  glxcontext= glXCreateContext(display, &xvisualinfo_array[0], NULL, True);

  printf("glXIsDirect: %d\n", glXIsDirect(display, glxcontext) );

  glXMakeCurrent(display, window, glxcontext);
 
}


/* ///////////////////////////////////////////////////////////////////////////////// */


void mainloop()
{
  XEvent event;

  while(1)
    {
      XWindowEvent(display, window, windowattributes.event_mask, &event);
      switch(event.type)
	{
        case KeyPress: callbackKeyPress(  &(event.xkey) ); break;
	case Expose: callbackExpose(  &(event.xexpose) ); break;
	}

    }
}





/* ////// CALLBACKS */

void callbackExpose( XExposeEvent* evptr)
{

  long event_mask=ExposureMask;





  while(XCheckWindowEvent(display,window,event_mask , (XEvent*) evptr));

  glDeleteLists(scene_list, 1);
  glDeleteLists(token_list, 1);
  glDeleteLists(gate_list, 1);
  
  glXMakeCurrent(display, None, NULL);
  glXDestroyContext(display,glxcontext);
  glxcontext= glXCreateContext(display, &xvisualinfo_array[0], NULL, True);
  glXMakeCurrent(display, window, glxcontext);
  {
    int param[4];
    glGetIntegerv(GL_VIEWPORT, param);
    if(screen.width != param[2] || screen.height!= param[3])
      {
	screen.width= param[2];
	screen.height= param[3];
	printf("viewport: %d x %d\n", screen.width, screen.height);
      }
  }
  setfrustum();

  /* // REDEFINE DISPLAY LISTS */
  scene_list= glGenLists(1);
  token_list=glGenLists(1);
  gate_list=glGenLists(1);
 

  graph_to_display_list(scene_list, &scene);
  graph_to_display_list(token_list, &token);
  graph_to_display_list(gate_list, &gate);
  redraw();

}


/* ////////////// KEY COMMANDS //////////// */

void callbackKeyPress( XKeyEvent* evptr)
{
  long event_mask=KeyPressMask;
  while(XCheckWindowEvent(display,window,event_mask , (XEvent*) evptr));

  if(paused)
    {
      paused=False;
      stage_timer=time(NULL)-stage_timer-1;
      token_timer=time(NULL)-token_timer-1;
      printf("RESUMED. Stage Timer: %d. Collection_Timer: %d.\n",
	     (int) (time(NULL)-stage_timer), (int) (time(NULL)-token_timer) );

    }

  if(tokens_left<=0 && !gate_visible) token_positions_init();

  switch(XLookupKeysym(evptr,0))
    {

    case XK_Up : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	  traveler_v_rotate(v_rot_step, &traveler); redraw(); break;
	case ShiftMask:
          traveler_move(0,mov_step,0, &traveler); redraw(); break;
	}
      break;
    case XK_Down : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	  traveler_v_rotate(-v_rot_step, &traveler); redraw(); break;
	case ShiftMask:
          traveler_move(0,-mov_step,0, &traveler); redraw(); break;
	}
      break;

    case XK_Left : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	  traveler_h_rotate(h_rot_step, &traveler); redraw(); break;
	case ShiftMask:
          traveler_move(-mov_step,0,0, &traveler); redraw(); break;
	}
      break;
    case XK_Right : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	  traveler_h_rotate(-h_rot_step, &traveler); redraw(); break;
	case ShiftMask:
          traveler_move(mov_step,0,0, &traveler); redraw(); break;
	}
      break;


    case XK_Home: 
    case XK_f : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	case ShiftMask:
          traveler_move(0,0,-mov_step, &traveler); redraw(); break;
	}
      break;

    case XK_End:
    case XK_b : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	case ShiftMask:
          traveler_move(0,0,mov_step, &traveler); redraw(); break;
	}
      break;

    case XK_p : 
      switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask))
	{
	case 0:
	  paused=True;
          stage_timer=time(NULL)-stage_timer;
          token_timer=time(NULL)-token_timer;
	  printf("PAUSED. Stage Timer: %d. Collection Timer: %d.\n",
		 (int) stage_timer, (int) token_timer);

	  redraw(); 
	  break;
	}
      break;


    case XK_Delete:
    case XK_BackSpace:
    case XK_Page_Up:
    case XK_Page_Down:
      break;
    case XK_space: 
      traveler_upright(&traveler); redraw(); break;
    case XK_Tab:
      break; 
    case XK_Escape : 
      break;

     

      /*     case XK_b: */
      /*       background= (background+1)% COLOR_MAX; redraw(); break; */

      /*     case XK_l: */
      /*       switch((evptr->state)&(ControlMask|Mod1Mask|ShiftMask)) */
      /* 	{ */
      /* 	case 0: */
      /* 	  set_light(light); redraw(); break; */
      /* 	}; */
      /*       break; */
          

    case XK_h:
      help_keys(); break;

    case XK_s:
      if(sound)
	{
	  sound=0;
	  printf("SOUND OFF\n");
	}
      else
	{
	  sound=1;
	  printf("SOUND ON\n");
	}      
      break;
      
      /*
	case XK_r:
	if(stage_random) 
	{
	stage_random=False;
	printf("RANDOM STAGE SELECTION: OFF\n");
	}
	else
	{
	stage_random=True;
	printf("RANDOM STAGE SELECTION: ON\n");
	}
	break;
      */

    case XK_q:
      exit(0); break;
    }
}






void setfrustum()
{
  float rwidth;
  float rheight;

  rwidth = screen.width*pixel_size/2;
  rheight = screen.height*pixel_size/2;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum( -rwidth*screen.clip_min/screen.distance, rwidth*screen.clip_min/screen.distance,
             -rheight*screen.clip_min/screen.distance, rheight*screen.clip_min/screen.distance,
             screen.clip_min, screen.clip_max) ;

}



/* //// REDRAW ///////// */



void graph_to_display_list(GLuint list, struct Graph *gptr)
{
  int i;

  glNewList(list, GL_COMPILE);

  /* // EDGES */
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  for(i=0; i<gptr->edge_top; i++)
    {
      glColor3fv( color[gptr->edge_color[i]] );
      glVertex3fv( &(gptr->vertex[3*gptr->edge[2*i]]) );
      glVertex3fv( &(gptr->vertex[3*gptr->edge[2*i+1]]) );
    }
  
  glEnd();    

  /* // TRIANGLES */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glBegin(GL_TRIANGLES);
  for(i=0; i<gptr->triangle_top; i++)
    {
      glColor3fv( color[gptr->triangle_color[i]] );
      glNormal3fv( &(gptr->triangle_normal[3*i]) );
      glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i]]) );
      glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i+1]]) );
      glVertex3fv( &(gptr->vertex[3*gptr->triangle[3*i+2]]) );
    }
  
  glEnd();    
  glEndList();
}



void redraw()
{

  glClearColor(color[background][0], color[background][1], color[background][2], 0.0);
  glEnable(GL_DEPTH_TEST);


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glRotatef(-traveler.v_angle, 1,0,0); /* // vertical rotation */
  glRotatef(-traveler.h_angle, 0,1,0); /* // horizontal rotation */
  glTranslated(-traveler.position[0],-traveler.position[1],-traveler.position[2]);
    
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



  {
    float diffuse[4];

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv( GL_LIGHT0, GL_POSITION, light);

    vector_scale(-1,light);
    diffuse[0]=diffuse[1]=diffuse[2]=diffuse[3]=1.0;
    glLightfv( GL_LIGHT1, GL_POSITION, light);
    glLightfv(GL_LIGHT1,GL_DIFFUSE, diffuse);
    vector_scale(-1,light);
  }

  if(boundAlert){
  drawFrameBox(
	       traveler.vmin[0]-boundMargin2, traveler.vmin[1]-boundMargin2, traveler.vmin[2]-boundMargin2,
	       traveler.vmax[0]+boundMargin2, traveler.vmax[1]+boundMargin2, traveler.vmax[2]+boundMargin2,
	       1,0,0 /* RED */
	       );

  drawBox(
	       traveler.vmin[0]-boundMargin2, traveler.vmin[1]-boundMargin2, traveler.vmin[2]-boundMargin2,
	       traveler.vmax[0]+boundMargin2, traveler.vmax[1]+boundMargin2, traveler.vmax[2]+boundMargin2,
	       0.6,0,0 /* RED */
	       );
  }


  glCallList( scene_list );

  {
    /* // DRAW TOKENS */
    int i;
    for(i=0; i<TOKEN_MAX; i++)
      if(!token_collected[i])
	{
	  glPushMatrix();
	  glTranslatef(
		       token_position[i][0],
		       token_position[i][1],
		       token_position[i][2] 
		       );
	  glCallList( token_list );
	  glPopMatrix();
	}
  }

  if(gate_visible) 
    {
      glPushMatrix();
      glTranslatef(
		   gate_position[0],
		   gate_position[1],
		   gate_position[2] 
		   );
      glCallList( gate_list );
      glPopMatrix();
    }



  glFlush(); 
  glXSwapBuffers(display,window);
}



/* ////////// MAIN */

void help_keys()
{
  printf("\n--------- LIST OF KEY COMMANDS -------------\n");
  printf("<Arrow keys> - turn your head\n");
  printf("<Home>/<B>, <End>/<F> - forward, backward\n");
  printf("<Shift>+(<Arrow keys>, <Home>, <End>) - move the observer\n");
  printf("<Space> - reset observer upright\n");
  printf("<H> - list of key commands\n");
  printf("<R> - random stage selection on/off switch\n");
  printf("<P> - pause\n");
  printf("<S> - sound OFF/ON\n");
  /*   printf("<L> - set the light perpendicular to the screen\n"); */
  /*   printf("<B> - change background color\n"); */
  printf("<Q> - quit\n");

  /* // ... */

  printf("-----------------------------------------\n\n");


}


int main(int argc, char *argv[])
{
  char * configFileName;

  if(argc < 2){
    /*
      printf("usage:   et-game  config_file_name\n");
      exit(-1);
    */
    dataDir = DATADIR "/" PACKAGE "/" ;
    configFileName = DATADIR "/" PACKAGE "/game.cfg";
  } else {
    configFileName = argv[1];
  }

  printf("\n TEST: %s %s\n", dataDir , configFileName );


  printf("==================================================\n");
  printf("E.T. GAME: SEARCH GAME\n");
  printf("version: " PACKAGE_VERSION "\n\n");
  printf("Copyright (C) 2003, 2013  Marcin Kik   mki1967@gmail.com \n");
  printf("\nE.T. GAME comes with ABSOLUTELY NO WARRANTY.\n");

  printf("This is free software, and you are welcome to redistribute it\n");
  printf(" under conditions of GNU GENERAL PUBLIC LICENSE\n");

  printf("Send any comments or suggestions to: " PACKAGE_BUGREPORT "\n");
  printf("Most recent versions are available at: " PACKAGE_URL "\n");

  printf("\n");
  printf("(Press key <H> on graphical window to print the list of key commands)\n");


  printf("==================================================\n");
  
  initglx();
  screen_init(&screen);
  setfrustum();

  srandom(time(NULL));

  graph_init(&scene);
  graph_init(&token);
  graph_init(&gate);

  scene_list= glGenLists(1);
  token_list=glGenLists(1);
  gate_list=glGenLists(1);


  config_stream=fopen(configFileName,"r");

  if(config_stream == NULL)
    {
      printf("can not open config file: %s\n", argv[1]);
      exit(-1);
    }

  {
    char fname[500];
    fscanf(config_stream, "%490s", fname);
    graph_load( fname,
		&token
		);
    graph_to_display_list(token_list, &token);

    fscanf(config_stream, "%490s", fname);
    graph_load( fname,
		&gate
		);
    graph_to_display_list(token_list, &token);
  }

  stage_names_load(config_stream);
  fclose(config_stream);

  printf("NUMBER OF STAGES: %d.\n", stage_top);
  printf("FOR RANDOM STAGE SELECTION PRESS <R> KEY\n");

  config_next_scene();





  XRaiseWindow(display, window);
  /* // XSetInputFocus(display, window, RevertToParent, CurrentTime); */
  

  mainloop();

  return 0;
}








