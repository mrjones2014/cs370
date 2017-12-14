// CS370 - Fall 2014
// Final Project


// CONTROLS (case insensitive) 
/*
 * 0 - toggle debug mode (disable camera clipping)
 * W - move forward
 * S - move backward 
 * A - strafe left
 * D - strafe right 
 * E - "escape the room" (open the door)
 * F - toggle the fan on/off
 * B - toggle the blinds open/closed
 * move mouse - look around
 */

bool debug = false;
char camera_pos_str[3][50];

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
#include <ctype.h>
#include <string.h>

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
GLfloat eye[3] = { 0.0f,0.75f,1.0f };
GLfloat at[3] = { 0.0f,0.5f,0.0f };
GLfloat up[3] = { 0.0f,1.0f,0.0f };

// CAMERA MANIPULATION VARIABLES
bool keys[256];
GLfloat theta = 0;
GLfloat eyeMinY = 0.1f;
GLfloat atMinY = -1.15f;
GLfloat atMaxY = 2.0f;
GLfloat start_y = NULL;
GLfloat start_x = NULL;

// ANIMATION VARIABLES
unsigned int time = 0;
GLfloat lasttime = 0;
GLfloat fpstime = 0;
GLfloat fps = 30;
GLboolean door_open = false;
GLfloat fan_theta = 0.0f;
GLboolean fanAnim = false;
GLboolean blindsAnim = false;
GLint blinds_state_incr = -1;
GLint blinds_state = 9;
GLfloat blinds_transition_states[10][2] = {
	{ 1.45, 0.1 },
	{ 1.3, 0.2 },
	{ 1.15, 0.3 },
	{ 1.0, 0.4 },
	{ 0.85, 0.5 },
	{ 0.7, 0.6 },
	{ 0.55, 0.7 },
	{ 0.4, 0.8 },
	{ 0.25, 0.9 },
	{ 0.025, 1.05 }
};


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
void keyupfunc(unsigned char key, int x, int y);
void idlefunc();
void lookfunc(int x, int y);
void reshape(int w, int h);
void create_lists();
bool load_textures();
void draw_sprite();
void draw_fan();
void draw_door();
void draw_walls();

void clip_camera_pos();
void execute_movement();

void create_mirror();
void draw_mirror();

void draw_debug_text();

// DISPLAY LIST IDS
#define WALLS 1
#define DESK 2
#define WINDOW 3
#define MIRROR 4
#define CARPET 5
#define BLINDS 6
#define PAINTING 7
#define BOWL 8
#define WINDOW_BORDER 9
#define FRUIT 10
#define BRICK_BEHIND_DOOR 11
#define CHAIR 12
#define COMPUTER 13
#define COMPUTER_SCREEN 14
#define SWITCH 15
#define KEYBOARD 16


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
#define BRICK_TEX 12
#define ENVIRONMENT 13
#define SATAN 14
#define KEYBOARD_TEX 15
#define NO_TEXTURE 16

GLint tex_ids[NO_TEXTURE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
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
	"bowl.bmp",
	"brick.jpg",
	"blank.bmp",
	"html-regex-satan.jpg",
	"keyboard.png"
};

GLfloat outlineColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat mirrorColor[4] = { 0.780f, 0.780f, 0.780f, 1.0f };

GLfloat roof[][3] = { { -4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, 4.0f },{ -4.0f, 3.25f, 4.0f } };
GLfloat floor_rect[][3] = { { -4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat back_wall[][3] = { { -4.0f, 3.25f, -4.0f },{ 4.0f, 3.25f, -4.0f } ,{ 4.0f, -2.0f, -4.0f } ,{ -4.0f, -2.0f, -4.0f } };
GLfloat front_wall[][3] = { { -4.0f, 3.25f, 4.0f },{ 4.0f, 3.25f, 4.0f },{ 4.0f, -2.0f, 4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat right_wall[][3] = { { -4.0f, 3.25f, 4.0f },{ -4.0f, 3.25f, -4.0f },{ -4.0f, -2.0f, -4.0f },{ -4.0f, -2.0f, 4.0f } };
GLfloat left_wall[][3] = { { 4.0f, 3.25f, 4.0f },{ 4.0f, 3.25f, -4.0f },{ 4.0f, -2.0f, -4.0f },{ 4.0f, -2.0f, 4.0f } };

GLUquadricObj* spriteCan;
GLUquadricObj* spriteTop;
GLUquadricObj* bowl;
GLUquadricObj* fanCenter;

GLfloat mirror_x = -3.89f;
GLfloat mirror_y_upper = 2.0f;
GLfloat mirror_y_lower = -1.0f;
GLfloat mirror_z_left = -2.5f;
GLfloat mirror_z_right = -mirror_z_left;


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
	fanCenter = gluNewQuadric();
	gluQuadricDrawStyle(fanCenter, GLU_FILL);
	gluQuadricNormals(fanCenter, GLU_SMOOTH);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Set the window size to image size
	glutInitWindowSize(512, 512);

	// Create window
	glutCreateWindow("House");

	glutSetCursor(GLUT_CURSOR_NONE);

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutIgnoreKeyRepeat(1);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutKeyboardUpFunc(keyupfunc);
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
	numLights_param = glGetUniformLocation(lightShaderProg, "numLights");
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
	// PASS 1 - Create mirror texture 
	create_mirror();

	// PASS 2 - Render actual scene
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
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200.0);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], eye[X] + cos(DEG2RAD * theta), at[Y], eye[Z] + sin(DEG2RAD * theta), up[X], up[Y], up[Z]);

	// Render scene
	render_Scene();

	// Render mirror
	draw_mirror();

	// Draw debug text if debug mode
	if (debug) draw_debug_text();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

// Scene render function
void render_Scene()
{ 
	glScalef(2.0f, 2.0f, 2.0f);
	
	glPushMatrix();
		draw_mirror();
	glPopMatrix();

	glUseProgram(defaultShaderProg);

	glPushMatrix();
		glCallList(WINDOW_BORDER);
	glEndList();

	glPushMatrix();
		glCallList(FRUIT);
		glPushMatrix();
			glTranslatef(0.22f, 0.0f, 0.0f);
			glCallList(FRUIT);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.22f, 0.0f, -0.22f);
			glCallList(FRUIT);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.22f, 0.0f, -0.22f);
			glCallList(FRUIT);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.22f, 0.0f, 0.0f);
			glCallList(FRUIT);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		draw_fan();
	glPopMatrix();

	glPushMatrix();
		glCallList(COMPUTER);
	glPopMatrix();

	glUseProgram(textureShaderProg);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[SATAN]);
		glCallList(COMPUTER_SCREEN);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[KEYBOARD_TEX]);
		glCallList(KEYBOARD);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[BOWL_TEX]);
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
		glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_TEX]);
		glTranslatef(-3.0f, 0.0f, 3.0f);
		glRotatef(45+90, 0.0f, 1.0f, 0.0f);
		glCallList(CHAIR);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[BRICK_TEX]);
		glCallList(BRICK_BEHIND_DOOR);
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
	draw_door();
	draw_walls();
}

void clip_camera_pos() {
	if (eye[X] > 5.5f) {
		eye[X] = 5.5f;
	}
	if (eye[X] < -5.5f) {
		eye[X] = -5.5f;
	}
	if (eye[Z] > 5.5f) {
		eye[Z] = 5.5f;
	}
	if (eye[Z] < -3.0) {
		eye[Z] = -3.0f;
	}
}

void execute_movement() {
	bool needs_redisplay = false;
	if (keys['a']) {
		eye[X] += sin(DEG2RAD * theta) * 0.1f;
		eye[Z] -= cos(DEG2RAD * theta) * 0.1f;
		needs_redisplay = true;
	}
	if (keys['d']) {
		eye[X] -= sin(DEG2RAD * theta) * 0.1f;
		eye[Z] += cos(DEG2RAD * theta) * 0.1f;
		needs_redisplay = true;
	}
	if (keys['w']) {
		eye[X] += cos(DEG2RAD * theta) * 0.1f;
		eye[Z] += sin(DEG2RAD * theta) * 0.1f;
		needs_redisplay = true;
	}
	if (keys['s']) {
		eye[X] -= cos(DEG2RAD * theta) * 0.1f;
		eye[Z] -= sin(DEG2RAD * theta) * 0.1f;
		needs_redisplay = true;
	}

	if (!debug) clip_camera_pos(); // make sure camera stays within bounds of house

	if (needs_redisplay) glutPostRedisplay();
}

// Keyup callback
void keyupfunc(unsigned char _key, int x, int y) {
	unsigned char key = tolower(_key);
	keys[key] = false;
}

// Keyboard callback
void keyfunc(unsigned char _key, int x, int y)
{
	// <esc> quits
	if (_key == 27)
	{
		exit(0);
	}
	if (_key == '0') {
		debug = !debug;
		glutPostRedisplay();
	}
	unsigned char key = tolower(_key);

	if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
		keys[key] = true;
	}
	else if (key == 'b') {
		if (blindsAnim) {
			blindsAnim = false;
			blinds_state_incr *= -1;
		}
		else blindsAnim = true;
	}
	else if (key == 'f') {
		fanAnim = !fanAnim;
	}
	else if (key == 'e') {
		door_open = !door_open;
		glutPostRedisplay();
	}
}

// Look callback 
void lookfunc(int x, int y)
{
	if (start_y == NULL) start_y = y;
	if (start_x == NULL) start_x = x;

	// reset cursor position to center
	if (x <= 10 || x >= 490 || y < 10 || y > 490) {
		start_x = 250;
		start_y = 250;
		glutWarpPointer(250, 250);
	}

	theta -= start_x - x;
	if (abs((int)start_y - y) < 200) at[Y] += (start_y - y) * 0.005; // to avoid camera jumping bug where y is set before start_y on cursor reset

	if (at[Y] < atMinY) at[Y] = atMinY;
	else if (at[Y] > atMaxY) at[Y] = atMaxY;

	start_y = y;
	start_x = x;
	
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
	if (fanAnim) {
		fan_theta += 1.0f;
		glutPostRedisplay();
	}

	execute_movement();
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
	glNewList(COMPUTER, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.0f, -0.9f, -3.0f);
			glScalef(0.6f, 0.075f, 0.2f);
			renderCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0f, -0.75f, -3.0f);
			glScalef(0.1f, 0.3, 0.1f);
			renderCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0f, -0.1f, -3.0f);
			glScalef(0.75f, 0.421875f, 0.075f);
			renderCube();
		glPopMatrix();
	glEndList();
	glNewList(COMPUTER_SCREEN, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.0f, -0.1f, -2.9f);
			glScalef(-0.75f, 0.421875f, 0.005f);
			texCube();
		glPopMatrix();
	glEndList();
	glNewList(KEYBOARD, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.0f, -0.9f, -2.5f);
			glScalef(0.6f, 0.005f, 0.2f);
			texCube();
		glPopMatrix();
	glEndList();

	glNewList(CHAIR, GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.0f, -1.1f, 0.0f);
			glScalef(0.5f, 0.1f, 0.5f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.39f, -1.55f, 0.39f);
			glScalef(0.1f, 0.5f, 0.1f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.39f, -1.55f, 0.39f);
			glScalef(0.1f, 0.5f, 0.1f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.39f, -1.55f, -0.39f);
			glScalef(0.1f, 0.5f, 0.1f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.39f, -1.55f, -0.39f);
			glScalef(0.1f, 0.5f, 0.1f);
			texCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.00f, -0.55f, -0.47f);
			glScalef(0.5f, 0.5f, 0.05f);
			texCube();
		glPopMatrix();
	glEndList();

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

	glNewList(BRICK_BEHIND_DOOR, GL_COMPILE);
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
			glTranslatef(0.0f, 0.0f, 0.1f);
			glTranslatef(-1.5f, -0.67f, -2.5f);
			glColor3f(1.0f, 0.55f, 0.0f);
			glutSolidSphere(0.1f, 20, 20);
		glPopMatrix();
	glEndList();
}

bool load_textures() {
	for (int i = 0; i < NO_TEXTURE; i++)
	{
		if (i == ENVIRONMENT) {
			// Load environment map texture (NO MIPMAPPING)
			tex_ids[i] = SOIL_load_OGL_texture(tex_files[0], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

			// TODO: Set environment map properties if successfully loaded
			if (tex_ids[i] != 0)
			{
				// Set scaling filters (no mipmap)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				// Set wrapping modes (clamped)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			}
			// Otherwise texture failed to load
			else
			{
				return false;
			}
		}
		else {
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
	}
	return true;
}

void draw_fan() {
	glPushMatrix();
			glTranslatef(0.0f, 3.0f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glColor3f(1.0f, 0.96f, 0.31f);
			gluDisk(fanCenter, 0.0f, 0.5f, 20, 20);
		glPopMatrix();
		glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 3.1f, 0.0f);
			glRotatef(fan_theta, 0.0f, 1.0f, 0.0f);
			glScalef(1.5f, 0.01f, 0.1f);
			renderCube();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0f, 3.1f, 0.0f);
			glRotatef(90.0f + fan_theta, 0.0f, 1.0f, 0.0f);
			glScalef(1.5f, 0.01f, 0.1f);
			renderCube();
		glPopMatrix();
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

void draw_door() {
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_ids[DOOR_TEX]);
		glTranslatef(2.25f, -0.1f, 3.99f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		if (door_open) {
			glTranslatef(-0.75f, 0.0f, 1.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		}
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(1.9f, 1.0f, 0.01f);
		texCube();
	glPopMatrix();
}


void create_mirror() {
	// PASS 1 - Render reflected scene
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Set projection matrix for flat "mirror" camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(mirror_z_left, mirror_y_upper, mirror_y_lower, mirror_y_upper, 0.0, 15.0);

	// TODO: Set modelview matrix positioning "mirror" camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(mirror_x, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// TODO: Render scene from mirror
	glPushMatrix();
	render_Scene();
	glPopMatrix();

	glFinish();

	// TODO: Copy scene to texture
	glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 512, 512, 0);
}

void draw_mirror() {
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler,0);
		// TODO: Draw mirror surface
		glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(mirror_x, mirror_y_lower, mirror_z_left);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(mirror_x, mirror_y_upper, mirror_z_left);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(mirror_x, mirror_y_upper, mirror_z_right);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(mirror_x, mirror_y_lower, mirror_z_right);
		glEnd();

		// Draw mirror frame
		glUseProgram(defaultShaderProg);
		glColor3f(0.8f, 0.8f, 0.8f);
		glBegin(GL_POLYGON);
			glVertex3f(mirror_x - 0.001, mirror_y_lower - 0.15, mirror_z_left - 0.15);
			glVertex3f(mirror_x - 0.001, mirror_y_upper + 0.15, mirror_z_left - 0.15);
			glVertex3f(mirror_x - 0.001, mirror_y_upper + 0.15, mirror_z_right + 0.15);
			glVertex3f(mirror_x - 0.001, mirror_y_lower - 0.15, mirror_z_right + 0.15);
		glEnd();
	glPopMatrix();
}

void draw_debug_text() {
	sprintf(camera_pos_str[0], "X: %f", eye[X]);
	sprintf(camera_pos_str[1], "Y: %f", eye[Y]);
	sprintf(camera_pos_str[2], "Z: %f", eye[Z]);

	// Save previous matrices, switch to 2D, and turn off lighting for text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Switch to default shader
	glUseProgram(0);

	GLfloat text_bg_color[4] = { 0.8f, 0.8f, 0.8f, 0.5f };
	GLfloat text_bg[4][2] = {
		{-1.0f, 1.0f},
		{ -0.25f, 1.0f},
		{ -0.25f, 0.65f},
		{-1.0f, 0.65f}
	};

	// TODO: Set camera_pos_str text position to top left and draw
	glColor3f(0.0f, 0.0f, 0.0f);
	GLfloat text_y = 0.9f;
	for (unsigned int i = 0; i < 3; i++) {
		glRasterPos2f(-0.9f, text_y);
		for (unsigned int j = 0; j<strlen(camera_pos_str[i]); j++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, camera_pos_str[i][j]);
		}
		text_y -= 0.1;
	}

	glColor4fv(text_bg_color);

	glBegin(GL_POLYGON);
		glVertex2fv(text_bg[0]);
		glVertex2fv(text_bg[1]);
		glVertex2fv(text_bg[2]);
		glVertex2fv(text_bg[3]);
	glEnd();

	// Restore previous matricies and turn on lighting
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Switch to default shader program
	glUseProgram(defaultShaderProg);
}
