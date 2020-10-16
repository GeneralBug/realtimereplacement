#include "main.h"


camera c = { false, 1, 0.01, {0, 0}, {0, 0}, 1, false };

bool axes = true;

void reshape(int w, int h)
{//camera mode stuff
	//perspective camera (somewhat) by me
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, w / h, 0.01, 100);
	glTranslatef(0, 0, -1);
	//	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void drawAxes(float scale)
{//draws a set of axes based on scale
	if (axes)
	{
		//x
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(scale, 0, 0);
		glEnd();
		//y
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, scale, 0);
		glEnd();
		//z
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, scale);
		glEnd();
	}
}

void cameraTransform()
{//transformations on scene, call in display
	glRotatef(c.angle.x, 1, 0, 0);
	glRotatef(c.angle.y, 0, 1, 0);
	glScalef(c.zoom, c.zoom, c.zoom);
}

void toggleAxes()
{
	std::cout << "axes\n";
	axes = !axes;
}
