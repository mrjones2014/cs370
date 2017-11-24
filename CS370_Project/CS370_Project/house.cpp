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
GLfloat at[3] = { 0.0f,0.0f,0.0f };
GLfloat up[3] = { 0.0f,1.0f,0.0f };

// CAMERA MANIPULATION VARIABLES
GLfloat theta = 0;
unsigned int time = 0;
GLfloat lasttime = 0;
GLfloat fpstime = 0;
GLfloat fps = 30;
GLfloat eyeMinY = 0.1f;
GLfloat atMinY = -1.25f;
GLfloat start_y = 0.0;

// Global screen dimensions
GLint ww, hh;


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
void lookfunc(int x, int y);
void reshape(int w, int h);
void create_lists();
bool load_textures();
void draw_sprite();

void draw_walls();

// DISPLAY LIST IDS
#define WALLS 1
#define DESK 2
#define WINDOW 3
#define MIRROR 4
#define DOOR 5
#define CARPET 6
#define BLINDS 7
#define PAINTING 8
#define BOWL 9
#define WINDOW_BORDER 10
#define FRUIT 11


#define WALL_TEX 0
#define WINDOW_TEX 1
#define DOOR_TEX 2
#define PAINTING_TEX 3
#define BLINDS_TEX 4
#define WOOD_TEX 5
#define CARPET_TEX 6
#define SPRITE_TEX 7
#define SPRITE_TOP_TEX 8
#define FLOOR_TEX 9
#define ROOF_TEX 10 
#define BOWL_TEX 11
#define NO_TEXTURE 12

GLint tex_ids[NO_TEXTURE] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
char tex_files[NO_TEXTURE][30] = { 
	"log.jpg", 
	"world.jpg", 
	"door.jpg", 
	"esao_andrews_painting.jpg", 
	"blinds.jpg", 
	"wood.jpg", 
	"carpet.jpg",
	"sprite.bmp",
	"sprite_top.bmp",
	"floor.jpg",
	"roof.jpg",
	"bowl.bmp"
};

GLfloat outlineColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat wallColors[][4] = {
		{0.921f, 0.921f, 0.921f, 0.5f},
		{ 0.921f, 0.921f, 0.921f, 1.0f },
		{0.921f, 0.0f, 0.0f, 1.0f },
		{0.921f, 0.0f, 0.0f, 1.0f },
		{0.047f, 0.0f, 0.921f, 1.0f },
		{0.047f, 0.0f, 0.921f, 1.0f }
};

GLfloat deskColor[4] = { 1.0f, 0.894f, 0.058f, 1.0f };
GLfloat windowColor[4] = { 0.0f, 0.941f, 0.047f, 1.0f };
GLfloat mirrorColor[4] = { 0.780f, 0.780f, 0.780f, 1.0f };
GLfloat doorColor[4] = { 0.490f, 0.345f, 0.231f, 1.0f };
GLfloat carpetColor[4] = { 1.0f, 0.121f, 0.956f, 1.0f };

GLfloat roof[][3] = { { -4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, 4.0f },{ -4.0f, 3.25f, 4.0f } };
GLfloat floor_rect[][3] = { { -4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat back_wall[][3] = { { -4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, -4.0f } ,{ 4.0f, -2.0f, -4.0f } ,{ -4.0f, -2.0f, -4.0f } };
GLfloat front_wall[][3] = { { -4.0f, 3.25f, 4.0f },{ 4.0f, 3.25f, 4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat right_wall[][3] = { { -4.0f, 3.25f, 4.0f },{ -4.0f, 3.25f, -4.0f },{ -4.0f, -2.0f, -4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat left_wall[][3] = { { 4.0f, 3.25f, 4.0f },{ 4.0f, 3.25f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f } };

GLboolean blindsAnim = false;
GLint blinds_state_incr = -1;
GLint blinds_state = 9;
GLfloat blinds_transition_states[10][2] = {
	{1.45, 0.1},
	{1.3, 0.2},
	{1.15, 0.3},
	{1.0, 0.4},
	{0.85, 0.5},
	{0.7, 0.6},
	{0.55, 0.7},
	{0.4, 0.8},
	{0.25, 0.9},
	{0.025, 1.05}
};

GLUquadricObj* spriteCan;
GLUquadricObj* spriteTop;
GLUquadricObj* bowl;


int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	spriteCan = gluNewQuadric();
	gluQuadricDrawStyle(spriteCan, GLU_FILL);
	gluQuadricNormals(spriteCan, GLU_SMOOTH);
	spriteTop = gluNewQuadric();
	gluQuadricDrawStyle(spriteTop, GLU_FILL);
	gluQuadricNormals(spriteTop, GLU_SMOOTH);
	bowl = gluNewQuadric();
	gluQuadricDrawStyle(bowl, GLU_FILL);
	gluQuadricNormals(bowl, GLU_SMOOTH);

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
	glutPassiveMotionFunc(lookfunc);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	create_lists();

	// Set background color to black
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load textures
	if (!load_textures())
	{
		printf("Error loading textures.\n\n");
		system("pause");
		exit(0);
	}


	// Load shader programs
	defaultShaderProg = load_shaders(defaultVertexFile, defaultFragmentFile);
	lightShaderProg = load_shaders(lightVertexFile, lightFragmentFile);
	textureShaderProg = load_shaders(texVertexFile, texFragmentFile);
	numLights_param = glGetUniformLocation(lightShaderProg, "num_lights");
	texSampler = glGetUniformLocation(textureShaderProg, "texMap");
	glUseProgram(defaultShaderProg);

	glEnable(GL_DEPTH_TEST);
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
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 200.0);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], eye[X] + cos(DEG2RAD * theta), at[Y], eye[Z] + sin(DEG2RAD * theta), up[X], up[Y], up[Z]);

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
	glUseProgram(defaultShaderProg);

	glPushMatrix();
		glCallList(WINDOW_BORDER);
	glEndList();

	glPushMatrix();
		glCallList(MIRROR); // not a mirror yet
	glPopMatrix();

	glPushMatrix();
		glCallList(FRUIT);
	glPopMatrix();

	glUseProgram(textureShaderProg);

	glPushMatrix();
		glBindTexture(GL_FRONT_AND_BACK, BOWL_TEX);
		glCallList(BOWL);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[WINDOW_TEX]);
		glCallList(WINDOW);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[BLINDS_TEX]);
		glTranslatef(0.0f, blinds_transition_states[blinds_state][0], 0.0f);
		glScalef(1.0f, blinds_transition_states[blinds_state][1], 1.0f);
		glCallList(BLINDS);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_TEX]);
		glCallList(DESK);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[DOOR_TEX]);
		glCallList(DOOR);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[CARPET_TEX]);
		glCallList(CARPET);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[PAINTING_TEX]);
		glCallList(PAINTING);
	glPopMatrix();

	draw_sprite();
	draw_walls();
}

void move_helper(unsigned char key) {
	if (key == 'a')
	{
		eye[X] += sin(DEG2RAD * theta) * 0.25f;
		eye[Z] -= cos(DEG2RAD * theta) * 0.25f;
	}
	else if (key == 'd')
	{
		eye[X] -= sin(DEG2RAD * theta) * 0.25f;
		eye[Z] += cos(DEG2RAD * theta) * 0.25f;
	}
	if (key == 'w')
	{

		eye[X] += cos(DEG2RAD * theta) * 0.1f;
		eye[Z] += sin(DEG2RAD * theta) * 0.1f;
		
	}
	else if (key == 's')
	{
		eye[X] -= cos(DEG2RAD * theta) * 0.1f;
		eye[Z] -= sin(DEG2RAD * theta) * 0.1f;
	}

	if (eye[X] > 1.5f) {
		eye[X] = 1.5f;
	}
	if (eye[X] < -1.5f) {
		eye[X] = -1.5f;
	}
	if (eye[Z] > 2.0f) {
		eye[Z] = 2.0f;
	}
	if (eye[Z] < -0.75) {
		eye[Z] = -0.75f;
	}
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// <esc> quits
	if (key == 27)
	{
		exit(0);
	}
	if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
		move_helper(key);
	}
	else if (key == 'b') {
		if (blindsAnim) {
			blindsAnim = false;
			blinds_state_incr *= -1;
		}
		else blindsAnim = true;
	}
	glutPostRedisplay();
}

// Look callback 
void lookfunc(int x, int y)
{
	theta = x - (theta * .005);

	GLint dx, dy;

	dy = start_y - y;
	start_y = y;
	at[Y] += dy * .005;
	if (at[Y] < atMinY) at[Y] = atMinY;
	
	glutPostRedisplay();
}

// Idle callback
void idlefunc()
{
	if (blindsAnim) {
		time = glutGet(GLUT_ELAPSED_TIME);
		if (time - lasttime > 1000 / fps) {
			blinds_state += blinds_state_incr;
			if (blinds_state == 10) {
				blinds_state = 9;
				blinds_state_incr *= -1;
				blindsAnim = false;
			}
			else if (blinds_state == -1) {
				blinds_state = 0;
				blinds_state_incr *= -1;
				blindsAnim = false;
			}
			lasttime = time;
		}
		glutPostRedisplay();
	}
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
	glNewList(DESK, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.0f, -1.0f, -3.25f);
			glScalef(2.2f, 0.075f, 1.0f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.75f, -1.5f, -3.25f);
			glScalef(0.08f, 0.5f, 1.0f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(1.75f, -1.5f, -3.25f);
			glScalef(0.08f, 0.5f, 1.0f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0f, -1.5f, -4.25f);
			glScalef(1.8f, 0.5f, 1.0f);
			texCube();
		glPopMatrix();
	glEndList();

	glNewList(WINDOW, GL_COMPILE);
		glPushMatrix();
			glTranslatef(3.9f, 0.25f, 0.0f);
			glScalef(0.01f, 1.3f, 2.0f);
			texCube();
		glPopMatrix();
	glEndList();

	glNewList(WINDOW_BORDER, GL_COMPILE);
		glPushMatrix();
			glColor3f(0.8f, 0.8f, 0.8f);
			glTranslatef(3.905f, 0.25f, 0.0f);
			glScalef(0.01f, 1.35f, 2.05f);
			renderCube();
		glPopMatrix();
	glEndList();

	glNewList(BLINDS, GL_COMPILE);
		glTranslatef(3.88f, 0.25f, 0.0f);
		glScalef(0.01f, 1.3f, 2.0f);
		texCube();
	glEndList();

	glNewList(MIRROR, GL_COMPILE);
		glTranslatef(-3.9f, 0.25f, 0.0f);
		glScalef(0.01f, 1.0f, 1.75f);
		renderCube(mirrorColor, outlineColor);
	glEndList();

	glNewList(DOOR, GL_COMPILE);
		glTranslatef(2.25f, -0.1f, 4.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(1.9f, 1.0f, 0.01f);
		texCube();
	glEndList();

	glNewList(CARPET, GL_COMPILE);
		glTranslatef(0.0f, -2.0f, 0.0f);
		glScalef(2.5f, 0.01f, 2.5f);
		texCube();
	glEndList();

	glNewList(PAINTING, GL_COMPILE);
		glTranslatef(-1.25f, 0.25f, 3.9f);
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.01f, 1.5f, 1.25f);
		texCube();
	glEndList();

	glNewList(BOWL, GL_COMPILE);
		gluQuadricTexture(bowl, GL_TRUE);
		glTranslatef(-1.5f, -0.67f, -2.5f);
		glScalef(0.5f, 0.5f, 0.5f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(bowl, 1.25f, 0.0f, .5f, 10, 10);
	glEndList();

	glNewList(FRUIT, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.1f, 0.0f, 0.0f);
			glTranslatef(-1.5f, -0.67f, -2.5f);
			glColor3f(1.0f, 0.1f, 0.1f);
			glutSolidSphere(0.1f, 20, 20);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.1f, 0.0f, 0.0f);
			glTranslatef(-1.5f, -0.67f, -2.5f);
			glColor3f(1.0f, 0.96f, 0.31f);
			glutSolidSphere(0.1f, 20, 20);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.05f, 0.0f, 0.05f);
			glTranslatef(-1.5f, -0.67f, -2.5f);
			glColor3f(1.0f, 0.55f, 0.0f);
			glutSolidSphere(0.1f, 20, 20);
		glPopMatrix();
	glEndList();
}

bool load_textures() {
	for (int i = 0; i < NO_TEXTURE; i++)
	{
		// TODO: Load images
		tex_ids[i] = SOIL_load_OGL_texture(tex_files[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		
		// Set texture properties if successfully loaded
		if (tex_ids[i] != 0)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			// TODO: Set wrapping modes
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		// Otherwise texture failed to load
		else
		{
			return false;
		}
	}
	return true;
}

void draw_walls() {

	glBindTexture(GL_TEXTURE_2D, tex_ids[WALL_TEX]);

	glPushMatrix();
		texQuad(front_wall);
		texQuad(back_wall);
		texQuad(left_wall);
		texQuad(right_wall);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex_ids[FLOOR_TEX]);
	glPushMatrix(); 
		texQuad(floor_rect);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex_ids[ROOF_TEX]);
	glPushMatrix(); 
		texQuad(roof);
	glPopMatrix();
}

void draw_sprite() {
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[SPRITE_TEX]);
		gluQuadricTexture(spriteCan, GL_TRUE);
		glTranslatef(1.75f, -0.78f, -2.25f);
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluCylinder(spriteCan, 0.1f, 0.1f, 0.5f, 20, 20);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[SPRITE_TOP_TEX]);
		gluQuadricTexture(spriteTop, GL_TRUE);
		glTranslatef(1.75f, -0.275f, -2.25f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(spriteTop, 0.0f, 0.1f, 35, 35);
	glPopMatrix(); 
}