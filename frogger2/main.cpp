/* ===========================================
 * FROGGER WITH OPENGL/GLUT
 * CONTAINS LOTS OF STUFF
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
#include "main.h"

global g = { false, 16, false, false, true, false, -0.25,
	0.0f, 0, 0.0f, 0.2f, 0.0f, 0.0f, false };

float light_pos[] = { 1, 1, 1, 0 };
float light_colour[] = { 0, 0, 0, 0 };
float object_colour[] = { 1, 1, 1, 1 };
float poly = 16;
int i;

actor frog =
{
	{1, 0.3, 0},
	{0.05, 0.05, 0},
	{1, 0.3, 0},
	{0, 0, 0},
	0.3,
	true
};

actor default_frog =
{
	{1, 0.3, 0}, 	//r0
	{0.05, 0.05, 0},//v0
	{1, 0.3, 0},	//r
	{0, 0, 0},		//v
	0.3,
	true
};

rect river_bounds =
{
	{2 * TILE_SIZE, -2, TILE_SIZE},
	{3 * TILE_SIZE, 0, -1 * TILE_SIZE}
};

vec3f cars[] =
{
	{13, CAR_HEIGHT, 4},
	{18, CAR_HEIGHT, 0},
	{23, CAR_HEIGHT, -6},
};

vec3f logs[] =
{
	{40, 0, 4},
	{36, 0, 0},
	{32, 0, -6},
};

rect road =
{
	{10, 0, TILE_SIZE},
	{25, ROAD_HEIGHT, -1 * TILE_SIZE}
};

vec3f ground_colour = { 0, 1, 0 };
vec3f water_colour = { 0, 0.5, 1 };
vec3f road_colour = { 0.2, 0.2, 0.2 };
vec3f sand_colour = { 0.5, 0.5, 0 };

vec3f origin = { 0,0,0 };
//car colours
float colour_body[] = { 1, 0, 0 };
float colour_wheel[] = { 0.1, 0.1, 0.1 };
float colour_log[] = { 0.7, 0.4, 0 };

void init()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

void drawCar(float scale, vec3f pos)
{
	vec3f temp_wheel_pos = pos;
	float y_val;
	//body
	glColor3f(colour_body[0], colour_body[1], colour_body[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour_body);
	drawSphere(pos, scale, g.tesselation, g.tesselation);
	y_val = pos.y - scale;
	scale /= 2;

	glColor3f(colour_wheel[0], colour_wheel[1], colour_wheel[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour_wheel);
	//wheel 1
	temp_wheel_pos.x += scale;
	temp_wheel_pos.y = y_val;
	temp_wheel_pos.z += scale;

	drawSphere(temp_wheel_pos, scale / 2, g.tesselation, g.tesselation);
	temp_wheel_pos = pos;
	glColor3f(colour_wheel[0], colour_wheel[1], colour_wheel[2]);
	//wheel 2
	temp_wheel_pos.x += scale;
	temp_wheel_pos.y = y_val;
	temp_wheel_pos.z -= scale;

	drawSphere(temp_wheel_pos, scale / 2, g.tesselation, g.tesselation);
	temp_wheel_pos = pos;
	glColor3f(colour_wheel[0], colour_wheel[1], colour_wheel[2]);
	//wheel 3
	temp_wheel_pos.x -= scale;
	temp_wheel_pos.y = y_val;
	temp_wheel_pos.z += scale;

	drawSphere(temp_wheel_pos, scale / 2, g.tesselation, g.tesselation);
	temp_wheel_pos = pos;
	glColor3f(colour_wheel[0], colour_wheel[1], colour_wheel[2]);
	//wheel 4
	temp_wheel_pos.x -= scale;
	temp_wheel_pos.y = y_val;
	temp_wheel_pos.z -= scale;

	drawSphere(temp_wheel_pos, scale / 2, g.tesselation, g.tesselation);
}

void drawFrog(float scale)
{
	/*



	vec2f arm_right;
	vec2f arm_left;
	vec2f leg_right;
	vec2f leg_left;
	vec3f origin = {0,0,0};
	drawSphere(origin, scale, 64, 64);
	*/

	//IMPORTANT - DRAWING SPHERE
	/*
	float angle;
	glPushMatrix();
	//pivot point
	glTranslatef(frog.r.x, -1*frog.r.y, frog.r.z);
	//rotation - uses actual velocity if moving
	if(g.stationary)
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
	glScalef(0.6f * scale, 1.0f * scale, 0.6f * scale);
	//actual circle
	*/
	float colour[] = { 0, 1, 0.5 };
	if (g.dead)
	{
		colour[0] = 1;
		colour[1] = 0;
		colour[2] = 1;
	}
	vec3f origin = { 0,0,0 };
	glColor3f(colour[0], colour[1], colour[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour);
	drawSphere(origin, scale, g.tesselation, g.tesselation);

	//glPopMatrix();

	/*
	glPushMatrix();//TORSO
		//translate to frog position
		glTranslatef(0, 0, 0);
		//render torso
		//glutWireCube(scale);
		glPushMatrix();//NECK
			//translate to neck
			glTranslatef(1, 0, 0);
			//neck rotations for animation go here

			glPushMatrix();//HEAD
				//translate to centre of head
				glTranslatef(0.5, 0, 0);
				//render head
				//glutWireCube(scale);
			glPopMatrix();
		glPopMatrix();

		drawLimb(arm_right);
		drawLimb(arm_left);
		drawLimb(leg_right);
		drawLimb(leg_left);

	glPopMatrix();
	*/

}

void drawGrid(int gridSize, float scale, bool wave, vec3f colour)
{
	//draws a grid of triangle-squares
	static float noise = 0;
	static float time_scale = 0.005;

	//TODO: draw normals
	int i, j, l;
	vec3f coords_left = { 0, 0, 0 };
	vec3f coords_right = { 0,0,0 };
	vec2f normals_left = { 0,0 };
	vec2f normals_right = { 0,0 };
	float amp = 1;
	float wavelength = gridSize;

	float k = (2 * M_PI) / wavelength;//TODO: add time stuff for animation
	float len;
	//drawing stuff
	float color[] = { colour.x, colour.y, colour.z };//TODO: fix water colour
	if (wave)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glColor4f(colour.x, colour.y, colour.z, 0.5);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color);
	if (g.wire) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	noise += time_scale;

	float unit_length = scale / gridSize / 2;
	for (i = 0; i < gridSize; i++)
	{
		coords_left.x = (unit_length * i);
		coords_right.x = coords_left.x + unit_length;
		for (j = 0; j < gridSize; j++)
		{
			coords_left.z = (unit_length * j);
			coords_right.z = coords_left.z + unit_length;
			//makes the grid into a wave
			coords_left.y = 0;
			coords_right.y = 0;
			if (wave)
			{

				coords_left.y = amp * sinf((k * coords_left.x) + (M_PI / 4) * noise);
				coords_right.y = amp * sinf((k * coords_right.x) + (M_PI / 4) * noise);
				//part that makes the logs bob, doesn't work
				for (l = 0; l < LOG_COUNT; l++)
				{
					if ((logs[l].x >= coords_left.x - LOG_RADIUS && logs[l].x <= coords_right.x + LOG_RADIUS)
						)//	|| (logs[l].x >= coords_right.x && logs[l].x <= coords_right.x ))
					{
						cout >> ("adjusting log height\n");
						logs[l].y = coords_left.y;
					}
				}
			}
			//normals
			if (wave)
			{
				normals_left.y = 1.0;
				normals_left.x = -1 * (amp * k * cos(k * coords_left.x));
				normals_right.y = 1.0;
				normals_right.x = -1 * (amp * k * cos(k * coords_right.x));
			}
			else
			{
				//TODO: fix
				normals_left.y = 1;
				normals_left.x = 0;
				normals_right.y = 1;
				normals_right.x = 0;
			}
			len = sqrtf(normals_left.x * normals_left.x + normals_left.y * normals_left.y);
			normals_left.x /= len;
			normals_left.y /= len;
			len = sqrtf(normals_right.x * normals_right.x + normals_right.y * normals_right.y);
			normals_right.x /= len;
			normals_right.y /= len;


			glBegin(GL_POLYGON);
			glNormal3f(normals_left.x, normals_left.y, 0);
			glVertex3f(coords_left.x, coords_left.y, coords_left.z);

			glNormal3f(normals_right.x, normals_right.y, 0);
			glVertex3f(coords_right.x, coords_right.y, coords_left.z);

			glNormal3f(normals_left.x, normals_left.y, 0);
			glVertex3f(coords_left.x, coords_left.y, coords_right.z);

			glEnd();
			//triangle 2
			glBegin(GL_POLYGON);
			glNormal3f(normals_left.x, normals_left.y, 0);
			glVertex3f(coords_left.x, coords_left.y, coords_right.z);

			glNormal3f(normals_right.x, normals_right.y, 0);
			glVertex3f(coords_right.x, coords_right.y, coords_left.z);

			glNormal3f(normals_right.x, normals_right.y, 0);
			glVertex3f(coords_right.x, coords_right.y, coords_right.z);

			glEnd();
		}
	}
	if (wave)
	{
		glDisable(GL_BLEND);
	}
	drawAxes(scale / 10);
}

bool magnitude(float scale)
{
	if (frog.v0.y + scale >= 1.25 || frog.v0.y + scale <= 0)
	{
		return false;
	}
	return true;
}

float getNextTime(float time, float final_time)
{
	if (g.debug)
		cout >> ("in time: %f\n", time);
	time = time + (final_time / g.tesselation);
	if (g.debug)
		cout >> ("out time: %f\n", time);
	return time;
}

void collide(actor* projectile, float collisionHeight)
{
	projectile->r.y = projectile->radius + collisionHeight;
	projectile->v.x = 0;
	projectile->v.z = 0;
	projectile->r0.x = projectile->r.x;
	projectile->r0.z = projectile->r.z;
	projectile->r0.y = projectile->r.y;
	if (projectile->isFrog)
	{
		g.stationary = true;
	}
}

float calcDistance(vec3f p1, vec3f p2)
{
	float distance = 0;
	float disX = (p2.x - p1.x);
	float disY = (p2.y - p1.y);
	float disZ = (p2.z - p1.z);
	distance = sqrt(disX * disX + disY * disY + disZ * disZ);
	return distance;
}

bool detectCollision(actor* projectile, bool prediction, vec3f coords)
{
	//TODO: collisions with cars while stationary
	//all collisions, except 
	//used for both predictions and actual movement

	float temp;

	//walls TODO: fix trajectory drawing
	//x plane
	if (((coords.x - projectile->radius) < 0) || (coords.x + projectile->radius > 4 * TILE_SIZE))
	{
		if (!prediction)
		{
			projectile->v.x *= -1;
		}
		return true;
	}
	//z plane
	if ((coords.z - projectile->radius < -0.5 * TILE_SIZE) || (coords.z + projectile->radius > 0.5 * TILE_SIZE))
	{
		if (!prediction)
		{
			projectile->v.z *= -1;
		}
		return true;
	}

	//road
	if (coords.x > road.low.x&& coords.x < road.high.x)
	{
		if (coords.y - projectile->radius < road.high.y)
		{
			if (!prediction)
			{
				collide(projectile, road.high.y);
			}
			return true;
		}
	}

	//cars
	for (i = 0; i < CAR_COUNT; i++)
	{
		if (calcDistance(coords, cars[i]) <= projectile->radius + CAR_SIZE)
		{
			if (!prediction)
			{
				collide(projectile, coords.y - projectile->radius);
				if (projectile->isFrog)
				{
					g.dead = true;
				}
			}
			return true;

		}
	}

	//logs
	for (i = 0; i < LOG_COUNT; i++)
	{
		if (projectile->r.z >= logs[i].z - LOG_RADIUS && projectile->r.z <= logs[i].z + LOG_RADIUS)
		{
			if (calcDistance(coords, logs[i]) <= projectile->radius + LOG_RADIUS)
			{
				if (!prediction)
				{
					collide(projectile, coords.y);
					//overrides changes in collide to avoid weird janky collision stuff
					projectile->r.y = logs[i].y + LOG_RADIUS + projectile->radius;
					projectile->r.x = logs[i].x;
					projectile->r.z = logs[i].z;
				}
				return true;
			}
		}
	}

	//river
	if (coords.x > river_bounds.low.x&& coords.x < river_bounds.high.x)
	{
		//z irrelevant since you can never be outside it
		if (coords.y <= river_bounds.low.y)
		{
			if (!prediction)
			{
				collide(projectile, coords.y - projectile->radius);
				if (projectile->isFrog)
				{
					g.dead = true;
				}
			}
			return true;
		}
	}

	//ground
	else if (coords.y - projectile->radius < 0)
	{
		if (!prediction)
			collide(projectile, 0);
		return true;
	}
	return false;
}

void drawTrajectory()
{
	if (g.lit) { glDisable(GL_LIGHTING); }
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	glVertex3f(frog.r.x, frog.r.y - frog.radius, frog.r.z);
	if (g.stationary)
	{
		glVertex3f(frog.r.x + frog.v0.x, frog.r.y + frog.v0.y - frog.radius, frog.r.z + frog.v0.z);
	}
	else
	{
		glVertex3f(frog.r.x + frog.v.x, frog.r.y + frog.v.y - frog.radius, frog.r.z + frog.v.z);
	}
	glEnd();
	if (g.lit) { glEnable(GL_LIGHTING); }
}

void drawQuadratic()
{
	//IMPORTANT 
	//TODO: change fromula to account for elevation
	if (g.lit) { glDisable(GL_LIGHTING); }
	glColor3f(1, 1, 1);

	float tfinal = (2 * frog.v0.y) / g.g;
	float t = 0;
	vec3f next;
	next.x = frog.r.x;
	next.y = frog.r.y;
	next.z = frog.r.z;

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < SEGMENT_COUNT + (SEGMENT_COUNT / 4); i++)
	{

		t = getNextTime(t, tfinal);
		next.x = (-1 * frog.v0.x * t) + frog.r0.x;
		next.z = (-1 * frog.v0.z * t) + frog.r0.z;
		//TODO: account for wall collisions
		next.y = -0.5 * g.g * (t * t) + frog.v0.y * t - frog.r0.y;
		next.y *= -1;
		if (!detectCollision(&frog, true, next))// && next.y >= frog.radius)
		{
			glVertex3f(next.x, next.y - frog.radius, next.z);
		}
		else
		{
			glEnd();
			return;
		}
		if (g.debug)
		{
			cout >> ("===================\n");
			cout >> ("X: %f Y: %f Z: %f T: %f\n", next.x, next.y, next.z, t);
		}
	}
	glEnd();
	if (g.lit) { glEnable(GL_LIGHTING); }

}

void updateNumerical(actor* actor, float dt)
{
	// Euler integration
	//TODO: calculate distance to something using pythag
	float car_speed = SCALE;
	float log_speed = SCALE;
	// Position
	actor->r.x += actor->v.x * dt;
	actor->r.y += actor->v.y * dt;
	actor->r.z += actor->v.z * dt;
	// Velocity
	actor->v.y += g.g * dt;

	//cars
	for (i = 0; i < CAR_COUNT; i++)
	{
		cars[i].z += car_speed;
		car_speed *= 1.01;
		if (cars[i].z >= TILE_SIZE / 2)
		{
			cars[i].z = -1 * TILE_SIZE / 2;
			car_speed *= 0.98;
		}
	}

	//logs

	for (i = 0; i < LOG_COUNT; i++)
	{
		logs[i].z -= log_speed;
		log_speed *= 1.01;
		if (logs[i].z - LOG_SIZE * 2 < -1 * TILE_SIZE / 2)
		{
			logs[i].z = (TILE_SIZE / 2) - LOG_SIZE * 2;
			log_speed *= 0.98;
		}
	}

	if (!g.dead) {}
	detectCollision(actor, false, actor->r);
}

void drawLight()
{
	//	glColor3f()
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	//glLightfv(GL_FRONT, GL_SPECULAR, light_colour);
	//glLightfv(GL_FRONT, GL_DIFFUSE, light_colour);

	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, object_colour);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, object_colour);
	//glMaterialfv(GL_BACK, GL_AMBIENT, light_colour);
}

void drawEnvironment()
{
	//ground
	glTranslatef(0, 0, -0.5 * TILE_SIZE);
	drawGrid(g.tesselation, 2 * TILE_SIZE, false, ground_colour);
	//ground 2
	glTranslatef(TILE_SIZE, 0, 0);
	drawGrid(g.tesselation, 2 * TILE_SIZE, false, ground_colour);
	//riverbed
	glTranslatef(TILE_SIZE, -2, 0);
	drawGrid(g.tesselation, 2 * TILE_SIZE, false, sand_colour);
	//ground 3
	glTranslatef(TILE_SIZE, 2, 0);
	drawGrid(g.tesselation, 2 * TILE_SIZE, false, ground_colour);
	//road
	glTranslatef(10 - (3 * TILE_SIZE), ROAD_HEIGHT, 0);
	drawGrid(g.tesselation, 2 * TILE_SIZE, false, road_colour);
	//water
	glTranslatef(20, -1 * (1 + ROAD_HEIGHT), 0);
	drawGrid(g.tesselation, 2 * TILE_SIZE, true, water_colour);


}

void keyboardCB(unsigned char key, int x, int y)
{
	switch (key) {
	case 'g':
		g.pause = !g.pause;

		break;
	case '`':
		g.debug = !g.debug;
		c.debug = !c.debug;
		break;
	case 'a':
		frog.v0.z -= SCALE;
		break;
	case 'd':
		frog.v0.z += SCALE;
		break;
	case 'n':
		sg.drawNormals = !sg.drawNormals;
		break;
	case '=':
		//TODO: add tesselation limit
		g.tesselation *= 2;
		if (g.debug)
			cout >> ("increasing tesselation: %f\n", g.tesselation);
		break;
	case '-':
		if (g.tesselation > 4)
			g.tesselation /= 2;

		if (g.debug)
			cout >> ("reducing tesselation: %f\n", g.tesselation);
		break;
	case 27:
	case 'q':
		if (g.debug)
			cout >> ("EXITING VIA KEY\n");
		exit(EXIT_SUCCESS);
		break;
	case 'l':
		if (!g.lit)
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
		}
		else
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}
		g.lit = !g.lit;
		sg.lit = !sg.lit;
		if (g.debug)
			cout >> ("TOGGLING LIGHTING\n");
		break;
	case 'f':
		g.wire = !g.wire;
		sg.wireframe = !sg.wireframe;
		if (g.debug)
			cout >> ("TOGGLING WIREFRAME\n");
		break;
	case ']':
		poly *= 2;
		break;
	case '[':
		poly /= 2;
		break;
	case ' ':
		if (g.stationary && !g.dead)
		{
			g.stationary = false;
			frog.v.x = frog.v0.x;
			frog.v.y = frog.v0.y;
			frog.v.z = frog.v0.z;
			if (g.debug)
				cout >> ("jump\n");
		}
		break;
	case 'x':
		toggleAxes();
		break;
	case 'r':
		frog = default_frog;
		g.dead = false;
		break;
	case '1':
		cout >> ("r:   x: %f y: %f z: %f\nr0:  x: %f y: %f z: %f\nv:   x: %f y: %f z: %f\nv0:  x: %f y: %f z: %f\n",
			frog.r.x, frog.r.y, frog.r.z, frog.r0.x, frog.r0.y, frog.r0.z, frog.v.x, frog.v.y, frog.v.z, frog.v0.x, frog.v0.y, frog.v0.z);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void specialCB(int key, int x, int y)
{
	//used for arrow keys, not sure why they need their own special function
	if (g.stationary && !g.dead)
	{
		//TODO: establish bounds
		switch (key)
		{
		case GLUT_KEY_LEFT:
			//cout >> ("left");
			if (frog.v0.x > MAX_X * -1)
				frog.v0.x -= SCALE;
			break;
		case GLUT_KEY_RIGHT:
			//cout >> ("right");
			if (frog.v0.x < MAX_X)
				frog.v0.x += SCALE;
			break;
		case GLUT_KEY_DOWN:
			//cout >> ("down");
			if (magnitude(-1 * SCALE))
				frog.v0.y -= SCALE;
			break;
		case GLUT_KEY_UP:
			//cout >> ("up");
			if (magnitude(SCALE))
				frog.v0.y += SCALE;
			break;
		default:
			break;
		}
	}
}

void displayOSD()
{
	char buffer[30];
	char* bufp;
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
	sncout >> (buffer, sizeof buffer, "fr (f/s): %6.0f", g.frameRate);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	/* Time per frame */
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(10, 40);
	sncout >> (buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / g.frameRate * 1000.0);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	//tesselation
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(10, 20);
	sncout >> (buffer, sizeof buffer, "tesselation: %d", g.tesselation);
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

void display()
{
	//GLenum err;

	vec3f car = { 10, 1, 0 };
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	cameraTransform();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//draw frog first so it's always centred
	drawLight();
	drawFrog(frog.radius);

	//apply translation based on frogs position, so everything else moves
	glTranslatef(-frog.r.x, -frog.r.y, -frog.r.z);


	if (!g.dead)
	{
		drawTrajectory();
		drawQuadratic();
	}
	if (g.lit)
		glDisable(GL_LIGHTING);
	drawAxes(50);

	if (g.lit)
		glEnable(GL_LIGHTING);

	for (i = 0; i < CAR_COUNT; i++)
	{
		drawCar(CAR_SIZE, cars[i]);
	}

	for (i = 0; i < LOG_COUNT; i++)
	{
		glColor3f(colour_log[0], colour_log[1], colour_log[2]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour_log);
		drawCylinder(logs[i], LOG_RADIUS, g.tesselation, LOG_SIZE);
	}

	drawEnvironment();


	displayOSD();
	glPopMatrix();

	glutSwapBuffers();

	g.frames++;
	// Check for errors
	//while ((err = glGetError()) != GL_NO_ERROR)
	//	cout >> ("%s\n",gluErrorString(err));


}

void update(void)
{
	static float lastT = -1.0;
	float t, dt;

	t = glutGet(GLUT_ELAPSED_TIME) / (float)MILLI - g.startTime;
	if (lastT < 0.0)
	{
		lastT = t;
		return;
	}

	dt = t - lastT;
	g.time = dt;
	if (g.debug)
		cout >> ("%f %f\n", t, dt);
	if ((!g.dead || !g.stationary) && !g.pause)
		updateNumerical(&frog, dt);
	lastT = t;

	/* Frame rate */
	dt = t - g.lastFrameRateT;
	if (dt > g.frameRateInterval) {
		g.frameRate = g.frames / dt;
		g.lastFrameRateT = t;
		g.frames = 0;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("s3718372 - Assignment 2");
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(specialCB);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseCB);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(update);
	init();
	glutMainLoop();
}