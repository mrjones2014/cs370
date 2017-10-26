// CS370 - Fall 2016
// Assign03 - Limelight

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "materials.h"
#include "lighting.h"
#include "vectorops.h"

// Shader file utility functions
#include "shaderutils.h"

// Shader files
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* defaultVertexFile = "basicvert.vs";
GLchar* defaultFragmentFile = "basicfrag.fs";

// Shader objects
GLuint lightShaderProg;
GLuint defaultShaderProg;
GLuint numLights_param;
GLint numLights = 1;

#define DEG_TO_RAD 0.0175f
#define X 0
#define Y 1
#define Z 2
#define N 8
#define STAGE 1
#define CUBE 2
#define SPHERE 3
#define DIAMOND 4

// Global variables
GLenum lights[4] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3 };

// Light0 (point) Parameters
GLfloat light0_pos[] = { 0.0f, 5.0f, 0.0f, 1.0f };

//light1 (spot) paramaters
GLfloat light1_pos[] = { -5.0f, 5.0f, 0.0f, 1.0f };
GLfloat light1_dir[] = { 0.0f, -1.0f, 0.0f };
GLfloat light1_cutoff = 30.0f;
GLfloat light1_exp = 0.0f;

//light2 (spot) paramaters
GLfloat light2_pos[] = { 0.0f, 5.0f, 0.0f, 1.0f };
GLfloat light2_dir[] = { 0.0f, -1.0f, 0.0f };
GLfloat light2_cutoff = 30.0f;
GLfloat light2_exp = 0.0f;

//light3 (spot) paramaters
GLfloat light3_pos[] = { 5.0f, 5.0f, 0.0f, 1.0f };
GLfloat light3_dir[] = { 0.0f, -1.0f, 0.0f };
GLfloat light3_cutoff = 30.0f;
GLfloat light3_exp = 0.0f;

GLfloat cube[][3] = { { -1.0f,-1.0f,-1.0f },{ 1.0f,-1.0f,-1.0f },{ 1.0f,-1.0f,1.0f },
{ -1.0f,-1.0f,1.0f },{ -1.0f,1.0f,-1.0f },{ 1.0f,1.0f,-1.0f },
{ 1.0f,1.0f,1.0f },{ -1.0f,1.0f,1.0f } };

GLfloat stage[][3] = { { -8.0f, 1.0f, -4.0f },{ -8.0f, 1.0f, 4.0f },{ 8.0f, 1.0f, 4.0f },{ 8.0f, 1.0f, -4.0f },
					{ -8.0f, -1.0f, 4.0f },{ -8.0f, -1.0f, -4.0f },{ 8.0f, -1.0f, -4.0f },{ 8.0f, -1.0f, 4.0f } };

GLUquadricObj *quadric;

// Global subdivision parameters
int div_level = 6;

// Shadow matrices
GLfloat M1_s[16];
GLfloat M2_s[16];
GLfloat M3_s[16];

// Global animation variables
GLint time = 0;
GLint lasttime = 0;
GLint fps = 30;

// Global camera vectors
GLfloat eye[3] = {1.0f,1.0f,1.0f};
GLfloat at[3] = {0.0f,0.0f,0.0f};
GLfloat up[3] = {0.0f,1.0f,0.0f};

// Global screen dimensions
GLfloat ww,hh;

void display();
void render_Scene(bool shadow);
void reshape(int w, int h);
void keyfunc(unsigned char key, int x, int y);
void idlefunc();

void colorcube();
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[]);
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], int n);
void smooth_stage();
void create_lists();

int main(int argc, char* argv[])
{
	// Initialize glut
    glutInit(&argc, argv);

	// Initialize window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Limelight");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);

	// Define callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);

	// Set background color to grey
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	// Enable depth test
    glEnable(GL_DEPTH_TEST);

	// Load default shader programs
	defaultShaderProg = load_shaders(defaultVertexFile,defaultFragmentFile);
	// Load lighting shader programs
	lightShaderProg = load_shaders(lightVertexFile,lightFragmentFile);
	// Associate num_lights parameter
	numLights_param = glGetUniformLocation(lightShaderProg,"numLights");

	create_lists();

	// Start graphics loop
	glutMainLoop();
}

// Display callback
void display()
{
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Adjust viewing volume (orthographic)
	GLfloat xratio = 1.0f;
	GLfloat yratio = 1.0f;
	// If taller than wide adjust y
	if (ww <= hh)
	{
		yratio = (GLfloat)hh / (GLfloat)ww;
	}
	// If wider than tall adjust x
	else if (hh <= ww)
	{
		xratio = (GLfloat)ww / (GLfloat)hh;
	}
	glOrtho(-10.0f*xratio, 10.0f*xratio, -10.0f*yratio, 10.0f*yratio, -10.0f, 10.0f);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);

	// Render scene without shadows
	render_Scene(false);

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

void render_Scene(bool shadow)
{
	// Set point light source fixed in world
	set_PointLight(GL_LIGHT0, &white_light, light0_pos);

	// Set number of lights
	glUseProgram(lightShaderProg);
	glUniform1i(numLights_param,numLights);
	// Placeholder stage object
	//glPushMatrix();
	//	set_material(GL_FRONT_AND_BACK, &brass);
	//	glScalef(16.0, 2.0, 8.0);
	//	glutSolidCube(1.0);
	//glPopMatrix();

	glPushMatrix();
		glCallList(STAGE);
	glPopMatrix();
}

// Reshape callback
void reshape(int w, int h)
{
	// Set new screen extents
	glViewport(0, 0, w, h);

	// Store new extents
	ww = w;
	hh = h;
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Keypress functionality

	// Esc to quit
	if (key==27)
	{
		exit(0);
	}

}

// Idle callback
void idlefunc()
{
	// Time-based Animations

}

void colorcube()
{
	// Top face
	div_quad(cube[4], cube[7], cube[6], cube[5], div_level);

	// Bottom face
	div_quad(cube[0], cube[1], cube[2], cube[3], div_level);

	// Left face
	div_quad(cube[0], cube[3], cube[7], cube[4], div_level);

	// Right face
	div_quad(cube[1], cube[5], cube[6], cube[2], div_level);

	// Front face
	div_quad(cube[2], cube[6], cube[7], cube[3], div_level);

	// Back face
	div_quad(cube[0], cube[4], cube[5], cube[1], div_level);
}

// Routine to perform recursive subdivision
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], int n)
{
	GLfloat v1_prime[3], v2_prime[3], v3_prime[3], v4_prime[3], v5_prime[3];

	// Recurse until n = 0
	if (n > 0)
	{
		// TODO: Compute midpoints
		for (int i = 0; i<3; i++)
		{
			v1_prime[i] = (v4[i] + v1[i]) / 2.0f;
			v2_prime[i] = (v1[i] + v2[i]) / 2.0f;
			v3_prime[i] = (v2[i] + v3[i]) / 2.0f;
			v4_prime[i] = (v3[i] + v4[i]) / 2.0f;
			v5_prime[i] = (v1[i] + v2[i] + v3[i] + v4[i]) / 4.0f;
		}

		// TODO: Subdivide polygon
		div_quad(v1, v2_prime, v5_prime, v1_prime, n - 1);
		div_quad(v2_prime, v2, v3_prime, v5_prime, n - 1);
		div_quad(v1_prime, v5_prime, v4_prime, v4, n - 1);
		div_quad(v5_prime, v3_prime, v3, v4_prime, n - 1);
	}
	else
	{
		// TODO: Otherwise render quad
		rquad(v1, v2, v3, v4);
	}
}

// Routine to draw quadrilateral face
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[])
{
	GLfloat normal[3];
	// TODO: Compute normal via cross product
	cross(v1, v2, v4, normal);
	normalize(normal);

	// TODO: Set normal
	glNormal3fv(normal);

	// Draw face 
	glBegin(GL_POLYGON);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glVertex3fv(v4);
	glEnd();
}

void smooth_stage() {
	div_quad(stage[0], stage[1], stage[2], stage[3], div_level);
}

void create_lists() {
	glNewList(STAGE, GL_COMPILE);
		glPushAttrib(GL_CURRENT_BIT);
			glPushMatrix();
				smooth_stage();	
			glPopMatrix();
			glPushMatrix();
				set_material(GL_FRONT_AND_BACK, &brass);
				glTranslatef(0.0, 1.0, 4.0);
				glRotatef(-90.0, 0.0, 1.0, 0.0);
				glRotatef(-90.0, 1.0, 0.0, 0.0);
				glScalef(1.0, 2.0, 1.0);
				gluPartialDisk(quadric, 0, 4, 30, 30, 0, 180);
			glPopMatrix();

			glPushMatrix();
				set_material(GL_FRONT_AND_BACK, &brass);
				glTranslatef(0.0, -1.0, 4.0);
				glRotatef(-90.0, 0.0, 1.0, 0.0);
				glRotatef(-90.0, 1.0, 0.0, 0.0);
				glScalef(1.0, 2.0, 1.0);
				gluCylinder(quadric, 4, 4, 2, 30, 30);
			glPopMatrix();
		glPopAttrib();
	glEndList();
}