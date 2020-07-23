/* ===========================================
 * SHAPE DRAWING FUNCTIONS FOR OPENGL GLUT
 * CONTAINS AXES, CAMERA CONTROL
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
 #include "shapes.h"

shape_globals sg = {false, false, false, 0.5};

void drawNormals(float x, float y, float z)
{
	if(sg.lit){glDisable(GL_LIGHTING);}
	
	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y + sg.normal_scale, z);//TODO: fix
	glEnd();
	glColor3f(1, 1, 1);
	
	if(sg.lit){glEnable(GL_LIGHTING);}
}
/*
void drawSquare(vec3f centre, float scale)
{
	//old square drawing code, doesn't work with sin waves
	float half = scale/2;
	vec3f neg_vals = {centre.x - half, centre.y, centre.z - half};
	vec3f pos_vals = {centre.x + half, centre.y, centre.z + half};
	float color[] = {0, 1, 0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
	if(sg.wireframe){glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
	else{glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);}
	//triangle 1
	glBegin(GL_POLYGON);
		glNormal3f(0,1,0);
		glVertex3f(neg_vals.x, centre.y, neg_vals.z);
		glNormal3f(0,1,0);
		glVertex3f(neg_vals.x, centre.y, pos_vals.z);
		glNormal3f(0,1,0);
		glVertex3f(pos_vals.x, centre.y, neg_vals.z);
	glEnd();
	//triangle 2
	glBegin(GL_POLYGON);
		glNormal3f(0,1,0);
		glVertex3f(centre.x - half, centre.y, centre.z + half);
		glNormal3f(0,1,0);
		glVertex3f(centre.x + half, centre.y, centre.z + half);
		glNormal3f(0,1,0);
		glVertex3f(centre.x + half, centre.y, centre.z - half);
	glEnd();
	
	//DRAW NORMALS
	if(sg.normals)
	{
		drawNormals(neg_vals.x, centre.y, neg_vals.z);
		drawNormals(neg_vals.x, centre.y, pos_vals.z);
		drawNormals(pos_vals.x, centre.y, neg_vals.z);
		drawNormals(pos_vals.x, centre.y, pos_vals.z);
	}
}
*/
void drawSphere(vec3f centre, float r, float slices, float stacks)
{
	//circle code stolen from Geoff
	//const int slices=8, stacks=8;
	float theta, phi;
	float x1, y1, z1, x2, y2, z2; //x3, y3, z3, x4, y4, z4;
	float sin_phi, sin_phi_step, sin_theta, cos_theta;
	float step_theta = 2.0 * M_PI / slices, step_phi = M_PI / stacks;

	if(sg.wireframe){glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
	else{glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);}
	for (int j = 0; j <= stacks; j++) 
	{
		phi = j / (float)stacks * M_PI;
		
		z1 = r * cosf(phi);
		z2 = r * cosf(phi + step_phi);
		
		sin_phi = r * sinf(phi);
		sin_phi_step = r * sinf(phi + step_phi);
		
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= slices; i++) {
			theta = i / (float)slices * 2.0 * M_PI;
			
			sin_theta = sinf(theta);
			cos_theta = cosf(theta);
			
			x1 = sin_phi * cos_theta;
			y1 = sin_phi * sin_theta;
			x2 = sin_phi_step * cos_theta;
			y2 = sin_phi_step * sin_theta;

			glNormal3f(x1, y1, z1);
			glVertex3f(x1, y1, z1);
			if(sg.normals){drawNormals(x1, y1, z1);}
			glNormal3f(x2, y2, z2);
			glVertex3f(x2, y2, z2);
			if(sg.normals){drawNormals(x2, y2, z2);}
		}
		glEnd();
	}
}

void drawSquare(vec3f coords_left, vec3f right, vec2f normals)
{
	//coords_left is lower coords_left corner, right is upper right corner
	/*
		|-----|right
		|-----|
	coords_left|-----|
	*/

	//triangle 1
	
	
	//normal drawing:
	if(sg.normals)
	{
		drawNormals(coords_left.x, coords_left.y, coords_left.z);
		drawNormals(right.x, right.y, coords_left.z);
		drawNormals(coords_left.x, coords_left.y, right.z);
		drawNormals(coords_left.x, coords_left.y, right.z);
		drawNormals(right.x, right.y, coords_left.z);
		drawNormals(right.x, right.y, right.z);
	}
}

void drawGrid(int gridSize, float scale, bool wave)
{
	//draws a grid of triangle-squares, scale is broken
	int i, j;
	vec3f coords_left = {-1* scale * gridSize, 0, 0};
	vec3f coords_right = {0,0,0};
	vec2f normals_left = {0,0};
	vec2f normals_right = {0,0};
	float amp = scale;
	float wavelength = gridSize;
	float k = (2 * M_PI)/wavelength;
	float len;
	
	//drawing stuff
	float color[] = {0, 0.5, 1};
	glColor3f(0, 0.5 ,1);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
	if(sg.wireframe){glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
	else{glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);}
	

	for(i = 0; i < 2 * gridSize; i = i + 2)
	{
		coords_left.x = (-0.5 * gridSize) + (i / 2) * scale;
		coords_right.x = coords_left.x + scale;
		for(j = 0; j < (2 * gridSize); j = j + 2)
		{
			coords_left.z = (-0.5 * gridSize) + (j / 2) * scale;
			coords_right.z = coords_left.z + scale;
			//makes the grid into a wave
			coords_left.y = 0;
			coords_right.y;
			if(wave)
			{
				coords_left.y = amp * sinf((k * coords_left.x) + (M_PI/4));// * global.dt / 10);
				coords_right.y = amp * sinf((k * coords_right.x) + (M_PI/4));
				//printf("%f, %f\n", coords.y, coords_right.y);
	
			}
			//calculates normals left
			normals_left.y = 1.0;
			normals_left.x = -1 * (amp * k * cos(k * coords_left.x));
			len = sqrtf(normals_left.x * normals_left.x + normals_left.y * normals_left.y);
			normals_left.x /= len;
			normals_left.y /= len;
			//calculates normals right			
			normals_right.y = 1.0;
			normals_right.x = -1 * (amp * k * cos(k * coords_right.x));
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
}

