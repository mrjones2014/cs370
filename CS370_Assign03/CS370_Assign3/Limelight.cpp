// CS370 - Fall 2016
// Assign03 - Limelight

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
GLint numLights = 4;

#define DEG_TO_RAD 0.0175f
#define X 0
#define Y 1
#define Z 2
#define N 8
#define STAGE 1
#define CUBE 2
#define SPHERE 3
#define DIAMOND 4

GLint channels;

unsigned char* backgroundImg;

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


// colored lights positions
GLfloat blueLightPos[3] = { 0.0f, -2.0f, 0.0f };
GLfloat redLightPos[3] = { 0.0f, -2.0f, 0.0f };
GLfloat greenLightPos[3] = { 0.0f, -2.0f, 0.0f };

GLfloat cube[][3] = { { -1.0f,-1.0f,-1.0f },{ 1.0f,-1.0f,-1.0f },{ 1.0f,-1.0f,1.0f },
{ -1.0f,-1.0f,1.0f },{ -1.0f,1.0f,-1.0f },{ 1.0f,1.0f,-1.0f },
{ 1.0f,1.0f,1.0f },{ -1.0f,1.0f,1.0f } };

GLfloat stage[][3] = { { -8.0f, 1.0f, -4.0f },{ -8.0f, 1.0f, 4.0f },{ 8.0f, 1.0f, 4.0f },{ 8.0f, 1.0f, -4.0f },
					{ -8.0f, -1.0f, 4.0f },{ -8.0f, -1.0f, -4.0f },{ 8.0f, -1.0f, -4.0f },{ 8.0f, -1.0f, 4.0f } };

GLUquadricObj *quadric;

// Global subdivision parameters
int div_level = 8;

// Shadow matrices
GLfloat M1_s[16];
GLfloat M2_s[16];
GLfloat M3_s[16];

GLfloat shadow_color[] = { 0.2f,0.2f,0.2f };

// Global animation variables
GLint time = 0;
GLint lasttime = 0;
GLint fps = 144;

GLfloat cubeZ = 0;
GLfloat cubeDZ = 0.3;
GLfloat diamondTheta = 0;
GLfloat diamondDTheta = 5.0f;
GLfloat sphereY = 0;
GLfloat sphereDY = 0.3;
GLboolean cubeAnim = false;
GLboolean diamondAnim = false;
GLboolean sphereAnim = false;

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
void create_lists();
unsigned char* load_background(GLchar* imageFile);
void draw_background(unsigned char* image);

void drawCube();
void drawSphere();
void drawDiamond();

// void smooth_stage(); div_quad(stage[0], stage[1], stage[2], stage[3], div_level);

int main(int argc, char* argv[])
{
	// initialize shadow matricies 
	for (int i = 0; i < 16; i++) {
		M1_s[i] = 0.0f;
		M2_s[i] = 0.0f;
		M3_s[i] = 0.0f;
	}
	M1_s[0] = M1_s[5] = M1_s[10] = M2_s[0] = M2_s[5] = M2_s[10] = M3_s[0] = M3_s[5] = M3_s[10] = 1.0f;
	M1_s[7] = -1.0f / light1_pos[1];
	M2_s[7] = -1.0f / light2_pos[1];
	M3_s[7] = -1.0f / light3_pos[1];


	// Initialize glut
    glutInit(&argc, argv);

	// Initialize window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);

	backgroundImg = load_background("green.jpg");

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

	draw_background(backgroundImg);
	
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
	render_Scene(true);

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

void render_Scene(bool shadow)
{
	// Set point light source fixed in world
	set_PointLight(GL_LIGHT0, &white_light, light0_pos);
	set_SpotLight(GL_LIGHT1, &green_light, light1_pos, greenLightPos, 20.0f, 0.0f);
	set_SpotLight(GL_LIGHT2, &red_light, light2_pos, redLightPos, 20.0f, 0.0f);
	set_SpotLight(GL_LIGHT3, &blue_light, light3_pos, blueLightPos, 20.0f, 0.0f);

	// Set number of lights
	glUseProgram(lightShaderProg);
	glUniform1i(numLights_param,numLights);
	// Placeholder stage object
	//glPushMatrix();
	//	set_material(GL_FRONT_AND_BACK, &brass);
	//	glScalef(16.0, 2.0, 8.0);
	//	glutSolidCube(1.0);
	//glPopMatrix();

	glUseProgram(lightShaderProg);
	glUniform1i(numLights_param, numLights);

	// stage 
	glPushMatrix();
		glTranslatef(0.0f, -1.8f, 1.75f);
		glCallList(STAGE);
	glPopMatrix();

	// cube 
	glPushMatrix();
		drawCube();
	glPopMatrix();

	// sphere 
	glPushMatrix();
		drawSphere();
	glPopMatrix();

	// diamond 
	glPushMatrix();
		drawDiamond();
	glPopMatrix();

	// shadows 
	if (shadow) {
		glUseProgram(defaultShaderProg);
		glColor3fv(shadow_color);

		// cube shadow (green spot light)
		glPushMatrix();
			glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
			glMultMatrixf(M1_s);
			glTranslatef(-light1_pos[0], -light1_pos[1], -light1_pos[2]);

			drawCube();
		glPopMatrix();
		// cube shadow (white point light)
		glPushMatrix();
			glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
			glMultMatrixf(M1_s);
			glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

			drawCube();
		glPopMatrix();

		// sphere shadow (red spot light) 
		glPushMatrix();
			glTranslatef(light2_pos[0], light2_pos[1], light2_pos[2]);
			glMultMatrixf(M2_s);
			glTranslatef(-light2_pos[0], -light2_pos[1], -light2_pos[2]);

			drawSphere();
		glPopMatrix();
		// sphere shadow (white point light)
		glPushMatrix();
			glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
			glMultMatrixf(M2_s);
			glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

			drawSphere();
		glPopMatrix();

		// diamond shadow (blue spot light)
		glPushMatrix();
			glTranslatef(light3_pos[0], light3_pos[1], light3_pos[2]);
			glMultMatrixf(M2_s);
			glTranslatef(-light3_pos[0], -light3_pos[1], -light3_pos[2]);

			drawDiamond();
		glPopMatrix();
		// diamond shadow (white point light)
		glPushMatrix();
			glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
			glMultMatrixf(M2_s);
			glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

			drawDiamond();
		glPopMatrix();
	}
}

void drawCube() {
	glTranslatef(0.5f, -1.0f, cubeZ + 1.75f);
	glCallList(CUBE);
}


void drawSphere() {
	glTranslatef(0.75f, -1.0f + sphereY, 1.75f);
	glCallList(SPHERE);
}

void drawDiamond() {
	glTranslatef(5.5f, 1.0f, 1.0f);
	glRotatef(diamondTheta, 0.3f, 1.0f, 0.0f);
	glCallList(DIAMOND);
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

	switch (tolower(key))
	{
	case 'v':
		cubeAnim = !cubeAnim;
		break;
	case 'b':
		sphereAnim = !sphereAnim;
		break;
	case 'n':
		diamondAnim = !diamondAnim;
		break;
	case 'w':
		greenLightPos[2] -= 0.05f;
		glutPostRedisplay();
		break;
	case 's':
		greenLightPos[2] += 0.05f;
		glutPostRedisplay();
		break;
	case 'a':
		greenLightPos[0] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'd':
		greenLightPos[0] += 0.05f;
		glutPostRedisplay();
		break;
	case 't':
		redLightPos[2] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'g':
		redLightPos[2] += 0.05f;
		glutPostRedisplay();
		break;
	case 'f':
		redLightPos[0] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'h':
		redLightPos[0] += 0.05f;
		glutPostRedisplay();
		break;
	case 'i':
		blueLightPos[2] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'k':
		blueLightPos[2] += 0.05f;
		glutPostRedisplay();
		break;
	case 'j':
		blueLightPos[0] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'l':
		blueLightPos[0] += 0.05f;
		glutPostRedisplay();
		break;
	}
}

// Idle callback
void idlefunc()
{
	// Time-based Animations
	// Time-based Animations
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - lasttime > 1000 / fps) {
		if (cubeAnim) {
			if (cubeZ > 1.5f || cubeZ < -1.5f) cubeDZ *= -1;
			cubeZ += cubeDZ;
		}
		if (diamondAnim) diamondTheta += diamondDTheta;
		if (sphereAnim) {
			if (sphereY > 2 || sphereY < 0) sphereDY *= -1;
			sphereY += sphereDY;
		}
		if (cubeAnim || sphereAnim || diamondAnim) glutPostRedisplay();
	}
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

void create_lists() {
	glNewList(STAGE, GL_COMPILE);
		glPushAttrib(GL_CURRENT_BIT);
			set_material(GL_FRONT_AND_BACK, &brass);
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -2.0f);
				glScalef(8.0f, 1.0f, 4.0f);
				colorcube();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 1.0, 2.0);
				glRotatef(90.0, -1.0, 0.0, 0.0);
				glScalef(1.33, 0.5, 1.0);
				gluPartialDisk(quadric, 0, 6, 100, 100, 90, 180);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 2.0f);
				glRotatef(90, -1.0, 0.0f, 0.0f);
				glScalef(8.0f, 3.0f, 1.0f);
				gluCylinder(quadric, 1, 1, 2, 100, 100);
			glPopMatrix();
		glPopAttrib();
	glEndList();

	glNewList(SPHERE, GL_COMPILE);
		set_material(GL_FRONT_AND_BACK, &ruby);
		glPushAttrib(GL_CURRENT_BIT);
			glTranslatef(0.0f, 2.0f, -1.0f);
			glScalef(1.5f, 1.5f, 1.5f);
			glutSolidSphere(1 / sqrt(2), 10, 10);
		glPopAttrib();
	glEndList();

	glNewList(CUBE, GL_COMPILE);
		set_material(GL_FRONT_AND_BACK, &jade);
		glPushAttrib(GL_CURRENT_BIT);
			glTranslatef(-5.0f, 2.0f, -1.0f);
			glScalef(1.0f, 1.0f, 1.0f);
			glutSolidCube(1.0);
		glPopAttrib();
	glEndList();

	glNewList(DIAMOND, GL_COMPILE);
		set_material(GL_FRONT_AND_BACK, &cyanRubber);
		glPushAttrib(GL_CURRENT_BIT);
			glScalef(1.0f, 1.0f, 1.0f);
			glutSolidOctahedron();
		glPopAttrib();
	glEndList();
}

unsigned char* load_background(GLchar* imageFile) {
	return SOIL_load_image(imageFile, (int*)&ww, (int*)&hh, &channels, SOIL_LOAD_AUTO);
}
void draw_background(unsigned char* image) {
	// disable blending and depth test for background image
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);

	// set 2D projection for image
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw image (inverting)
	glRasterPos2f(0.0f, 1.0f);
	glPixelZoom(1.0f, -1.0f);
	glDrawPixels(ww * 1.3f, hh, GL_RGB, GL_UNSIGNED_BYTE, image);

	// reset zoom factors
	glPixelZoom(1.0f, 1.0f);

	// re-enable lighting, blending, and depth test
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
}