#include "teapot.h"

global g = {false, 0.2f, false, false};

float light_pos[] = {1, 1, 1, 0};
float light_colour[] = {1, 1, 1, 1};
float object_colour[] = {1, 0, 0, 1};
float poly = 16;
vec3f origin = {0,0,0};
void drawTeapot(float scale)
{

	glPushMatrix();
	glScalef(scale, scale, scale);
	drawAxes();
	glColor3f(1,1,1);
	if(g.wire)
	{
		glutWireTeapot(scale);
		glutWireTorus(3*scale, 5*scale, poly, poly);
	}
	else
	{
		glutSolidTeapot(scale);
		glutSolidTorus(3*scale, 5*scale, poly, poly);
	}
	glPopMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, object_colour);
}

void drawLight()
{
//	glColor3f()

//	glLightfv(GL_FRONT, GL_SPECULAR, light_colour);
//	glLightfv(GL_FRONT, GL_DIFFUSE, light_colour);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, object_colour);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, object_colour);
}

void keyboardCB (unsigned char key, int x, int y)
{
	switch(key) {
	case 'd':
		g.debug = !g.debug;
		sg.normals = !sg.normals;
		c.debug = !c.debug;
		break;
	case '=':
		g.teapot_scale += 0.01;
		if(g.debug)
			printf("increasing scale: %f\n", g.teapot_scale);
		break;
	case '-':
		g.teapot_scale -= 0.01;
		if(g.debug)
			printf("reducing scale: %f\n", g.teapot_scale);
		break;
	case 'q':
		if(g.debug)
			printf("EXITING VIA KEY\n");
		exit(0);
		break;
	case 'l':
		if(!g.lit)
		{
			glShadeModel(GL_SMOOTH);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}
		g.lit = !g.lit;
		sg.lit = !sg.lit;
		if(g.debug)
			printf("TOGGLING LIGHTING\n");
		break;
	case 'w':
		g.wire = !g.wire;
		sg.wireframe = !sg.wireframe;
		if(g.debug)
			printf("TOGGLING WIREFRAME\n");
		break;
	case ']':
		poly *= 2;
		break;
	case '[':
		poly /= 2;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void display()
{
	int i, j;
	vec3f coords = {-10, 0, 0};
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glPushMatrix();
	
	//camera control
	glRotatef(c.angle.x, 1, 0, 0);
	glRotatef(c.angle.y, 0, 1, 0);
	glScalef(c.zoom, c.zoom, c.zoom);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glRotatef(c.x_angle, c.y_angle, 0);

	if(g.lit)
		glDisable(GL_LIGHTING);
	drawAxes();
	if(g.lit)
		glEnable(GL_LIGHTING);
	
	glColor3f(1, 1, 1);
	//drawTeapot(g.teapot_scale);
	drawSphere(coords, 1, 32, 32);
	/*
	for(i = 0; i < 20; i = i + 2)
	{
		coords.x = -10 + i;
		for(j = 0; j < 20; j = j + 2)
		{
			coords.z = -10 + j;
			//printf("drawing square\n");
			drawSquare(coords, 2);
		}
	}
	*/
	drawLight();
	
	glPopMatrix();
	
	glutSwapBuffers();

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("s3718372 - Assignment 1");
	glutKeyboardFunc(keyboardCB);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseCB);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
//	glutIdleFunc(update);
	
	glutMainLoop();
}