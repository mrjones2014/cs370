/******************************************/
/*!!!!!DO NOT MODIFY PAST THIS POINT!!!!!!*/
/******************************************/
// Keyboard callback - DO NOT MODIFY
void keyfunc(unsigned char key, int x, int y)
{

	if (mode>3)
	{
		if (key == 'p')
		{
			spin_flag = !spin_flag;
		}
		if (mode>4)
		{
			if (key == 'r')
			{
				rot_flag = !rot_flag;
			}
		}
	}
		
	// Spacebar changes mode - DO NOT MODIFY
	if (key==' ')
	{
		mode++;
		if (mode > 5)
		{
			mode = 0;
			spin_theta = 0.0f;
			rot_theta = 0.0f;
			spin_flag = true;
			rot_flag = true;
		}
	}
	// Adjust azimuth angle - DO NOT MODIFY
	else if (key == 'a')
	{
		azimuth += daz;
		if (azimuth > 360.0)
		{
			azimuth -= 360.0;
		}
	}
	else if (key == 'd')
	{
		azimuth -= daz;
		if (azimuth < 0.0)
		{
			azimuth += 360.0;
		}
	}
	// Adjust elevation angle - DO NOT MODIFY
	else if (key == 'w')
	{
		elevation += del;
		if (elevation > 180.0)
		{
			elevation = 180.0;
		}
	}
	else if (key == 's')
	{
		elevation -= del;
		if (elevation < 0.0)
		{
			elevation = 0.0;
		}
	}
	// Esc key quits - DO NOT MODIFY
	else if (key==27)
	{
		exit(0);
	}

	// TODO: Compute cartesian camera position
	eye[X] = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
	eye[Y] = (GLfloat)(radius*cos(elevation*DEG2RAD));
	eye[Z] = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
		
	glutPostRedisplay();
}

// Routine to draw pyramid faces
void draw_triangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat color[])
{
	// Draw face
	glBegin(GL_POLYGON);
		glColor3fv(color);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();

	// Draw edge
	glBegin(GL_LINE_LOOP);
		glColor3f(0.0f,0.0f,0.0f);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();
}

// Routine to draw table and target
void draw_table()
{		
	if (mode < 3)
	{
		// Draw y=0 table
		glBegin(GL_POLYGON);
			glColor3f(1.0f,1.0f,1.0f);
			glVertex3f(-TABLE_SIZE,0.0f,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,0.0f,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,0.0f,TABLE_SIZE);
			glVertex3f(-TABLE_SIZE,0.0f,TABLE_SIZE);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f(-TABLE_SIZE,0.0f,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,0.0f,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,0.0f,TABLE_SIZE);
			glVertex3f(-TABLE_SIZE,0.0f,TABLE_SIZE);
		glEnd();

		// Draw target at origin
		glBegin(GL_LINES);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f(-TABLE_SIZE/3.0f,0.01f,0.0f);
			glVertex3f(TABLE_SIZE/3.0f,0.01f,0.0f);
			glVertex3f(0.0f,0.01f,TABLE_SIZE/3.0f);
			glVertex3f(0.0f,0.01f,-TABLE_SIZE/3.0f);
		glEnd();
	}
	else
	{
		// Draw table
		glBegin(GL_POLYGON);
			glColor3f(1.0f,1.0f,1.0f);
			glVertex3f(-TABLE_SIZE,TABLE_Y,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,TABLE_Y,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,TABLE_Y,TABLE_SIZE);
			glVertex3f(-TABLE_SIZE,TABLE_Y,TABLE_SIZE);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f(-TABLE_SIZE,TABLE_Y,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,TABLE_Y,-TABLE_SIZE);
			glVertex3f(TABLE_SIZE,TABLE_Y,TABLE_SIZE);
			glVertex3f(-TABLE_SIZE,TABLE_Y,TABLE_SIZE);
		glEnd();

		// Draw target at location
		glBegin(GL_LINES);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f(TARGET_X-TABLE_SIZE/3.0f,TABLE_Y+0.01f,TARGET_Z);
			glVertex3f(TARGET_X+TABLE_SIZE/3.0f,TABLE_Y+0.01f,TARGET_Z);
			glVertex3f(TARGET_X,TABLE_Y+0.01f,TARGET_Z+TABLE_SIZE/3.0f);
			glVertex3f(TARGET_X,TABLE_Y+0.01f,TARGET_Z-TABLE_SIZE/3.0f);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glColor3f(0.0f,0.0f,0.0f);
			for (float i = 0; i < 6.3; i += 0.1){
				glVertex3f(TARGET_X + CIRCLE_RAD*sin(i), TABLE_Y+0.01f, TARGET_Z + CIRCLE_RAD*cos(i));
			}
		glEnd();
	}
}

// Reshape routine - Fall 2014
void reshape(int w, int h)
{
	glViewport(0,0,w,h);
}

