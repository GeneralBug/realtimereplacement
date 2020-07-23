#include "utils.h"

int rotAnglex, rotAngley;
int prevx, prevy;
int tess = 16;
float zoom = 1.5;
GLboolean leftClick = false;
GLboolean rightClick = false;
typedef struct {float x; float y; float z;} coords;

/*
	gotta do: grid normals
			sine normals
			sine normals animation
			sine grid
			sine grid normals 
			lighting
			sine grid normals animation 
			sine grid normals animation lighting 
*/
float returnSq(float x)
{
	return x*x;
}

void drawTangent(float a, float k, coords ncoords)
{
  float dydx = a * k * cos(k * ncoords.x);
  float tx = 1;
  float ty = dydx;
  float length = sqrt(returnSq(tx)+returnSq(ty));
  tx /= length;
  ty /= length;

  glBegin(GL_LINES);
  glColor3f(1,1,0);
  glVertex3f(ncoords.x,ncoords.y,ncoords.z);
  glVertex3f(ncoords.x+tx,ncoords.y+ty,ncoords.z);
  glEnd();
}

void drawNormal(float a, float k, coords ncoords)
{
  float dydx = a * k * cos(k * ncoords.x);
  float tx = -dydx;
  float ty = 1;
  float length = sqrt(returnSq(tx)+returnSq(ty));
  tx /= length;
  ty /= length;

  glBegin(GL_LINES);
  glColor3f(1,1,0);
  glVertex3f(ncoords.x,ncoords.y,ncoords.z);
  glVertex3f(ncoords.x+tx,ncoords.y+ty,ncoords.z);
  glEnd();
}

void sineWave()
{
	coords ncoords[tess];
	float left = -1.0;
	float right = 1.0;
	float range = right-left;
	float stepSize = range/(float)tess; 
	float x,y,a,theta,k;

  a=1;
	theta=2;
	k=(2*M_PI)/theta;

	glBegin(GL_LINE_STRIP);
	glColor3f(100,100,100);
	for(int i=0; i<=tess; i++)
	{
		x = i*stepSize + left;
		y = a*sin(k*x);

		glVertex3f(x,y,0);
		ncoords[i].x = x;
		ncoords[i].y = y;
		ncoords[i].z = 0;
      //printf("in loop x: %3.2f y: %3.2f\n", ncoords[i].x, ncoords[i].y);
	}
	glEnd();

	for (int i=0; i<=tess; i++)
	{
    drawNormal(a, k, ncoords[i]);
    drawTangent(a, k, ncoords[i]);
  
/*
		a=1;
		theta=2;
		k=(2*M_PI)/theta;
		x = i*stepSize + left;
		y = a*sin(k*x);

    float dydx = a * k * cos(k * x);
    float tx = -dydx;
    float ty = 1;
    float length = sqrt(returnSq(tx)+returnSq(ty));
    tx /= length;
    ty /= length;

    glBegin(GL_LINES);
	  glColor3f(1,1,0);
	  glVertex3f(x,y,0);
	  glVertex3f(x+tx,y+ty,0);
	  glEnd();
*/
	}
}

void quadStrip()
{
	double x,z; 
	double xStep = 1.0 / tess;                
  double zStep = 1.0 / tess; 

	coords ncoords[4*(tess*tess)];
	int iterator = 0;

	for (int j = 0; j < tess; j++) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLE_STRIP);
		z = -0.5 + j * zStep;
		for (int i = 0; i <= tess; i++) 
		{
			x = -0.5 + i * xStep;
			glVertex3f(x,0,z);
			glVertex3f(x,0,z+zStep);

			ncoords[iterator].x = x;
			ncoords[iterator].y = 0;
			ncoords[iterator].z = z;
			iterator++;
			ncoords[iterator].x = x;
			ncoords[iterator].y = 0;
			ncoords[iterator].z = z+zStep;
			iterator++;
			
		}
		glEnd();                 
	}
	for (int k=0; k<=4*(tess*tess); k++)
	{
		//drawNormal(ncoords[k]);
	}
}

void draw()
{
  glPushMatrix();
  glLoadIdentity();
  
  //camera();
  
  glColor3f(1,1,1);
	sineWave();
  drawAxes();
  
  glPopMatrix();
  glutPostRedisplay();
  
}

void display()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
 
  draw();

  /* Always check for errors! */
  int err;
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

/* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
  case 'q':
    exit(EXIT_SUCCESS);
    break;
  default:
    break;
  }
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(400, 400);
  glutCreateWindow("Vector");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(mouseMotion);
  glutMouseFunc(mouseCB);

  glutMainLoop();

  return EXIT_SUCCESS;
}
