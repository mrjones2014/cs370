typedef struct MaterialType {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} MaterialType;
						
void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}

MaterialType jade = { { 0.135f, 0.2225f, 0.1575f, 0.95f },
					  { 0.54f, 0.89f, 0.63f, 0.95f },
					  { 0.316228f, 0.316228f, 0.316228f, 0.95f },
						12.8f };
MaterialType ruby = { { 0.1745f, 0.01175f, 0.01175f, 0.55f },
					  { 0.61424f, 0.04136f, 0.04136f, 0.55f },
					  { 0.727811f, 0.626959f, 0.626959f, 0.55f },
						76.8f };
MaterialType cyanRubber = { { 0.0f,0.05f,0.05f,1.0f },
						    { 0.4f,0.5f,0.5f,1.0f },
							{ 0.04f,0.7f,0.7f,1.0f },
							  10.0f };
