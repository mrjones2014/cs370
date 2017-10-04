// CS370.102 - Fall 2017
// Exam1 - Double Diamond

/******************************************/
/*       INSERT (a) CODE HERE             */
/******************************************/
// Name: Mat Jones

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "examconst.h"

// Shader file utility functions - DO NOT MODIFY
#include "shaderutils.h"

// Display list symbolic constants - DO NOT MODIFY
#define DIAMOND 1
#define X 0
#define Y 1
#define Z 2
#define DEG2RAD 0.01745

// Global geometry variables - DO NOT MODIFY
GLfloat pyr[][3] = {{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{1.0f,0.0f,1.0f},{0.0f,0.0f,1.0f},{0.5f,1.0f,0.5f}};
GLfloat bottom_color[3] = {0.0f,0.0f,0.0f};
GLfloat top_color[3] = {1.0f,1.0f,0.0f};

// Global spin variables - DO NOT MODIFY
GLfloat spin_theta = 0.0f;
bool spin_flag = true;
GLfloat rot_theta = 0.0f;
bool rot_flag = true;

// Routine prototypes - DO NOT MODIFY
void display();
void render_Scene();
void idlefunc();
void keyfunc(unsigned char key, int x, int y);
void reshape(int w, int h);
void draw_table();
void draw_diamond();
void draw_pyramid(GLfloat col[]);
void draw_triangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat color[]);
void create_list();

// Main routine - DO NOT MODIFY
int main(int argc, char* argv[])
{
	// Initialize glut
	glutInit(&argc, argv);

	// Initialize window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("CS370 Exam1-102");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);

	// Set clear color to grey
	glClearColor(0.5f,0.5f,0.5f,1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Create display lists
	create_list();

	// Load shader programs
	shaderProg = load_shaders(vertexFile, fragmentFile);
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

	/******************************************/
	/*       INSERT (b) CODE HERE             */
	/******************************************/
	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-6.0f, 6.0f, -6.0f, 6.0f, -6.0f, 6.0f);
	gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render scene
	render_Scene();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

// Render scene
void render_Scene()
{
	if (mode == 0)
	{
		draw_table();
	}
	// (c) Pyramid instance model
	else if (mode==1)
	{
		draw_table();
		draw_pyramid(bottom_color);		
	}
	// (d) Basic diamond
	else if (mode==2)
	{
		draw_table();
		glPushMatrix();
			draw_diamond();
		glPopMatrix();		
	}

	// (e) Diamonds located at target location
	else if (mode==3)
	{
		draw_table();
		/******************************************/
		/*       INSERT (e) CODE HERE             */
		/******************************************/

		glPushMatrix();
			glTranslatef(TARGET_X, TABLE_Y, TARGET_Z);
			draw_diamond();
			glPushMatrix();
				glRotatef(180.0f, 0.0f, 0.0f, TARGET_Z);
				glTranslatef(0.0f, -4 * pyr[4][1], 0.0f);
				draw_diamond();
			glPopMatrix();
		glPopMatrix();		

	}

	// (f) Spinning Diamonds
	else if (mode==4)
	{
		draw_table();
		/******************************************/
		/*       INSERT (f) CODE HERE             */
		/******************************************/

		glPushMatrix();
			glTranslatef(TARGET_X, TABLE_Y, TARGET_Z);
			glRotatef(spin_theta, 0.0f, TABLE_Y, 0.0f);
			draw_diamond();
			glPushMatrix();
				glRotatef(-2 * spin_theta, 0.0f, TABLE_Y, 0.0f);
				glRotatef(180.0f, 0.0f, 0.0f, TARGET_Z);
				glTranslatef(0.0f, -4 * pyr[4][1], 0.0f);
				draw_diamond();
			glPopMatrix();
		glPopMatrix();	

	}
	// (g) Spinning diamonds with revolving diamond
	else if (mode==5)
	{
		draw_table();
		/******************************************/
		/*       INSERT (g) CODE HERE          */
		/******************************************/
		glPushMatrix();
			glTranslatef(TARGET_X, TABLE_Y, TARGET_Z);
			glRotatef(spin_theta, 0.0f, TABLE_Y, 0.0f);
			draw_diamond();
			glPushMatrix();
				glRotatef(-2 * spin_theta, 0.0f, TABLE_Y, 0.0f);
				glRotatef(180.0f, 0.0f, 0.0f, TARGET_Z);
				glTranslatef(0.0f, -4 * pyr[4][1], 0.0f);
				draw_diamond();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(TARGET_X, TABLE_Y, TARGET_Z);
			glRotatef(spin_theta, 0.0f, pyr[4][1], 0.0f);
			glTranslatef(0.0f, 0.0f, CIRCLE_RAD);
			draw_diamond();	
		glPopMatrix();
	}
}

// Routine to draw pyramid instance
void draw_pyramid(GLfloat col[])
{
	/******************************************/
	/*       INSERT (c) CODE HERE             */
	/******************************************/
	glPushMatrix();
		draw_triangle(pyr[0], pyr[1], pyr[4], col);
		draw_triangle(pyr[1], pyr[2], pyr[4], col);
		draw_triangle(pyr[2], pyr[3], pyr[4], col);
		draw_triangle(pyr[3], pyr[0], pyr[4], col);
	glPopMatrix();
}

// Routine to draw diamond from pyramid instances
void draw_diamond()
{
	/******************************************/
	/*       INSERT (d) CODE HERE             */
	/******************************************/
	// bottom half
	glPushMatrix();
		glTranslatef(pyr[4][0], pyr[4][1], -pyr[4][2]);
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		draw_pyramid(bottom_color);
	glPopMatrix();

	// top half
	glPushMatrix();
		glTranslatef(-pyr[4][0], pyr[4][1], -pyr[4][2]);
		draw_pyramid(top_color);
	glPopMatrix();
}

// Idle callback
void idlefunc()
{
	/******************************************/
	/*         INSERT (f) (g) CODE HERE       */
	/******************************************/
	if (mode > 3)
	{
		if (spin_flag)
		{
			spin_theta += dtheta;
		}
		if (mode > 4)
		{
			if (rot_flag)
			{
				rot_theta += dtheta;
			}
		}
		glutPostRedisplay();
	}
}

void create_list()
{
	/******************************************/
	/*       INSERT (h) CODE HERE             */
	/******************************************/

}

#include "examfunc.cpp"