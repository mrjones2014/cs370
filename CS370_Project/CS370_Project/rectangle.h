#pragma once
GLfloat baseCube[][3] = {
        {-1.0f, -1.0f, -1.0f},
        {1.0f,  -1.0f, -1.0f},
        {1.0f,  -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        {-1.0f, 1.0f,  -1.0f},
        {1.0f,  1.0f,  -1.0f},
        {1.0f,  1.0f,  1.0f},
        {-1.0f, 1.0f,  1.0f}
};

void quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[]) {
    glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

    // Draw outline
    glBegin(GL_LINE_LOOP);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();
}

void quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat bodyColor[], GLfloat outlineColor[]) {
    glColor3fv(bodyColor);
    glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

    // Draw outline
    glColor3fv(outlineColor);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();
}

void rect(GLfloat rect[4][3]) {
	quad(rect[0], rect[1], rect[2], rect[3]);
}

void rect(GLfloat rect[4][3], GLfloat color[3]) {
	quad(rect[0], rect[1], rect[2], rect[3], color, color);
}

void rect(GLfloat rect[4][3], GLfloat bodyColor[3], GLfloat outlineColor[3]) {
	quad(rect[0], rect[1], rect[2], rect[3], bodyColor, outlineColor);
}

void renderCube()
{
	quad(baseCube[4], baseCube[7], baseCube[6], baseCube[5]);
	quad(baseCube[0], baseCube[1], baseCube[2], baseCube[3]);
	quad(baseCube[0], baseCube[3], baseCube[7], baseCube[4]);
	quad(baseCube[1], baseCube[5], baseCube[6], baseCube[2]);
	quad(baseCube[2], baseCube[6], baseCube[7], baseCube[3]);
	quad(baseCube[0], baseCube[4], baseCube[5], baseCube[1]);
}

void renderCube(GLfloat bodyColor[3], GLfloat outlineColor[3])
{
	quad(baseCube[4], baseCube[7], baseCube[6], baseCube[5], bodyColor, outlineColor);
	quad(baseCube[0], baseCube[1], baseCube[2], baseCube[3], bodyColor, outlineColor);
	quad(baseCube[0], baseCube[3], baseCube[7], baseCube[4], bodyColor, outlineColor);
	quad(baseCube[1], baseCube[5], baseCube[6], baseCube[2], bodyColor, outlineColor);
	quad(baseCube[2], baseCube[6], baseCube[7], baseCube[3], bodyColor, outlineColor);
	quad(baseCube[0], baseCube[4], baseCube[5], baseCube[1], bodyColor, outlineColor);
}

void renderCube(GLfloat rect1[4][3], GLfloat rect2[4][3], GLfloat rect3[4][3], GLfloat rect4[4][3], GLfloat rect5[4][3], GLfloat rect6[4][3])
{
	rect(rect1);
	rect(rect2);
	rect(rect3);
	rect(rect4);
	rect(rect5);
	rect(rect6);
}

void renderCube(GLfloat rect1[4][3], GLfloat rect2[4][3], GLfloat rect3[4][3], GLfloat rect4[4][3], GLfloat rect5[4][3], GLfloat rect6[4][3], GLfloat faceColors[6][3], GLfloat outlineColor[3])
{
	rect(rect1, faceColors[0], outlineColor);
	rect(rect2, faceColors[1], outlineColor);
	rect(rect3, faceColors[2], outlineColor);
	rect(rect4, faceColors[3], outlineColor);
	rect(rect5, faceColors[4], outlineColor);
	rect(rect6, faceColors[5], outlineColor);
}

void renderCubeTopless(GLfloat rect1[4][3], GLfloat rect2[4][3], GLfloat rect3[4][3], GLfloat rect4[4][3], GLfloat rect5[4][3], GLfloat faceColors[6][3], GLfloat outlineColor[3])
{
	rect(rect1, faceColors[1], outlineColor);
	rect(rect2, faceColors[2], outlineColor);
	rect(rect3, faceColors[3], outlineColor);
	rect(rect4, faceColors[4], outlineColor);
	rect(rect5, faceColors[5], outlineColor);
}