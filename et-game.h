/**
 *
 *    et-game 
 * 
 *    Copyright (C) 2003, 2013  Marcin Kik mki1967@gmail.com
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define  XK_MISCELLANY
#define  XK_LATIN1
#include <X11/keysymdef.h>

#include <GL/gl.h>
#include <GL/glx.h>


/* ///// PROGRAM VARIABLES AND FUNCTIONS //////////////////////// */

/* ///////// X / GLX VARIABLES */

Display *display;
XVisualInfo *xvisualinfo_array;
int xvisualinfo_nitems;
Window window;
Window terminal;
XSetWindowAttributes windowattributes;

GLXContext glxcontext;



/* //////// PROGRAM INTERNAL VARIABLES */


float pixel_size=0.028;
float screen_distance= 20.0;
float clip_min_limit=0.0001;
float clip_min= 0.2;
float clip_max= 100.0;

float mov_step=0.5;
float h_rot_step=5, v_rot_step=5;
float v_max_angle=80; 

int screen_width=800, screen_height=600;

char program_name[]="E.T. GAME";

#define MAX_FILE_NAME 200
char file_name[MAX_FILE_NAME+5]="noname.et";

int sound=1;


/* ///// TRAVELER VARIABLES */

struct Traveler
{
  float radius;
  float position[16];
  float vmin[3], vmax[3]; 
  int h_angle, v_angle;
};

struct Traveler traveler;


/* /// SCREEN VARIABLES */

struct Screen
{
  float distance;
  float clip_min, clip_max;
  int width, height;
};

struct Screen screen;




/* ///// COLOR VARIABLES  */

#define COLOR_MAX 12

float color[COLOR_MAX][3] =
  {
    { 1.0, 1.0, 1.0},
    { 1.0, 0.0, 0.0},
    { 0.0, 1.0, 0.0},
    { 0.0, 0.0, 1.0},
    { 1.0, 1.0, 0.0},
    { 1.0, 0.0, 1.0},
    { 0.0, 1.0, 1.0},
    { 1.0, 0.5, 0.5},
    { 0.5, 1.0, 0.5},
    { 0.5, 0.5, 1.0},
    { 0.5, 0.5, 0.5},
    { 0.0, 0.0, 0.0},
  };

int background= COLOR_MAX-1;
int current_color=0;



/* ///// LIGHT VARIABLES */

float basic_light[16]= {0.0, 0.0, 1.0, 0.0};
float light[16]= {0.0, 0.0, 1.0, 0.0};
Bool two_lights = True;




/* //// GRAPH */

struct Graph
{
  int vsize;
  float *vertex;
  
  int *edge; 
  int *edge_color;
  int edge_top;

  int *triangle; 
  int *triangle_color; 
  float *triangle_normal;
  int triangle_top;
};


struct Graph scene, token, gate;





/* //// LOADING VARIABLES */
char transformation_label[]="TRANSFORMATION";
char light_label[]="LIGHT";
char background_label[]="BACKGROUND";
char cursor_label[]="CURSOR";




/* /// DISPLAY LISTS */

GLuint scene_list, token_list, gate_list;

#define TOKEN_MAX 10

float token_position[TOKEN_MAX][3];
Bool token_collected[TOKEN_MAX];
int tokens_left=TOKEN_MAX;
float token_eps=1.0;
time_t token_timer, stage_timer;
Bool paused=False;
float gate_position[3];
Bool gate_visible=False;

int score=0, last_score=0;
FILE *config_stream;


#define STAGE_MAX 10000


char* stage_name[STAGE_MAX];
int stage_top=0;
int stage_current=-1;
Bool stage_random=False;


/* //// FUNCTIONS  */


int file_exists(char* fname);

void stage_names_load(FILE* conf);

void scene_get_range(struct Graph* sptr, 
		 float vmin[3], float vmax[3]);

void token_try_collect();

void token_positions_init();

void gate_init();

/* ///// TRAVELER */

void traveler_init(struct Traveler * traveler);

void traveler_move(float x, float y, float z, struct Traveler * traveler);

void traveler_h_rotate(int angle, struct Traveler * traveler);

void traveler_v_rotate(int angle, struct Traveler * traveler);

void traveler_upright(struct Traveler * traveler);

/* ////// SCREEN */


void screen_init(struct Screen* scr);

void screen_set_distance(struct Screen* scr,float distance);

void screen_set_clipping(struct Screen* scr,float clip_min, float clip_max);

/* /// VECTOR OPERATIONS */

void float_zeroes(int n, float v[]);

void double_zeroes(int n, double v[]);

Bool vector_eq(float  a[], float b[]);

void vector_add(float a[], float b[], float result[]);

void vector_sub(float a[], float b[], float result[]);

void vector_scale( float s, float v[]);

void vectorcpy(float dst[], float src[]);

void vector_product(float a[], float b[], float result[]);

float scalar_product(float a[], float b[]);

void vector_normalize(float v[]);

/* ///// VECTOR I/O */

void vector_fprintf(FILE *stream, float v[3]);

void vector_fscanf(FILE *stream, float v[3]);

/* //// LIGHT */

void set_light(float light[16]);

/* //// EDGE */

void triangle_normal_vector(float a[], float b[], float c[], float norm[]);

/* ///// LOADING */

void graph_free(struct Graph * gptr);

void graph_init(struct Graph * gptr);

void config_next_scene();

int scene_load(
	       char* fname,
	       struct Graph * gptr,
	       float light[3],
	       int* background,
	       struct Traveler *travptr
	       );

int graph_load(
	       char* fname,
	       struct Graph * gptr

	       );

void graph_fscanf(
		 FILE *stream,
		 struct Graph * gptr
		 );


void remaining_fscanf(
		FILE* stream,
		float light[3],
		int* background,
		struct Traveler *travptr
		);

/* //////////// X STUFF /////// */


void initglx();

/* ///////////////////////////////////////////////////////////////////////////////// */

void mainloop();

/* ////// CALLBACKS */

void callbackExpose( XExposeEvent* evptr);


/* ////////////// KEY COMMANDS //////////// */

void callbackKeyPress( XKeyEvent* evptr);

/* ///////////////  */

void setfrustum();

/* //// REDRAW ///////// */

void graph_to_display_list(GLuint list, struct Graph *gptr);


void redraw();


/* ////////// MAIN */

void help_keys();

int  main(int argc, char *argv[]);

