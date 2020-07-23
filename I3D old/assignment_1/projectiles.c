#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


//const float radius = 0.025;
void drawAxes();
void drawFrog();
void drawQuadratic();
void drawTrajectory();
void drawEnvironment();
void drawObstacles();


float getNextTime(float, float, int);

typedef struct { float x, y; } vec2f;
typedef struct { vec2f r0, v0, r, v; float radius; bool isFrog;} sphere;
typedef struct { vec2f low, high; } rectangle;
typedef struct { vec2f r; float radius; } log;

void drawRectangle(rectangle);

bool detectCollision(sphere*, bool, vec2f);
void collide(sphere*,float);

const sphere default_sphere = 
{ 
	{ -0.80, 0.025 },//r0 = inital coords
	{  0.25, 0.25 },//v0 = max velocity
	{ -0.80, 0.025 },//r = coords
	{  0.00, 0.00 }, //v = velocity
	0.025,
	true
};
sphere frog = 
{
	{ -0.80, 0.025 },
	{  0.25, 0.25 },
	{ -0.80, 0.025 },
	{  0.00, 0.00 },
	0.025,
	true
	
};

const int CAR_COUNT = 3;
const int LOG_COUNT = 3;
const float g = -0.25;
const int milli = 1000;
const float SCALE = 0.01;//unit of shift when changing trajectory
const int SEGMENT_COUNT = 512;

typedef struct {
  bool debug;
  bool go;
  float startTime;
  bool OSD;
  int frames;
  float frameRate;
  float frameRateInterval;
  float lastFrameRateT;
  bool dead;
  bool stationary;
  float wave;
} global_t;

global_t global = { false, true, 0.0, true, 0, 0.0, 0.2, 0.0, false, true, 0.2};
rectangle cars[] = 
{ 
	{{.x = -0.4, .y = 0.05}, {.x = -0.3, .y = 0.2 }},
	{{.x = -0.1, .y = 0.05}, {.x =    0, .y = 0.15}},
	{{.x =  0.1, .y = 0.05}, {.x =  0.2, .y = 0.15}}
};
rectangle road = {{-0.5f, 0.0f},{0.2f, 0.05f}};
rectangle river_bounds = {{0.5f, -0.1f},{0.8f, 0.0f}};

sphere logs[] = 
{
	{{   0, 1 }, {0.25, 0.25}, {   0, 1}, {0.00, 0.00}, 0.025, false},
	{{ 0.2, 1 }, {0.25, 0.25}, { 0.2, 1}, {0.00, 0.00}, 0.025, false},
	{{-0.2, 1 }, {0.25, 0.25}, {-0.2, 1}, {0.00, 0.00}, 0.025, false}
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
	
	//ground
	if(projectile->r.y - projectile->radius < 0)
	{
		collide(projectile, 0);
	}
	
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

// Idle callback for animation
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

		for(i = 0; i < LOG_COUNT; i++)
		{
			updateNumerical(&logs[i], dt);
		};
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
	default:
		break;
	}
	glutPostRedisplay();
}

void specialCB(int key, int x, int y)
{
	//used for arrow keys, not sure why they need their own special function
	if(global.stationary)
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
			frog.v0.y -= SCALE;
			break;
		case GLUT_KEY_UP:
			//printf("up");
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

/*IMPORTANT DRAWING FUNCTIONS*/
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
//TODO: ellipse
	glPushMatrix();
	glTranslatef(frog.r.x, frog.r.y, 0);
	glutWireSphere(frog.radius, 8, 4);
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
	for(i = 0; i < SEGMENT_COUNT + (SEGMENT_COUNT/4); i++)
	{

		t = getNextTime(t, tfinal, SEGMENT_COUNT);
		next.x = (-1*frog.v0.x * t) + frog.r0.x;
		//TODO: account for wall collisions
		next.y = -0.5*g*(t*t) + frog.v0.y * t - frog.r0.y;
		next.y *= -1;
		if(!detectCollision(&frog, true, next) && next.y >= frog.radius)
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

	int i;
	float amp = (river_bounds.high.y - river_bounds.low.y)/2;
	float k = (2 * M_PI)/global.wave;
	float stepSize = (river_bounds.high.x - river_bounds.low.x)/SEGMENT_COUNT;
	float x, y;
	glColor3f(0, 0.1, 1);
	glBegin(GL_LINE_STRIP);
	
	for(i = 0; i <= SEGMENT_COUNT; i++)
	{
		x = i*stepSize+river_bounds.low.x;
		y = amp * sinf((k * x) + (M_PI/4)*time) - amp;// + global.dt / 10);
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
	glColor3f(1,1,1);
	drawRectangle(river_bounds);
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
	for(i = 0; i < LOG_COUNT; i++)
	{
		glPushMatrix();
		glTranslatef(logs[i].r.x, logs[i].r.y, 0);
		glutWireSphere(logs[i].radius, 8, 4);
		glPopMatrix();
	}
}

void drawRectangle(rectangle box)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(box.low.x, box.low.y, 0.0f);
	glVertex3f(box.low.x, box.high.y, 0.0f);
	glVertex3f(box.high.x, box.high.y, 0.0f);
	glVertex3f(box.high.x, box.low.y, 0.0f);
	glEnd();	
}
/*END DRAWING FUNCTIONS*/

float getNextTime(float time, float final_time, int segment_count)
{
	if (global.debug)
		printf("in time: %f\n", time);
	time = time + (final_time/segment_count);
	if (global.debug)
		printf("out time: %f\n", time);
	return time;
}

bool detectCollision(sphere *projectile, bool prediction, vec2f coords)
{
	//all collisions except ground and walls, both frog and logs
	//used for both predictions and actual movement, returns coords of collision
	int i;
	//road
	if(coords.x > road.low.x && coords.x < road.high.x)
	{
		if(coords.y - projectile->radius < road.high.y)
		{
			if(!prediction)
			{
				collide(projectile, road.high.y);
				//printf("ROAD COLLISION aaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
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
					//printf("CAR COLLISION bbbbbbbbbbbbbbbbbbbbbbbbbbbbBBB\n");
				}

				return true;
			}
		}
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