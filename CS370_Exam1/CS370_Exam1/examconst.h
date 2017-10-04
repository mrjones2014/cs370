// Table coordinates - DO NOT MODIFY
#define TABLE_SIZE 4.0f
#define TABLE_Y 0.50f
#define TARGET_X -0.5f
#define TARGET_Z 1.0f
#define CIRCLE_RAD 2.3f

// Shader files - DO NOT MODIFY
GLchar* vertexFile = "basicvert.vs";
GLchar* fragmentFile = "basicfrag.fs";

// Shader objects - DO NOT MODIFY
GLuint shaderProg;

// Global mode variables - DO NOT MODIFY
int mode=0;
GLfloat dtheta = 0.2f;
GLfloat azimuth = 45.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 54.7f;
GLfloat del = 2.0f;
GLfloat radius = 1.732f;

// Global camera vectors - DO NOT MODIFY
GLfloat eye[3] = {1.0f,1.0f,1.0f};
GLfloat at[3] = {0.0f,0.0f,0.0f};
GLfloat up[3] = {0.0f,1.0f,0.0f};
