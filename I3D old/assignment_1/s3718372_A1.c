#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


void drawAxes();
void drawFrog();
void drawQuadratic();
void drawTrajectory();
void drawEnvironment();
void drawObstacles();
float getNextTime(float, float);
bool magnitude(float);
void checkTess();

typedef struct { float x, y; } vec2f;
typedef struct { vec2f r0, v0, r, v; float radius; bool isFrog;} sphere;
typedef struct { vec2f low, high; } rectangle;
typedef struct { vec2f r; float radius; } barrel;
typedef struct {
  bool debug;
  bool go;
  float startTime;
  bool OSD;
  int frames;
  float frameRate;
  float frameRateInterval;
  float lastFrameRateT;
  //stuff I added
  bool dead;
  bool stationary;
  float wave;
  int segment_count;
  bool filled;
} global_t;

void drawRectangle(rectangle);
bool detectCollision(sphere*, bool, vec2f);
void collide(sphere*,float);

const int CAR_COUNT = 3;
const int BARREL_COUNT = 3;
const float g = -0.25;
const int milli = 1000;
const float SCALE = 0.01;//unit of shift when changing trajectory

global_t global = 
{ 
	false, true, 0.0, true, 0, 0.0, 0.2, 0.0, false, true, 0.8, 64, false
};
const sphere default_sphere = 
{ 
	{ -0.90, 0.025 },//r0 = inital coords
	{  0.05, 0.05 },//v0 = max velocity
	{ -0.90, 0.025 },//r = coords
	{  0.05, 0.05 }, //v = velocity
	0.02,
	true
};
sphere frog = 
{
	{ -0.90, 0.025 },
	{  0.05, 0.05 },
	{ -0.90, 0.025 },
	{  0.05, 0.05 },
	0.02,
	true
	
};
const rectangle cars[] = 
{ 
	{{-0.4, 0.02}, {-0.3, 0.08 }},
	{{-0.1, 0.02}, { 0,   0.03 }},
	{{ 0.1, 0.02}, { 0.2, 0.1  }}
};
const rectangle road = 
{
	{-0.5f, 0.0f},{0.2f, 0.02f}
};
const rectangle river_bounds = 
{
	{0.3f, -0.1f},{0.8f, 0.0f}
};
barrel barrels[] = 
{
	{{0.4, 0}, 0.02},
	{{0.6, 0}, 0.06},
	{{0.73, 0}, 0.02}
};


void updateNumerical(sphere *projectile, float dt)
{
	// Euler integration

	// Position
	projectile->r.x += projectile->v.x * dt;
	projectile->r.y += projectile->v.y * dt;
	
	// Velocity
	projectile->v.y += g * dt;
  
	//IMPORTANT - COLLISION
	
	//walls
	if(projectile->r.x+projectile->radius > 1)
	{
		projectile->v.x *= -1;
		projectile->v0.x *= -1;
	}
	if(projectile->r.x-projectile->radius < -1)
	{
		projectile->v.x *= -1;
		projectile->v0.x *= -1;
	}
	
	//other objects
	if(!global.dead)
		detectCollision(projectile, false, projectile->r);
}

void update(void)
{
	static float lastT = -1.0;
	float t, dt;
	int i;
	t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;

	if (lastT < 0.0) 
	{
		lastT = t;
		return;
	}

	dt = t - lastT;
	if (global.debug)
		printf("%f %f\n", t, dt);
	if(!global.dead || !global.stationary)
		updateNumerical(&frog, dt);
	lastT = t;

	/* Frame rate */
	dt = t - global.lastFrameRateT;
	if (dt > global.frameRateInterval) {
		global.frameRate = global.frames / dt;
		global.lastFrameRateT = t;
		global.frames = 0;
	}


	
  glutPostRedisplay();
}

void displayOSD()
{
  char buffer[30];
  char *bufp;
  int w, h;
    
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  /* Set up orthographic coordinate system to match the 
     window, i.e. (0,0)-(w,h) */
  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  /* Frame rate */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 60);
  snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frameRate);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Time per frame */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 40);
  snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frameRate * 1000.0);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	//tesselation
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 20);
  snprintf(buffer, sizeof buffer, "tesselation: %d", global.segment_count);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Pop modelview */
  glPopMatrix();  
  glMatrixMode(GL_PROJECTION);

  /* Pop projection */
  glPopMatrix();  
  glMatrixMode(GL_MODELVIEW);

  /* Pop attributes */
  glPopAttrib();
}

void display(void)
{
	if(global.go)
	{
		GLenum err;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glColor3f (1, 1, 1);
		if(global.dead)
			glColor3f(1, 0, 1);
		else
			glColor3f(0, 1, 0);
		drawFrog();
		//trajectory stuff
		if(!global.dead)
		{
			drawQuadratic();
			drawTrajectory();
		}
		//area
		glColor3f (1, 1, 1);
		drawEnvironment();
		drawObstacles();
		drawAxes();
		// Display OSD
		if (global.OSD)
		displayOSD();

		glPopMatrix();
		
		glutSwapBuffers();

		global.frames++;
		// Check for errors
		while ((err = glGetError()) != GL_NO_ERROR)
			printf("%s\n",gluErrorString(err));
	}
}

/*IMPORTANT CONTROL FUNCTIONS*/
void keyboardCB(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	case 'd':
		global.debug = !global.debug;
		break;
	case 'o':
		global.OSD = !global.OSD;
		break;
	//IMPORTANT - JUMP
	case ' ':
		if(global.stationary && !global.dead)
		{
			global.stationary = false;
			frog.v.x = frog.v0.x;
			frog.v.y = frog.v0.y;
		}
		break;
	case 'r':
		frog = default_sphere;
		global.dead = false;
		break;
	case 'g':
		//TODO: confirm validity
		global.go = !global.go;
		break;
	case '-':
		global.segment_count /= 2;
		checkTess();
		break;
	case '=':
		global.segment_count *= 2;
		checkTess();
		break;
	case '_':
		global.wave /= 2;
		checkTess();
		break;
	case '+':
		global.wave *= 2;
		checkTess();
		break;
	case 'f':
		global.filled = !global.filled;
	default:
		break;
	}
	glutPostRedisplay();
}

void specialCB(int key, int x, int y)
{
	//used for arrow keys, not sure why they need their own special function
	if(global.stationary && !global.dead)
	{
		//TODO: establish bounds
		switch(key) 
		{
		case GLUT_KEY_LEFT:
			//printf("left");
			frog.v0.x -= SCALE;
			break;
		case GLUT_KEY_RIGHT:
			//printf("right");
			frog.v0.x += SCALE;
			break;
		case GLUT_KEY_DOWN:
			//printf("down");
			if(magnitude(-1*SCALE))
				frog.v0.y -= SCALE;
			break;
		case GLUT_KEY_UP:
			//printf("up");
			if(magnitude(SCALE))
				frog.v0.y += SCALE;
			break;
		default:
			break;
		}
	}
}
/*END CONTROL FUNCTIONS*/

void myReshape(int w, int h)
{//TODO: find out that this does

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*IMPORTANT DRAWING FUNCTIONS */
void drawAxes()
{
	/* DRAW AXES */
	//x
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();
	//y
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glEnd();
	//z
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();
}

void drawFrog()
{
//IMPORTANT - DRAWING SPHERE
	float angle;
	glPushMatrix();
	//pivot point
	glTranslatef(frog.r.x, frog.r.y-frog.radius, 0);
	//rotation - uses actual velocity if moving
	if(global.stationary)
	{
		angle = atan(frog.v0.y/(frog.v0.x + 0.001)) * 180/M_PI;
		glRotatef(angle - 90, 0, 0, 1);
	}
	else
	{
		angle = atan(frog.v.y/(frog.v.x + 0.001)) * 180/M_PI;
		glRotatef(angle -90, 0, 0, 1);
	}
	//frog position, flips if going backwards
	if(frog.v0.x < 0)
		glTranslatef(0, -1*frog.radius, 0);
	else
		glTranslatef(0, frog.radius, 0);
	//scale, makes it an ellipse
	glScalef(0.6f, 1.0f, 1.0f);
	//actual circle
	if(global.filled)
		glutSolidSphere(frog.radius, global.segment_count, 2);
	else
		glutWireSphere(frog.radius, global.segment_count, 2);

	glPopMatrix();
}

void drawQuadratic()
{
	//IMPORTANT 
	//TODO: change fromula to account for elevation
	int i;
	float tfinal = (2 * frog.v0.y)/g;
	float t = 0;
	vec2f next;
	next.x = frog.r.x;
	next.y = frog.r.y;
	
	glBegin(GL_LINE_STRIP);	
	for(i = 0; i < global.segment_count + (global.segment_count/4); i++)
	{

		t = getNextTime(t, tfinal);
		next.x = (-1*frog.v0.x * t) + frog.r0.x;
		//TODO: account for wall collisions
		next.y = -0.5*g*(t*t) + frog.v0.y * t - frog.r0.y;
		next.y *= -1;
		if(!detectCollision(&frog, true, next))// && next.y >= frog.radius)
		{
			glVertex3f(next.x, next.y-frog.radius, 0.0f);
		}
		else 
		{
			glEnd();
			return;
		}
		if(global.debug)
		{
			printf("===================\n");
			printf("X: %f Y: %f T: %f\n", next.x, next.y, t);
		}
	}
	glEnd();
	
}

void drawTrajectory()
{
	glBegin(GL_LINE_STRIP);
	if(global.stationary)
	{
		glVertex3f(frog.r.x, frog.r.y-frog.radius, 0.0f);
		glVertex3f(frog.r.x + frog.v0.x, frog.r.y + frog.v0.y-frog.radius, 0.0f);
	}
	else 
	{
		glVertex3f(frog.r.x, frog.r.y-frog.radius, 0.0f);
		glVertex3f(frog.r.x + frog.v.x, frog.r.y + frog.v.y-frog.radius, 0.0f);
	}
	glEnd();
}

void drawEnvironment()
{
	static float time = 0;
	//ground
	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	
	//road
	drawRectangle(road);
	
	//water

	int i, j;
	float amp = (river_bounds.high.y - river_bounds.low.y)/2;
	float k = (2 * M_PI)/global.wave;
	float stepSize = (river_bounds.high.x - river_bounds.low.x)/global.segment_count;
	float x, y;
	
	glColor3f(0,0,0.5);
	drawRectangle(river_bounds);
	
	glColor3f(0, 0.3, 1);
	glBegin(GL_LINE_STRIP);
	
	for(i = 0; i <= global.segment_count; i++)
	{
		x = i*stepSize+river_bounds.low.x;
		y = amp * sinf((k * x) + (M_PI/4)*time) - amp;// + global.dt / 10);
		//update logs
		for(j = 0; j < BARREL_COUNT; j++)
		{
			if(barrels[j].r.x <= x + barrels[j].radius && barrels[j].r.x >= x - barrels[j].radius)
			{
				barrels[j].r.y = y;
			}
		}
		
		glVertex3f(x, y, 0.0f);
	}
	glEnd();

	time += 0.05;
}

void drawObstacles()
{
	int i;
	glColor3f(1, 1, 0);
	for(i = 0; i < CAR_COUNT; i++)
	{
		drawRectangle(cars[i]);
	}
	for(i = 0; i < BARREL_COUNT; i++)
	{
		//redraw, updated in drawEnvironment
		glPushMatrix();
		glTranslatef(barrels[i].r.x, barrels[i].r.y, 0);
		if(global.filled)
			glutSolidSphere(barrels[i].radius, global.segment_count, 2);
		else
			glutWireSphere(barrels[i].radius, global.segment_count, 2);
		glPopMatrix();
	}
	
	
}

void drawRectangle(rectangle box)
{
	if(global.filled)
		glBegin(GL_QUADS);
	else
		glBegin(GL_LINE_STRIP);
	glVertex3f(box.low.x, box.low.y, 0.0f);
	glVertex3f(box.low.x, box.high.y, 0.0f);
	glVertex3f(box.high.x, box.high.y, 0.0f);
	glVertex3f(box.high.x, box.low.y, 0.0f);
	glEnd();	
}
/*END DRAWING FUNCTIONS*/

float getNextTime(float time, float final_time)
{
	if (global.debug)
		printf("in time: %f\n", time);
	time = time + (final_time/global.segment_count);
	if (global.debug)
		printf("out time: %f\n", time);
	return time;
}

bool detectCollision(sphere *projectile, bool prediction, vec2f coords)
{
	//all collisions walls
	//used for both predictions and actual movement
	int i;
	float temp;
	//road
	if(coords.x > road.low.x && coords.x < road.high.x)
	{
		if(coords.y - projectile->radius < road.high.y)
		{
			if(!prediction)
			{
				collide(projectile, road.high.y);
			}
			return true;
		}
	}
	
	//cars
	for(i = 0; i < CAR_COUNT; i++)
	{
		if(coords.x > cars[i].low.x && coords.x < cars[i].high.x)
		{
			if(coords.y - projectile->radius < cars[i].high.y)
			{
				if(!prediction)
				{
					collide(projectile, coords.y - projectile->radius);
					if(projectile->isFrog)
					{
						global.dead = true;
					}
				}
				return true;
			}
		}
	}
	
	//barrels
	for(i = 0; i < BARREL_COUNT; i++)
	{
		temp = sqrt(((barrels[i].r.x - coords.x)*(barrels[i].r.x - coords.x)) + ((coords.y - barrels[i].r.y)*(coords.y - barrels[i].r.y)));
		if(temp <= projectile->radius + barrels[i].radius)
		{
			if(!prediction)
			{
				collide(projectile, coords.y);
				//overrides changes in collide to avoid weird janky collision stuff
				projectile->r.y = barrels[i].r.y + barrels[i].radius + projectile->radius;
				projectile->r.x = barrels[i].r.x;
			}
			return true;
		}
	}
	
	//river
	if(coords.x > river_bounds.low.x && coords.x < river_bounds.high.x)
	{
		if(coords.y <= river_bounds.low.y)
		{
			if(!prediction)
			{
				collide(projectile, coords.y - projectile->radius);
				if(projectile->isFrog)
				{
					global.dead = true;
				}
			}
			return true;
		}
	}
	//ground
	else if(coords.y - projectile->radius < 0)
	{
		if(!prediction)	
			collide(projectile, 0);
		return true;
	}
	return false;
}

void collide(sphere *projectile, float collisionHeight)
{
	projectile->r.y = projectile->radius + collisionHeight;
	projectile->v.x = 0;
	projectile->r0.x = projectile->r.x;
	projectile->r0.y = projectile->r.y;
	if(projectile->isFrog)
	{
		global.stationary = true;
	}
}

bool magnitude(float scale)
{
	if(frog.v0.y + scale >= 0.25 || frog.v0.y + scale <= -0.25)
	{
		return false;
	}
	return true;
	
}

void checkTess()
{
	
	if(global.segment_count > 8192)
		global.segment_count = 8192;
	if(global.segment_count < 4)
		global.segment_count = 4;
	if(global.debug)
		printf("%f\n", global.wave);
	if(global.wave > 25)
		global.wave = 25;
	if(global.wave < 0.0008)
		global.wave = 0.0008;
	
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("s3718372 - Assignment 1");
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(specialCB);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(update);
	
	glutMainLoop();
}