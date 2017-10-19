// CS370 - Fall 2014
// Final Project

#ifdef OSX
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lighting.h"
#include "materials.h"

// Shader file utility functions
#include "shaderutils.h"


// CUSTOM OBJECT HEADERS
#include "rectangle.h"

#define X 0
#define Y 1
#define Z 2

#define RAD2DEG (180.0f/3.14159f)
#define DEG2RAD (3.14159f/180.0f)

// Global camera vectors
GLfloat eye[3] = { 0.0f,0.1f,1.0f };
GLfloat at[3] = { 0.0f,-2.5f,0.0f };
GLfloat up[3] = { 0.0f,1.0f,0.0f };

// CAMERA MANIPULATION VARIABLES
GLfloat theta = 0;

// Global screen dimensions
GLfloat ww, hh;


// Shader files
GLchar* defaultVertexFile = "defaultvert.vs";
GLchar* defaultFragmentFile = "defaultfrag.fs";
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* texVertexFile = "texturevert.vs";
GLchar* texFragmentFile = "texturefrag.fs";

// Shader objects
GLuint defaultShaderProg;
GLuint lightShaderProg;
GLuint textureShaderProg;
GLuint numLights_param;
GLint numLights = 1;
GLint texSampler;

void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void idlefunc();
void reshape(int w, int h);
void create_lists();

// DISPLAY LIST IDS
#define WALLS 1
#define DESK 2
#define WINDOW 3
#define MIRROR 4
#define DOOR 5
#define CARPET 6

GLfloat outlineColor[3] = { 0.0f, 0.0f, 0.0f };
GLfloat wallColors[][3] = {
		{0.921f, 0.921f, 0.921f},
		{ 0.921f, 0.921f, 0.921f },
		{0.921f, 0.0f, 0.0f},
		{0.921f, 0.0f, 0.0f},
		{0.047f, 0.0f, 0.921f},
		{0.047f, 0.0f, 0.921f}
};
GLfloat deskColor[3] = { 1.0f, 0.894f, 0.058f };
GLfloat windowColor[3] = { 0.0f, 0.941f, 0.047f };
GLfloat mirrorColor[3] = { 0.780f, 0.780f, 0.780f };
GLfloat doorColor[3] = { 0.490f, 0.345f, 0.231f };
GLfloat carpetColor[3] = { 1.0f, 0.121f, 0.956f };

GLfloat roof[][3] = { { -4.0f, 2.0f, -4.0f },{ 4.0f, 2.0f, -4.0f },{ 4.0f, 2.0f, 4.0f },{ -4.0f, 2.0f, 4.0f } };
GLfloat floor_rect[][3] = { { -4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat back_wall[][3] = { { -4.0f, 2.0f, -4.0f },{ 4.0f, 2.0f, -4.0f } ,{ 4.0f, -2.0f, -4.0f } ,{ -4.0f, -2.0f, -4.0f } };
GLfloat front_wall[][3] = { { -4.0f, 2.0f, 4.0f },{ 4.0f, 2.0f, 4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat right_wall[][3] = { { -4.0f, 2.0f, 4.0f },{ -4.0f, 2.0f, -4.0f },{ -4.0f, -2.0f, -4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat left_wall[][3] = { { 4.0f, 2.0f, 4.0f },{ 4.0f, 2.0f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f } };

int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Set the window size to image size
	glutInitWindowSize(512, 512);

	// Create window
	glutCreateWindow("House");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);
	glutReshapeFunc(reshape);

	create_lists();

	// Set background color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Load shader programs
	defaultShaderProg = load_shaders(defaultVertexFile, defaultFragmentFile);
	lightShaderProg = load_shaders(lightVertexFile, lightFragmentFile);
	textureShaderProg = load_shaders(texVertexFile, texFragmentFile);
	numLights_param = glGetUniformLocation(lightShaderProg, "num_lights");
	texSampler = glGetUniformLocation(textureShaderProg, "texMap");
	glUseProgram(defaultShaderProg);

	// Begin event loop
	glutMainLoop();
	return 0;
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
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], eye[X] + cos(DEG2RAD * theta), at[Y] + 2.5f, eye[Z] + sin(DEG2RAD * theta), up[X], up[Y], up[Z]);

	// Render scene
	render_Scene();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

// Scene render function
void render_Scene()
{
	glPushMatrix();
		glCallList(WALLS);
	glPopMatrix();

	glPushMatrix();
		glCallList(DESK);
	glPopMatrix();

	glPushMatrix();
		glCallList(WINDOW);
	glPopMatrix();

	glPushMatrix();
		glCallList(MIRROR);
	glPopMatrix();

	glPushMatrix();
		glCallList(DOOR);
	glPopMatrix();

	glPushMatrix();
		glCallList(CARPET);
	glPopMatrix();
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// <esc> quits
	if (key == 27)
	{
		exit(0);
	}

	if (key == 'a')
	{
		theta -= 15;
	}
	else if (key == 'd')
	{
		theta += 15;
	}
	if (key == 'w')
	{ 

		eye[X] += cos(DEG2RAD * theta) * 0.05f;
		eye[Z] += sin(DEG2RAD * theta) * 0.05f;
		if (eye[X] > 1.5f) {
			eye[X] = 1.5f;
		}
		if (eye[X] < -1.5f) {
			eye[X] = -1.5f;
		}
		if (eye[Z] > 2.0f) {
			eye[Z] = 2.0f;
		}
		if (eye[Z] < -0.25f) {
			eye[Z] = -0.25f;
		}
	}
	else if (key == 's')
	{
		eye[X] -= cos(DEG2RAD * theta) * 0.05f;
		eye[Z] -= sin(DEG2RAD * theta) * 0.05f;
		if (eye[X] > 1.5f) {
			eye[X] = 1.5f;
		}
		if (eye[X] < -1.5f) {
			eye[X] = -1.5f;
		}
		if (eye[Z] > 2.0f) {
			eye[Z] = 2.0f;
		}
		if (eye[Z] < -1.25f) {
			eye[Z] = -1.25f;
		}
	}
	glutPostRedisplay();
}

// Idle callback
void idlefunc()
{

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

void create_lists() {
	glNewList(WALLS, GL_COMPILE);
		renderCube(roof, floor_rect, left_wall, right_wall, front_wall, back_wall, wallColors, outlineColor);
	glEndList();

	glNewList(DESK, GL_COMPILE);
		glTranslatef(0.0f, -1.9f, -3.9f);
		glScalef(2.2f, 1.5f, 1.8f);
		renderCube(deskColor, outlineColor);
	glEndList();

	glNewList(WINDOW, GL_COMPILE);
		glTranslatef(-3.9f, 0.25f, 0.0f);
		glScalef(0.01f, 1.0f, 1.78f);
		renderCube(windowColor, outlineColor);
	glEndList();

	glNewList(MIRROR, GL_COMPILE);
		glTranslatef(3.9f, 0.25f, 0.0f);
		glScalef(0.01f, 1.0f, 1.75f);
		renderCube(mirrorColor, outlineColor);
	glEndList();

	glNewList(DOOR, GL_COMPILE);
		glTranslatef(2.25f, -0.25f, 4.0f);
		glScalef(1.0f, 1.9f, 0.01f);
		renderCube(doorColor, outlineColor);
	glEndList();

	glNewList(CARPET, GL_COMPILE);
		glTranslatef(0.0f, -2.0f, 0.0f);
		glScalef(2.5f, 0.01f, 2.5f);
		renderCube(carpetColor, outlineColor);
	glEndList();
}