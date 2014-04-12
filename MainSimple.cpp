/* Christina Taggart
 * Program 3: Lighting
 * CSC 471 with Professor Zoe Wood
 * Description: OpenGL, glut and GLSL application
 * Reads in a mesh file, and stores data in a VBO
 * Draws the data, uses glm for matrix transforms
 * Shaders implement Gouraud and full Phong shading
 * User may manipulate specified light variables through the keyboard
 * This program uses base code provided by I. Dunn and Z. Wood  
 * (original .m loader by H. Hoppe)
*/

#include <iostream>

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif

#ifdef __unix__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#pragma comment(lib, "glew32.lib")

#include <GL\glew.h>
#include <GL\glut.h>
#endif

#include "CMeshLoaderSimple.h"
#include <stdlib.h>
#include <stdio.h>
#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

using namespace std;

//position and color data handles
GLuint triBuffObj, colBuffObj, normBuffObj, iboBuffObj;

//flag and ID to toggle on and off the shader
int shade = 1;
int ShadeProg;
int TriangleCount;
static float g_width, g_height;
float g_Camtrans = -2.5;
float g_Camangle = 0;
glm::vec3 g_trans(0);

//transform variables
float g_angle = 0;
int g_startx, g_starty, g_endx, g_endy;
int g_scale_mode, g_trans_mode, g_rot_mode, g_track;
float g_sf = 1.0;
glm::vec3 g_axis(0);
glm::mat4 allRot(1.0);
glm::mat4 restartMatrix; 

//will allow user to change lighting parameters
float material = 0.1;
glm::vec3 lightPosition(1.0, 1.0, 1.0); 

//Handles to the shader data
GLint h_aPosition;
GLint h_aColor;
GLint h_aNormal;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_aViewPosition;
GLint h_uRotMat;
GLint h_uMaterial;
GLint h_uLightPosition;


/* initialize the geomtry (including color) */   
void InitGeom() {
  CMeshLoader::loadVertexBufferObjectFromMesh("Models/tyra_100k.m", TriangleCount, triBuffObj, colBuffObj, normBuffObj, iboBuffObj);
}


/* projection matrix */
void SetProjectionMatrix() {
  glm::mat4 Projection = glm::perspective(90.0f, (float)g_width/g_height, 0.1f, 100.f);
  safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
}


/* camera controls - do not change */
void SetView() {
  glm::vec3 viewPosition = glm::vec3(0.0, 0.0, g_Camtrans);	
  glm::mat4 Trans = glm::translate( glm::mat4(1.0f), viewPosition);
  glm::mat4 RotateX = glm::rotate( Trans, g_Camangle, glm::vec3(0.0f, 1, 0));
  safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(RotateX));
  safe_glUniform3f(h_aViewPosition, viewPosition.x, viewPosition.y, viewPosition.z);
}


/* model transforms */
void SetModel() {
  glm::mat4 Trans = glm::translate( glm::mat4(1.0f), g_trans);
  glm::mat4 Scale = glm::scale( glm::mat4(1.0f), glm::vec3(g_sf));
  if (g_track) {
    glm::mat4 newRot = glm::rotate( glm::mat4(1.0f), g_angle, g_axis);
    allRot = newRot*allRot;
  }
  glm::mat4 Model = Trans*Scale*allRot;
  safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(Model));
  safe_glUniformMatrix4fv(h_uRotMat, glm::value_ptr(allRot));
  safe_glUniform1f(h_uMaterial, material);
  safe_glUniform3f(h_uLightPosition, lightPosition.x, lightPosition.y, lightPosition.z);
}


/* set the model transform to the identity */
void SetModelI() {
  glm::mat4 tmp = glm::mat4(1.0f); 
  safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(tmp));

}


/*function to help load the shader  - note current version only loading a vertex shader */
int InstallShader(const GLchar *vShaderName, const GLchar *fShaderName) {
	GLuint VS; //handles to shader object
	GLuint FS; //handles to frag shader object
	GLint vCompiled, fCompiled, linked; //status of shader
	
	VS = glCreateShader(GL_VERTEX_SHADER);
	FS = glCreateShader(GL_FRAGMENT_SHADER);

	//load the source
	glShaderSource(VS, 1, &vShaderName, NULL);
	glShaderSource(FS, 1, &fShaderName, NULL);
	
	//compile shader and print log
	glCompileShader(VS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
	printShaderInfoLog(VS);

    //compile shader and print log
    glCompileShader(FS);
    /* check shader status requires helper functions */
    printOpenGLError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, &fCompiled);
    printShaderInfoLog(FS);

	if (!vCompiled || !fCompiled) {
		printf("Error compiling either shader %s or %s", vShaderName, fShaderName);
		return 0;
	}
	 
	//create a program object and attach the compiled shader
	ShadeProg = glCreateProgram();
	glAttachShader(ShadeProg, VS);
	glAttachShader(ShadeProg, FS);
	
	glLinkProgram(ShadeProg);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
	printProgramInfoLog(ShadeProg);

	glUseProgram(ShadeProg);
	
	/* get handles to attribute data */
	h_aPosition = safe_glGetAttribLocation(ShadeProg, "aPosition");
	h_aColor = safe_glGetAttribLocation(ShadeProg,	"aColor");
	h_aNormal = safe_glGetAttribLocation(ShadeProg,	"aNormal");
	h_aViewPosition = safe_glGetAttribLocation(ShadeProg, "aViewPosition");
	h_uRotMat = safe_glGetUniformLocation(ShadeProg, "uRotMat");
    h_uProjMatrix = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
    h_uViewMatrix = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
    h_uModelMatrix = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
    h_uMaterial = safe_glGetUniformLocation(ShadeProg, "uMaterial");
    h_uLightPosition= safe_glGetUniformLocation(ShadeProg, "ulPos");

	printf("sucessfully installed shader %d\n", ShadeProg);
	return 1;
	
}


/* Some OpenGL initialization */
void Initialize ()					// Any GL Init Code 
{
	// Start Of User Initialization
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);								
	// Black Background
 	//glClearDepth (1.0f);	// Depth Buffer Setup
 	//glDepthFunc (GL_LEQUAL);	// The Type Of Depth Testing
	glEnable (GL_DEPTH_TEST);// Enable Depth Testing

}


/* Main display function */
void Draw (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Start our shader	
 	glUseProgram(ShadeProg);

        /* only set the projection and view matrix once */
        SetProjectionMatrix();
        SetView();

        SetModel();
	
	//data set up to access the vertices, color, normal, and ibo
  	safe_glEnableVertexAttribArray(h_aPosition);
 	glBindBuffer(GL_ARRAY_BUFFER, triBuffObj);
	safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); //was 4!

  	safe_glEnableVertexAttribArray(h_aColor);
 	glBindBuffer(GL_ARRAY_BUFFER, colBuffObj);
	safe_glVertexAttribPointer(h_aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, normBuffObj); 
	safe_glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBuffObj);
    
	//actually draw the data
	glDrawElements(GL_TRIANGLES, TriangleCount, GL_UNSIGNED_INT, 0);//was array, *3
	//clean up 
	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aColor);
	safe_glDisableVertexAttribArray(h_aNormal);
	//disable the shader
	glUseProgram(0);	
	glutSwapBuffers();
}


/* Reshape */
void ReshapeGL (int width, int height)
{
        g_width = (float)width;
        g_height = (float)height;
        glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));

}


//the keyboard callback to change the values to the transforms
void keyboard(unsigned char key, int x, int y ){
  switch( key ) {
    /* WASD keys effect view/camera transform */
    case 'w':
      g_Camtrans += 0.1;
      break;
    case 's':
      g_Camtrans -= 0.1;
      cout << " Cam trans " << g_Camtrans << endl;
      break;
    case 'a':
      g_Camangle += 1;
      break;
    case 'd':
      g_Camangle -= 1;
      break;
    /* NMJK keys effect object/image translation */
    case 'm':
	  g_trans.x += .1;
	  break;
	case 'n':
	  g_trans.x -= .1;
	  break;
	case 'k':
	  g_trans.y += .1;
	  break;
	case 'j':
	  g_trans.y -= .1;
	  break;
	/* '+' and e keys effect object/image scale and rotation, respectively */
    case '+':
      g_scale_mode = !g_scale_mode;
      break;
    /* default */
    case 'e':
	  g_trans = glm::vec3(0);
	  g_sf = 1.0;
	  allRot = glm::mat4(1.0);	      
	  break;
	/* PO keys effect the object/image's material brightness */
	case 'p':
	  material += .01;
	  break;
	case 'o':
	  if(material > 0){
	  	material -= .01;
	  }
	  break;
	/* XZCV keys effect the position of the light */
	case 'x':
	  lightPosition.x += .1;
	  break;
	case 'z':
	  lightPosition.x -= .1;
	  break;
	case 'c':
	  lightPosition.y += .1;
	  break;
	case 'v':
	  lightPosition.y -= .1;
	  break;
	/* r key resets the mesh back to world origin with original size and orientation */
	case 'r':
		allRot = restartMatrix;
		g_sf = 1.0;
		g_trans.x = 0.0;
    	g_trans.y = 0.0;
    	break;
    case 'q': case 'Q' :
      exit( EXIT_SUCCESS );
      break;
  }
  glutPostRedisplay();
}


/*2D coord transforms - slightly off, but fine for user interaction for p2 */
float p2i_x(int p_x) {
  float x_i = ( (float)p_x - ((g_width-1.0)/2.0) )*2.0/g_width;
  return(x_i);
}

float p2i_y(int p_y) {
  return( ( (float)p_y - ((g_height-1.0)/2.0) )*2.0/g_height);
}

float p2w_x(int p_x) {
  float x_i = ( (float)p_x - ((g_width-1.0)/2.0) )*2.0/g_width;
  return(((float)g_width/(float)g_height)*x_i);
}

float p2w_y(int p_y) {
  return( ( (float)p_y - ((g_height-1.0)/2.0) )*2.0/g_height);
}


void mouse(int button, int state, int x, int y) {
      if (state == GLUT_DOWN) {
          g_startx = x;
          g_starty = g_height-y-1;
      	  if (button == GLUT_LEFT_BUTTON && !g_scale_mode) { /* if the left button is clicked */
        	g_scale_mode = 0;
        	g_rot_mode = g_track = 1;
        	g_trans_mode =0;
      	  } 
          if (button == GLUT_LEFT_BUTTON && g_scale_mode) {       	
        	g_scale_mode = 1;
        	g_rot_mode = g_track = g_trans_mode =0;
      	  } else if (button == GLUT_RIGHT_BUTTON) {
        	g_scale_mode = 0 ;
        	g_rot_mode = g_track = 0;
        	g_trans_mode =1;
          }
      } else if (state == GLUT_UP) {
       	g_scale_mode = g_trans_mode = g_rot_mode = g_track = 0;
      }
}


void TrackBall(void);


void mouseMove(int x, int y) {
  g_endx = x;
  g_endy = g_height-y-1;
    
  float startWX = p2w_x(g_startx);
  float startWY = p2w_y(g_starty);
  float endWX = p2w_x(g_endx);
  float endWY = p2w_y(g_endy);
  if (g_rot_mode) {
    g_track = 1;
    TrackBall();
  } else if (g_scale_mode) {
    if ( g_endx > g_startx) {
      if (g_sf <2.2)
        g_sf += .1;
      else
        g_sf = 2.2;
    }else {
      if (g_sf > 0.3)
        g_sf -= .1;
      else
        g_sf = 0.3;
    }
  } else if (g_trans_mode) {
    g_trans.x += endWX - startWX;
    g_trans.y += endWY - startWY;
  }
  g_startx = g_endx;
  g_starty = g_endy;
  glutPostRedisplay();
}


void TrackBall() {

        float startx_i, starty_i, endx_i, endy_i;
        glm::vec3 u, v;

        //first set to image coordinates
        startx_i = p2i_x(g_startx);
        starty_i = p2i_y(g_starty);
        endx_i = p2i_x(g_endx);
        endy_i = p2i_y(g_endy);

        //first project the start  to the unit sphere
        float r = 1.0 - startx_i*startx_i - starty_i*starty_i;
        if (r > 0) {
                u.x = startx_i; u.y = starty_i; u.z = sqrt(r);
        } else { //point outside of ball
                float d = sqrt(startx_i*startx_i + starty_i*starty_i);
                assert(d != 0);
                u.x = startx_i/d; u.y = starty_i/d; u.z = 0.0;
        }

        //then project the end to the unit sphere
        r = 1.0 - endx_i*endx_i - endy_i*endy_i;
        if (r > 0) {
                v.x = endx_i; v.y = endy_i; v.z = sqrt(r);
        } else { //point outside of ball
                float d = sqrt(endx_i*endx_i + endy_i*endy_i);
                assert(d != 0);
                v.x = endx_i/d; v.y = endy_i/d; v.z = 0.0;
        }

        //compute the axis of rotation
        g_axis = glm::cross(u, v);
        //compute the angle of rotation (damped)
        g_angle = (180/3.14*acos(glm::dot(u, v)))/1.0;

  if (g_angle < 0 || g_angle > 90)
    g_angle = 0;
  /* should in clude isnan() checks on all values but doesn't */
/*
  if( isnan(g_axis.x) || isnan(g_axis.y) || isnan(g_axis.z)  ){ 
  		g_axis.x = 0.0;
  		g_axis.y = 0.0;
  		g_axis.z = 1.0;
  } 
  if(isnan(g_angle) ){
  		g_angle = 0.0; 
  }
  */
}


int main( int argc, char *argv[] )
{
   	glutInit( &argc, argv );
   	glutInitWindowPosition( 200, 200 );
   	glutInitWindowSize( 800, 800 );
   	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   	glutCreateWindow("My first mesh");
   	glutReshapeFunc( ReshapeGL );
   	glutDisplayFunc( Draw );
    glutKeyboardFunc( keyboard );
    glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	g_width = g_height = 800;

#ifdef _WIN32 
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
		return 1;
	}
#endif

   	Initialize();
	
	//test the openGL version
	getGLversion();
	//install the shader
	if (!InstallShader(textFileRead((char *)"mesh_vert.glsl"), (textFileRead((char *)"mesh_frag.glsl")) ) ){
		printf("Error installing shader!\n");
		return 0;
	}
		
	InitGeom();

	//added for transformations
	g_startx = g_endx = g_starty = g_endy = 0;
  	g_angle = 0.0;
  	g_track = 0;
  	g_sf = 1.0;
  	g_scale_mode = g_rot_mode =0;
	g_trans_mode = 1;

  	glutMainLoop();
   	return 0;
}
