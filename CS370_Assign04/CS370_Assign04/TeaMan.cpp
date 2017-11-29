// CS370 - Fall 2016
// Assign04 - Textured Tea Man

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
#include "robot.h"

// Shader file utility functions
#include "shaderutils.h"

// Shader files
GLchar* defaultVertexFile = "basicvert.vs";
GLchar* defaultFragmentFile = "basicfrag.fs";
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* texVertexFile = "texturevert.vs";
GLchar* texFragmentFile = "texturefrag.fs";

// Shader objects
GLuint defaultShaderProg;
GLuint lightShaderProg;
GLuint textureShaderProg;
GLuint numLights_param;
GLint texSampler;

// Initial light position
GLfloat light_pos[4] = { 0.0, 30.0, 10.0, 1.0 };
// Global light variables
GLint numLights = 1;

// Texture constants
#define NO_TEXTURES 1
#define SHIRT 0
#define X 0
#define Y 1
#define Z 2
#define ANG_RANGE 60
#define DEG_PER_SEC (360.0/60.0)
#define DEG2RAD (3.14159/180.0)

// Texture indices
GLuint tex_ids[NO_TEXTURES];

// Texture files
char texture_files[NO_TEXTURES][20] = { "shirt_z.png" };

// Global camera vectors
GLfloat eye[3] = {0.0f,0.0f,25.0f};
GLfloat at[3] = {0.0f,0.0f,0.0f};
GLfloat up[3] = {0.0f,1.0f,0.0f};
int startx=0;
int starty=0;
// Global spherical coord values
GLfloat azimuth = 0.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 90.0f;
GLfloat del = 2.0f;
GLfloat radius = 25.0f;

// Global screen dimensions
GLfloat ww,hh;

// Node structure
struct treenode
{
	MaterialType material;
	GLfloat color[4];
	GLuint texture;
	void (*f)();
	GLfloat m[16];
	treenode *sibling;
	treenode *child;
	GLuint shaderProg;
};

// Cube vertices
GLfloat cube[][3] = { { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, 1.0f },
{ -1.0f, -1.0f, 1.0f }, { -1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f },
{ 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f } };

// Cube texture coords
// based on dimensions of texture 
GLfloat cube_tex[][2] = { 
	{ 0.5f, 0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },
	{ 0.5f,0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },
	{ 0.5f,0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },
	{ 0.5f,0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },{ 0.5f, 0.5f },
	{ (204.0f / 582.0f),0.0f },{ (204.0f / 582.0f), 1.0f },{ (408.0f / 582.0f), 1.0f },{ (408.0f / 582.0f), 0.0f },
	{ 0.0f, 0.0f },{ 204.0 / 582.0f, 1.0f },{ 204.0 / 582.0f, 1.0f },{ 0.0f, 1.0f }
};

// Robot nodes
treenode torso, head, leftshoulder, lefthand, rightshoulder, righthand, leftleg, leftfoot, rightleg, rightfoot, teapot, teapotbox;

GLUquadricObj* ears_quadric;

// Rotation angles
GLfloat teapot_theta = 0.0f;

// Animation variables
GLfloat time = 0.0f, lasttime = 0.0f;
GLfloat fps = 30.0f;

GLboolean teapot_anim = true;
GLfloat teapot_rps = 1.0f;

// Camera rotation variables

// Billboard variables
unsigned char* backgroundimg;
GLint channels;
GLint width = 500;
GLint height = 500;

void display();
void render_Scene();
void reshape(int w, int h);
void keyfunc(unsigned char key, int x, int y);
void idlefunc();
void mousefunc(int btn, int state, int x, int y);
void mousemove(int x, int y);
void traverse(treenode *node);
void create_scene_graph();
void load_image();
void draw_background();
bool load_textures();
void texturecube();
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[]);

void draw_torso();
void update_torso();
void draw_head();
void update_head();
void draw_leftshoulder();
void update_leftshoulder();
void draw_lefthand();
void update_lefthand();
void draw_rightshoulder();
void update_rightshoulder();
void draw_righthand();
void update_righthand();
void draw_leftleg();
void update_leftleg();
void draw_leftfoot();
void update_leftfoot();
void draw_rightleg();
void update_rightleg();
void draw_rightfoot();
void update_rightfoot();
void draw_teapot();
void update_teapot();
void draw_teapotbox();
void update_teapotbox();


int main(int argc, char* argv[])
{
	// Initialize glut
	glutInit(&argc, argv);

	ears_quadric = gluNewQuadric();
	gluQuadricDrawStyle(ears_quadric, GLU_FILL);
	gluQuadricNormals(ears_quadric, GLU_SMOOTH);

	// Initialize window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Robot");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);
	glutMouseFunc(mousefunc);
	glutMotionFunc(mousemove);

	// Compute initial cartesian camera position
	eye[X] = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
	eye[Y] = (GLfloat)(radius*cos(elevation*DEG2RAD));
	eye[Z] = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));

	// Set background color
	glClearColor(0.8,0.8,0.8,1.0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	
	// Set initial ambient light
	GLfloat background[] = { 0.2, 0.2, 0.2, 1.0 };
	set_AmbientLight(background);

	// Load shader programs
	defaultShaderProg = load_shaders(defaultVertexFile,defaultFragmentFile);
	lightShaderProg = load_shaders(lightVertexFile, lightFragmentFile);
	textureShaderProg = load_shaders(texVertexFile, texFragmentFile);

	// Associate shader parameters
	numLights_param = glGetUniformLocation(lightShaderProg,"numLights");
	texSampler = glGetUniformLocation(textureShaderProg, "texMap");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	load_image();

	// Load textures
	if (!load_textures())
	{
		exit(0);
	}

	// Build scene graph
	create_scene_graph();

	// Start graphics loop
	glutMainLoop();
	return 0;
}

// Display routine
void display()
{
	// Clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_background();

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
	glOrtho(-25.0f*xratio, 25.0f*xratio, -10.0f*yratio, 25.0f*yratio, 0.0f, 50.0f);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);

	// Render scene
	render_Scene();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

void render_Scene() 
{
	// Turn on and position LIGHT0
	set_PointLight(GL_LIGHT0,&white_light,light_pos);

	glTranslatef(0.0f, 10.0f, 0.0f);

	traverse(&torso);
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Esc to quit
	if (key == 27)
	{
		exit(0);
	}
	if (key == ' ') {
		teapot_anim = !teapot_anim;
	}
}

// Idle callback
void idlefunc()
{
	// Time-based Animations
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - lasttime > 1000.0f / fps) {
		if (teapot_anim) {
			// (seconds * rotations/second) results in fraction of 1 full rotation,
			// 1 full rotation = 360 degrees; multiply this fraction by 360 to get rotation in degrees
			GLfloat elapsed_time = (time - lasttime) / 1000.0f;
			GLfloat dtheta = (elapsed_time * teapot_rps) * 360.0f;
			teapot_theta += dtheta;
			if (teapot_theta > 360) teapot_theta -= 360; // bound to 0-360 degrees
			update_teapot();
		}
		lasttime = time;
	}
}

// Mouse callback
void mousefunc(int btn, int state, int x, int y)
{
	// Rotate around x and y axes for left button
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		startx = x;
		starty = y;
	}

}

// Mouse motion callback
void mousemove(int x, int y)
{
	// Adjust x and y rotation angles
	elevation += (y - starty);
	if (elevation > 179.8)
	{
		elevation = 179.8;
	}
	else if (elevation < 0.0)
	{
		elevation = 0.2;
	}

	azimuth += (x - startx);
	if (azimuth > 360.0)
	{
		azimuth -= 360.0;
	}
	else if (azimuth < 0.0)
	{
		azimuth += 360.0;
	}

	// Compute cartesian camera position
	eye[X] = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
	eye[Y] = (GLfloat)(radius*cos(elevation*DEG2RAD));
	eye[Z] = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));

	// Update mouse reference position
	startx = x;
	starty = y;

	// Redraw display
	glutPostRedisplay();
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

// Tree traversal routine
void traverse(treenode *node)
{
	// Stop when at bottom of branch
	if (node == NULL)
	{
		return;
	}

	// Apply local transformation and render
	glPushMatrix();
	glMultMatrixf(node->m);
	glUseProgram(node->shaderProg);
	node->f();

	// Recurse vertically if possible (depth-first)
	if (node->child != NULL)
	{
		traverse(node->child);
	}

	// Remove local transformation and recurse horizontal
	glPopMatrix();
	if (node->sibling != NULL)
	{
		traverse(node->sibling);
	}
}

// Routine to load background billboard image
void load_image()
{
	backgroundimg = SOIL_load_image("robotfactory_2.JPG", &width, &height, &channels, SOIL_LOAD_AUTO);
}

void draw_background()
{
	// TODO: Disable blending, and depth test for background image
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);

	// TODO: Set 2D projection for image
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// TODO: Draw image
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, backgroundimg);

	// TODO: Reset zoom factors
	glPixelZoom(1.0f, 1.0f);

	// TODO: Reenable lighting, blending, and depth test
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

// Routine to load textures using SOIL
bool load_textures() {
	for (int i = 0; i < NO_TEXTURES; i++)
	{
		// TODO: Load images
		tex_ids[i] = SOIL_load_OGL_texture(texture_files[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		
		// Set texture properties if successfully loaded
		if (tex_ids[i] != 0)
		{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			// TODO: Set wrapping modes
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		// Otherwise texture failed to load
		else
		{
			return false;
		}
	}
	return true;
}

// Routine to create model tree
void create_scene_graph()
{
	torso.texture = SHIRT;
	torso.f = draw_torso;
	torso.child = &leftleg;
	torso.sibling = &head;
	torso.shaderProg = textureShaderProg;
	update_torso();

	head.material = brass;
	head.f = draw_head;
	head.sibling = &leftshoulder;
	head.child = NULL;
	head.shaderProg = lightShaderProg;
	update_head();

	leftshoulder.material = red_plastic;
	leftshoulder.f = draw_leftshoulder;
	leftshoulder.sibling = &rightshoulder;
	leftshoulder.child = &lefthand;
	leftshoulder.shaderProg = lightShaderProg;
	update_leftshoulder();

	lefthand.material = brass;
	lefthand.f = draw_lefthand;
	lefthand.sibling = NULL;
	lefthand.child = &teapot;
	lefthand.shaderProg = lightShaderProg;
	update_lefthand();

	teapot.material = silver;
	teapot.f = draw_teapot;
	teapot.sibling = &teapotbox;
	teapot.child = NULL;
	teapot.shaderProg = lightShaderProg;
	update_teapot();

	teapotbox.material = red_glass;
	teapotbox.f = draw_teapotbox;
	teapotbox.sibling = NULL;
	teapotbox.child = NULL;
	teapotbox.shaderProg = lightShaderProg;
	update_teapotbox();

	rightshoulder.material = red_plastic;
	rightshoulder.f = draw_rightshoulder;
	rightshoulder.sibling = NULL;
	rightshoulder.child = &righthand;
	rightshoulder.shaderProg = lightShaderProg;
	update_rightshoulder();

	righthand.material = brass;
	righthand.f = draw_righthand;
	righthand.sibling = NULL;
	righthand.child = NULL;
	righthand.shaderProg = lightShaderProg;
	update_righthand();

	leftleg.material = silver;
	leftleg.f = draw_leftleg;
	leftleg.sibling = &rightleg;
	leftleg.child = &leftfoot;
	leftleg.shaderProg = lightShaderProg;
	update_leftleg();

	leftfoot.material = brass;
	leftfoot.f = draw_leftfoot;
	leftfoot.sibling = NULL;
	leftfoot.child = NULL;
	leftfoot.shaderProg = lightShaderProg;
	update_leftfoot();

	rightleg.material = silver;
	rightleg.f = draw_rightleg;
	rightleg.sibling = NULL;
	rightleg.child = &rightfoot;
	rightleg.shaderProg = lightShaderProg;
	update_rightleg();

	rightfoot.material = brass;
	rightfoot.f = draw_rightfoot;
	rightfoot.sibling = NULL;
	rightfoot.child = NULL;
	rightfoot.shaderProg = lightShaderProg;
	update_rightfoot();
}

// Routine to draw textured cube
void texturecube()
{
	// Top face
	texquad(cube[4], cube[7], cube[6], cube[5], cube_tex[0], cube_tex[1], cube_tex[2], cube_tex[3]);

	// Bottom face
	texquad(cube[0], cube[1], cube[2], cube[3], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);

	// Left face
	texquad(cube[2], cube[6], cube[7], cube[3], cube_tex[8], cube_tex[9], cube_tex[10], cube_tex[11]);

	// Right face
	texquad(cube[0], cube[4], cube[5], cube[1], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);

	// Front face
	texquad(cube[1], cube[5], cube[6], cube[2], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

	// Back face
	texquad(cube[0], cube[3], cube[7], cube[4], cube_tex[20], cube_tex[21], cube_tex[22], cube_tex[23]);
}

// Routine to draw quadrilateral face
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[])
{
	// Draw face 
	glBegin(GL_POLYGON);
		glTexCoord3fv(t1);
		glVertex3fv(v1);
		glTexCoord3fv(t2);
		glVertex3fv(v2);
		glTexCoord3fv(t3);
		glVertex3fv(v3);
		glTexCoord3fv(t4);
		glVertex3fv(v4);
	glEnd();
}

void draw_torso()
{
	glPushMatrix();
		glScalef(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH);
		texturecube();
	glPopMatrix();
}

void update_torso()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, torso.m);
	glPopMatrix();
}

void draw_head()
{
	glPushMatrix();
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &head.material);
		glTranslatef(0.0f, TORSO_HEIGHT + HEAD_OFFSET, 0.0f);
		glScalef(HEAD_XSCALE, HEAD_YSCALE, HEAD_ZSCALE);
		glutSolidSphere(HEAD_RADIUS, 20, 20);
		set_material(GL_FRONT_AND_BACK, &silver);
		glScalef(EAR_XSCALE, 1.0f, EAR_ZSCALE);
		glTranslatef(0.0f, EAR_OFFSET, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(ears_quadric, EAR_RADIUS, EAR_RADIUS, EAR_HEIGHT, 20, 20);
	glPopMatrix();
}

void update_head()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, head.m);
	glPopMatrix();
}

void draw_leftshoulder()
{
	glUniform1i(numLights_param, numLights);
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &leftshoulder.material);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_SHOULDER_RATIO * 2), (TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glutSolidSphere(UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO, 20, 20);
	glPopMatrix();
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &silver);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_SHOULDER_RATIO * 2) - (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO * 2), (TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_leftshoulder()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, leftshoulder.m);
	glPopMatrix();
}

void draw_lefthand()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &silver);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_HEIGHT + (2 * LOWER_ARM_ELBOW_RATIO)), (TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO, LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO, LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO);
		glRotatef(45.0, 1.0f, 1.0f, 0.0f);
		glutSolidCube(1.0f);
	glPopMatrix();
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &lefthand.material);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_HEIGHT * 2) + LOWER_ARM_ELBOW_RATIO, (TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_lefthand()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, lefthand.m);
	glPopMatrix();
}

void draw_rightshoulder()
{
	glUniform1i(numLights_param, numLights);
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &rightshoulder.material);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_SHOULDER_RATIO * 2), -(TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glutSolidSphere(UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO, 20, 20);
	glPopMatrix();
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &silver);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_SHOULDER_RATIO * 2) - (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO * 2), -(TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_rightshoulder()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, rightshoulder.m);
	glPopMatrix();
}

void draw_righthand()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &silver);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_HEIGHT + (2 * LOWER_ARM_ELBOW_RATIO)), -(TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO, LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO, LOWER_ARM_WIDTH * LOWER_ARM_ELBOW_RATIO);
		glRotatef(45.0, 1.0f, 1.0f, 0.0f);
		glutSolidCube(1.0f);
	glPopMatrix();
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &lefthand.material);
		glTranslatef(0.0f, TORSO_HEIGHT - (UPPER_ARM_HEIGHT * 2) + LOWER_ARM_ELBOW_RATIO, -(TORSO_DEPTH + (UPPER_ARM_HEIGHT * UPPER_ARM_SHOULDER_RATIO)));
		glScalef(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_righthand()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, righthand.m);
	glPopMatrix();
}

void draw_leftleg()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &leftleg.material);
		glTranslatef(0.0f, -TORSO_HEIGHT - (UPPER_LEG_HEIGHT / 2.0), UPPER_LEG_XOFFSET);
		glScalef(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_leftleg()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, leftleg.m);
	glPopMatrix();
}

void draw_leftfoot()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &leftfoot.material);
		glTranslatef(0.0f, -(2.0f * UPPER_LEG_HEIGHT) - (LOWER_LEG_HEIGHT / 2.0f), LOWER_LEG_XOFFSET);
		glScalef(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_leftfoot()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, leftfoot.m);
	glPopMatrix();
}

void draw_rightleg()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &rightleg.material);
		glTranslatef(0.0f, -TORSO_HEIGHT - (UPPER_LEG_HEIGHT / 2.0), -UPPER_LEG_XOFFSET);
		glScalef(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_rightleg()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, rightleg.m);
	glPopMatrix();
}

void draw_rightfoot()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &rightfoot.material);
		glTranslatef(0.0f, -(2.0f * UPPER_LEG_HEIGHT) - (LOWER_LEG_HEIGHT / 2.0f), -LOWER_LEG_XOFFSET);
		glScalef(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_DEPTH);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_rightfoot()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, rightfoot.m);
	glPopMatrix();
}

void draw_teapotbox()
{
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &teapotbox.material);
		glTranslatef(-TEAPOT_X, -TEAPOT_Y, TEAPOT_Z);
		glScalef(BOX_SCALE, BOX_SCALE, BOX_SCALE);
		glutSolidCube(1.0f);
	glPopMatrix();
}

void update_teapotbox()
{
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, teapotbox.m);
	glPopMatrix();
}

void draw_teapot() {
	glUniform1i(numLights_param, numLights);
	glPushMatrix();
		set_material(GL_FRONT_AND_BACK, &teapot.material);
		glScalef(TEAPOT_SCALE, TEAPOT_SCALE, TEAPOT_SCALE);
		glutSolidTeapot(1.0f);
	glPopMatrix();
}

void update_teapot() {
	glPushMatrix();
		glLoadIdentity();
		glTranslatef(-TEAPOT_X, -TEAPOT_Y, TEAPOT_Z);
		glRotatef(teapot_theta, 0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, teapot.m);
	glPopMatrix();
	glutPostRedisplay();
}
