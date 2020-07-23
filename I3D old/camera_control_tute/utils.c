/* ===========================================
 * UTILITY FUNCTIONS FOR OPENGL/GLUT
 * CONTAINS AXES, CAMERA CONTROL
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
#include "utils.h"

camera c = {false, 1, 0.01, {0, 0}, {0, 0}, 1, false};
bool axes = true;

void reshape(int w, int h)
{//camera mode stuff
	//perspective camera (somewhat) by me
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, w / h, 0.01, 100);
	glTranslatef(0,0,-1);
//	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
/*
void reshape (int w, int h)
{
	//orthographic camera from the internet
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}
*/
void drawAxes(float scale)
{//draws a set of axes based on scale
	if(axes)
	{
		//x
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(scale,0,0);
		glEnd();
		//y
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,scale,0);
		glEnd();
		//z
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,0,scale);
		glEnd();
	}
}

void mouseMotion(int x, int y)
{//sets rotation for the scene when dragging mouse

	float deltaX, deltaY;
	if(c.debug)
		printf("x: %f\ny: %f\n", x, y);
	if(c.rotating)
	{
		deltaX = x - c.last.x;
		deltaY = y - c.last.y;
		
		c.angle.y += deltaX * c.scale;
		c.angle.x += deltaY * c.scale;
		
		c.last.x = x;
		c.last.y = y;
	}
	glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{//mouse controls for camera
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
		//sets the last position to the current mouse position to avoid jumping
			if(state == GLUT_DOWN)
			{
				c.rotating = true;
				c.last.x = x;
				c.last.y = y;
			}
			if(state == GLUT_UP)
			{
				c.rotating = false;
			}
			break;
		//zoom
		case 3://scroll wheel up
			if(state == GLUT_UP)
				break;
			c.zoom += c.zoom_scale;
			if(c.debug)
				printf("zooming in %f\n", c.zoom);
			break;
		case 4://scroll wheel down
			if(state == GLUT_UP)
				break;
			c.zoom -= c.zoom_scale;
			//clamping values
			if(c.zoom <= 0)
				c.zoom = c.zoom_scale;
			if(c.debug)
				printf("zooming in %f\n", c.zoom);
			break;
		default:
			break;
	}
	
	
	glutPostRedisplay();
}

void cameraTransform()
{//transformations on scene, call in display
	glRotatef(c.angle.x, 1, 0, 0);
	glRotatef(c.angle.y, 0, 1, 0);
	glScalef(c.zoom, c.zoom, c.zoom);
}

void toggleAxes()
{
	axes != axes;
}