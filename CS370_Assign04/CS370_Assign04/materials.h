typedef struct MaterialType {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} MaterialType;

MaterialType brass = {{0.33,0.22,0.03,1.0},
					{0.78,0.57,0.11,1.0},
					{0.99,0.91,0.81,1.0},
					27.8};

MaterialType red_plastic = {{0.3,0.0,0.0,1.0},
						{0.6,0.0,0.0,1.0},
						{0.8,0.6,0.6,1.0},
						32.0};
MaterialType red_glass = { { 0.3f, 0.0f, 0.0f, 0.5f },
							{ 0.6f, 0.0f, 0.0f, 0.5f },
							{ 0.8f, 0.6f, 0.6f, 0.5f },
							32.0f };

MaterialType silver = { {0.19225,0.19225,0.19225, 1.0},
						{0.50754,0.50754,0.50754, 1.0},
						{0.508273,0.508273,0.508273, 1.0},
						50.0 };
						
void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}
