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
GLfloat sqr[4][2] = {
	{1.0f,1.0f},{-1.0f,1.0f},
	{-1.0f,-1.0f},{1.0f,-1.0f}
};

// Grass global variables
GLfloat grass_square[4][2] = { {sqr[1][0], sqr[0][0] * 0}, {sqr[0][0], sqr[0][0] * 0}, {sqr[3][0], sqr[3][1]}, {sqr[2][0], sqr[2][1]} };

// Sky global variables
GLfloat sky_square[4][2] = {
	{ 1.0f,1.0f },{ -1.0f,1.0f },
	{ -1.0f,-1.0f },{ 1.0f,-1.0f }
};

// Sun global variables

// House global variables
GLfloat house_square[4][2] = { 
	{.5 * sqr[0][0], 0.10 * sqr[0][1]},{.5 * sqr[2][0], 0.10 * sqr[0][0]},
	{.5 * sqr[2][0] , .75 * sqr[1][0]},{ .5 * sqr[0][0], .75*sqr[1][0] }
};
GLfloat house_roof_triangle[3][2] = {
	{0, 0.5*sqr[0][1]}, {0.5*sqr[2][0], 0.1*sqr[0][1]}, {0.5*sqr[0][0], 0.1*sqr[0][1]}
};

// Fan global variables
GLfloat theta = 0.0f;
GLfloat fanSpeedIncrement = 0.0f;
GLfloat fan_triangle[3][2] = {
	{0, 0}, {-0.15*sqr[0][0], 0.3*sqr[1][1]}, {0.15*sqr[0][1], 0.3*sqr[1][1]}
};

void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void idlefunc();

void drawSky();
void drawGrass();
void drawHouseBody();
void drawHouseRoof();
void drawHouse();
void drawFanBlade(int _theta);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("don quixote");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

	glutPostRedisplay();
}

// Scene render function
void render_Scene()
{
	drawSky();
	drawGrass();
	drawHouse();
	drawFanBlade(0.0f + theta);
	drawFanBlade(120.0f + theta);
	drawFanBlade(240.0f + theta);
}

void drawSky() {
	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glBegin(GL_POLYGON);
	for (int i = 0; i <4; i++) {
		if (i == 2) {
			glColor3f(1.0f, 1.0f, 1.0f); // back to white for gradient
		}
		glVertex2fv(sky_square[i]);
	}
	glEnd();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawGrass() {
	glColor3f(0.0f, 1.0f, 0.0f); //green
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(grass_square[i]);
	}
	glEnd();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawHouse() {
	drawHouseBody();
	drawHouseRoof();
}

void drawHouseBody() {
	glColor3f(0.5f, 0.25f, 0.08f); // brown ish
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2fv(house_square[i]);
	}
	glEnd();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawHouseRoof() {
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glBegin(GL_POLYGON);
	for (int i = 0; i < 3; i++) {
		glVertex2fv(house_roof_triangle[i]);
	}
	glEnd();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawFanBlade(int _theta) {
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 1.0f); //cyan

	GLfloat translateDist = house_roof_triangle[0][1] - fan_triangle[0][1];
	glTranslatef(0.0f, translateDist, 0.0f);
	glRotatef(_theta, 0.0f, 0.0f, 1.0f);

	glBegin(GL_POLYGON);
	for (int i = 0; i < 3; i++) {
		glVertex2fv(fan_triangle[i]);
	}
	glEnd();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Quit with escape
	if(key == 27)
	{
		exit(0);
	}
	if (key == 32) {
		fanSpeedIncrement += 10;
	}
}

// Idle callback
void idlefunc()
{
	if (fanSpeedIncrement != 0)
	{
		fanSpeedIncrement = fmax(fanSpeedIncrement - 0.05, 0);
	}

	if (fanSpeedIncrement != 0)
	{
		theta += fanSpeedIncrement;
		glutPostRedisplay();
	}
}