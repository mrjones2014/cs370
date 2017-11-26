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
GLfloat cube_tex[][2] = {{0.0f,0.0f}};

// Robot nodes
treenode torso, head, leftshoulder, lefthand, rightshoulder, righthand, leftleg, leftfoot, rightleg, rightfood, teapotbox;

// Rotation angles

// Animation variables

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
void draw_teapotbox();
void update_teapotbox();


int main(int argc, char* argv[])
{
	// Initialize glut
	glutInit(&argc, argv);

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
}

// Idle callback
void idlefunc()
{
	// Time-based Animations

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

// Routine to create model tree
void create_scene_graph()
{
	torso.material = silver;
	torso.f = draw_torso;
	torso.child = NULL;
	torso.sibling = NULL;
	torso.shaderProg = lightShaderProg;
	update_torso();
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
		glTexCoord2fv(t1);
		glVertex3fv(v1);
		glTexCoord2fv(t2);
		glVertex3fv(v2);
		glTexCoord2fv(t3);
		glVertex3fv(v3);
		glTexCoord2fv(t4);
		glVertex3fv(v4);
	glEnd();
}

void draw_torso()
{
	glPushMatrix();
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &torso.material);
		glScalef(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH);
		glutSolidCube(1.0f);
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
}

void update_head()
{
}

void draw_leftshoulder()
{
}

void update_leftshoulder()
{
}

void draw_lefthand()
{
}

void update_lefthand()
{
}

void draw_rightshoulder()
{
}

void update_rightshoulder()
{
}

void draw_righthand()
{
}

void update_righthand()
{
}

void draw_leftleg()
{
}

void update_leftleg()
{
}

void draw_leftfoot()
{
}

void update_leftfoot()
{
}

void draw_rightleg()
{
}

void update_rightleg()
{
}

void draw_rightfoot()
{
}

void update_rightfoot()
{
}

void draw_teapotbox()
{
}

void update_teapotbox()
{
}
