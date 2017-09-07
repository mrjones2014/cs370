// CS370 - Fall 2016
// Assign01 - DonQuixote

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>

#define DEG2RAD 0.017453

// Instance square - DO NOT MODIFY
GLfloat sqr[4][2] = {{1.0f,1.0f},{-1.0f,1.0f},{-1.0f,-1.0f},{1.0f,-1.0f}};

// Grass global variables
GLfloat grass[4][2] = { {sqr[1][0], sqr[0][0] * 0}, {sqr[0][0], sqr[0][0] * 0}, {sqr[3][0], sqr[3][1]}, {sqr[2][0], sqr[2][1]} };
// Sky global variables
GLfloat sky[4][2];

// Sun global variables


// House global variables
GLfloat house[4][2] = { {.5*sqr[0][0], 0.10 * sqr[0][1]}, {.5 * sqr[2][0], 0.10 * sqr[0][0]}, {.5 * sqr[2][0] , .75 * sqr[1][0] }, {.5 * sqr[0][0], .75*sqr[1][0]} };
GLfloat roof[3][2] = { {0 * sqr[0][0], .5 * sqr[0][1]}, { .5 * sqr[2][0], 0.1 * sqr[0][1]} , { .5*sqr[0][0], 0.1* sqr[0][1] } };
// Fan global variables
GLfloat fan[6][2] = { {0,.5f}, {-.1f,.7f}, {0,.65}, { .1f,.7f }, { .1f,.65f } ,{ -.1f,.7f } };
GLfloat fan1[6][2] = { {0,.5f },{-.1f,.3f },{ 0,.35 },{ .1f,.3f },{ .1f,.25f } ,{-.1f,.3f } };



void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void idlefunc();

int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// Set the window size to 500x500 pixels
	glutInitWindowSize(500, 500);

	// Create window
	glutCreateWindow("Don Quixote!");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			sky[i][j] = sqr[i][j];
		}
	}
	// Define callbacks
	glutDisplayFunc(display);

	// Set background color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Begin event loop
	glutMainLoop();

	return 0;
}

// Display callback
void display()
{
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT);

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
	//render sky
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(sky[i]);
	}
	glEnd();
	//render grass
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(grass[i]);
	}
	glEnd();
	//build the house
	glColor3f(0.545f, 0.27f, .074f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(house[i]);
	}
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	
	for (int i = 0; i < 3; i++) {
		glVertex2fv(roof[i]);
	}



	//render the fans
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 4; i++) {
		glColor3f(i/3, 1.0f, 1.0f);
		glVertex2fv(fan[i]);
	}

	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 4; i++) {
		glColor3f(i / 3, 1.0f, 1.0f);
		glVertex2fv(fan1[i]);
	}

	glEnd();
	glutSolidSphere(.15, 500, 20);
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Quit with escape
	if(key == 27)
	{
		exit(0);
	}
}

// Idle callback
void idlefunc()
{

}