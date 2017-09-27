// CS370 - Fall 2016
// Assign02 - Rollin Train

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Shader file utility functions
#include "shaderutils.h"
#include "train.h"

// Shader files
GLchar* vertexFile = "basicvert.vs";
GLchar* fragmentFile = "basicfrag.fs";

// Shader objects
GLuint shaderProg;

#define RAD2DEG (180.0f/3.14159f)
#define DEG2RAD (3.14159f/180.0f)

// View modes
#define ORTHOGRAPHIC 0
#define PERSPECTIVE 1
int proj = ORTHOGRAPHIC;

// Component indices
#define X 0
#define Y 1
#define Z 2

// Display list identifiers
#define CUBE 1
#define ENGINE 2
#define TRACKS 3
#define WHEEL 4


// Define globals
GLfloat eye[3] = { 2.0f, 2.0f, 2.0f };
GLfloat at[3] = { 0.0f, 0.0f, 0.0f };
GLfloat up[3] = { 0.0f, 1.0f, 0.0f };

GLint animationState = -1;
GLfloat trainMoveScalar = -0.005f;
GLint TRAIN_Z_OFFSET = 2;
#define NUM_TRACKS 18

GLfloat viewAngle = 2.0f;
GLfloat viewAngleIncr = 2.0f;

GLfloat elevationAngle = 45.0f;
GLfloat elevationAngleIncr = 2.0f;

GLfloat radius = 5.0f;
GLfloat radiusIncr = 0.1f;



GLfloat cube[8][3] = { 
	{ -1.0f,-1.0f,-1.0f },
	{ 1.0f,-1.0f,-1.0f },
	{ 1.0f,-1.0f,1.0f }, 
	{ -1.0f,-1.0f,1.0f },
	{ -1.0f,1.0f,-1.0f },
	{ 1.0f,1.0f,-1.0f }, 
	{ 1.0f,1.0f,1.0f },
	{ -1.0f,1.0f,1.0f } 
};

// Function Prototypes
void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void reshape(int w, int h);
void idlefunc();
void create_lists();

void drawSquare(GLfloat p1[], GLfloat p2[], GLfloat p3[], GLfloat p4[]);
void drawCube();
void drawWheel();
void toggleAnimation();

int main(int argc, char* argv[])
{
	// Initialize glut
	glutInit(&argc, argv);

	// Initialize window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("RailRoad");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);
	glutReshapeFunc(reshape);

	// Create display lists
	create_lists();

	// Set background color
	glClearColor(1.0f,1.0f,1.0f,1.0f);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Load shader programs
	shaderProg = load_shaders(vertexFile,fragmentFile);
	glUseProgram(shaderProg);

	// Start graphics loop
	glutMainLoop();

	return 0;
}

// Display callback
void display()
{
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set initial orthographic projection matrix
	if (proj == ORTHOGRAPHIC)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-15.0, 15.0, -15.0, 15.0, -15.0, 15.0);

		// Set modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Set orthographic camera
		gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);
	} else {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);

		// Set modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Set PERSPECTIVE camera
		gluLookAt(0, 1.1f, TRAIN_Z_OFFSET + 1.5f, at[X], at[Y] + 2, TRAIN_Z_OFFSET - 10, up[X], up[Y], up[Z]);
	}
	// Render scene
	render_Scene();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}
void drawTower() {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -10.0f);
		glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
		glScalef(2.5f, 2.5f, 2.5f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glCallList(CUBE);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f, 4.5f, -10.0f);
		glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
		glScalef(2.0f, 2.0f, 2.0f);
		glColor3f(0.3f, 0.2f, 0.6f);
		glCallList(CUBE);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f, 7.5f, -10.0f);
		glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glColor3f(1.0f, 0.4f, 1.0f);
		glCallList(CUBE);
	glPopMatrix();
}

void drawTracks() {
	for (int i = 0; i < NUM_TRACKS; i++) {
		glPushMatrix();
			glTranslatef(0.0f, -2.7f, (i - 6.2f));
			glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
			glCallList(TRACKS);
		glPopMatrix();
	}
}
void drawChooChoo() {
	// train engine 
	glPushMatrix();
		glTranslatef(0.0f, -1.0f, TRAIN_Z_OFFSET);
		glCallList(ENGINE);
	glPopMatrix();

	// wheels
	glPushMatrix();
		glTranslatef(0.6f, -2.0, TRAIN_Z_OFFSET - 1.5);
		glRotatef(90, 0.0f, 1.0f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		glCallList(WHEEL);
	glPopMatrix();
}

// Scene render function
void render_Scene()
{
	// cube tower
	drawTower();
	
	// tracks
	drawTracks();

	// chugga chugga choo choo
	drawChooChoo();
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{

	switch (key) {
		case 'o':
			proj = ORTHOGRAPHIC;
			break;
		case 'p':
			proj = PERSPECTIVE;
			break;
		case 27:
			exit(0);
			break;
		case ' ':
			toggleAnimation();
			break;
		case 'w':
			elevationAngle -= elevationAngleIncr;
			break;
		case 'a':
			viewAngle += viewAngleIncr;
			break;
		case 's':
			elevationAngle += elevationAngleIncr;
			break;
		case 'd': 
			viewAngle -= viewAngleIncr;
			break;
	}

	eye[X] = (GLfloat)(radius*sin(viewAngle*DEG2RAD)*sin(elevationAngle*DEG2RAD));
	eye[Y] = (GLfloat)(radius*cos(elevationAngle*DEG2RAD));
	eye[Z] = (GLfloat)(radius*cos(viewAngle*DEG2RAD)*sin(elevationAngle*DEG2RAD));

	// Redraw scene
	glutPostRedisplay();
}

// Reshape callback
void reshape(int w, int h)
{
	// Set new screen extents
	glViewport(0,0,w,h);
}

// Idle callback
void idlefunc()
{
	if (animationState > 0) {
		if (trainMoveScalar > 0 && TRAIN_Z_OFFSET > 10) {
			trainMoveScalar *= -1;
			glutPostRedisplay();
		} else if (trainMoveScalar > 0 && TRAIN_Z_OFFSET < -5) {
			trainMoveScalar *= -1;
			glutPostRedisplay();
		}
	}
}

// Routine to create display lists
void create_lists()
{
	// cube list
	glNewList(CUBE, GL_COMPILE);
		drawCube();
	glEndList();

	// tracks list 
	glNewList(TRACKS, GL_COMPILE);
		glColor3f(0.0f, 0.0f, 0.0f);
		glScalef(1.0, .1f, .1f);
		drawCube();
		//glColor3f(.5, .5, .5);
		glScalef(.1f, 1.0f, 10.0f);
		glTranslatef(-5.0f, 1.0f, 0.0f);
		drawCube();
		glTranslatef(10.0f, 0.0f, 0.0f);
		drawCube();
	glEndList();

	// train engine list
	glNewList(ENGINE, GL_COMPILE);
		// engine
		glColor3f(1.0f, 0.0f, 0.0f);
		glScalef(0.5f, 1.0f, 2.0f);
		drawCube();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 1.5f, 0.5f);
		glScalef(1.0f, 0.5f, -0.5f);
		drawCube();

		// steam thingy
		glColor3f(1.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.0f, 0.5f);
		glTranslatef(0.0f, 0.0f, 5.0f);
		drawCube();
		glTranslatef(0.0f, 2.2f, 0.0f);
		glRotatef(90, 1.0, 0.0, 0.0);
		glutSolidCone(1.0, 2.0, 30, 40);
	glEndList();

	// train wheels list 
	glNewList(WHEEL, GL_COMPILE);
		// left
		glColor3f(0.0f, 0.0f, 0.0f);
		drawWheel();
		glTranslatef(-3.5f, 0.0f, 0.0f);
		drawWheel();
		glTranslatef(-2.5f, 0.0f, 0.0f);
		drawWheel();
		// right
		glTranslatef(0.0f, 0.0f, -2.4f);
		drawWheel();
		glTranslatef(2.5f, 0.0f, 0.0f);
		drawWheel();
		glTranslatef(3.5f, 0.0f, 0.0f);
		drawWheel();
	glEndList();
}

void drawSquare(GLfloat p1[], GLfloat p2[], GLfloat p3[], GLfloat p4[]) {
	// square 
	glBegin(GL_POLYGON);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
	glEnd();
	// outline 
	glBegin(GL_LINE_LOOP);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
	glEnd();
}

void drawCube() {
	drawSquare(cube[0], cube[1], cube[2], cube[3]);
	drawSquare(cube[0], cube[3], cube[7], cube[4]);
	drawSquare(cube[0], cube[4], cube[5], cube[1]);
	drawSquare(cube[1], cube[5], cube[6], cube[2]);
	drawSquare(cube[2], cube[6], cube[7], cube[3]);
	drawSquare(cube[4], cube[7], cube[6], cube[5]);
}

void drawWheel() {
	glutSolidTorus(.6, .6, 20, 20);
}

void toggleAnimation() {
	animationState *= -1;
}
