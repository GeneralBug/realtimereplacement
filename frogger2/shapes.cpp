#include "main.h"
shape_globals sg = { false, false, false, 0.5 };

void drawNormals(float x, float y, float z)
{
	//TODO: this
	if (sg.lit) { glDisable(GL_LIGHTING); }

	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(x, y, z);
	glVertex3f(x, y + sg.normal_scale, z);//TODO: fix
	glEnd();
	glColor3f(1, 1, 1);

	if (sg.lit) { glEnable(GL_LIGHTING); }
}

void drawSphere(vec3f centre, float r, float slices, float stacks)
{

	//TODO: normals
	//circle code stolen from Geoff
	//const int slices=8, stacks=8;
	float theta, phi;
	float x1, y1, z1, x2, y2, z2; //x3, y3, z3, x4, y4, z4;
	float sin_phi, sin_phi_step, sin_theta, cos_theta;
	float step_theta = 2.0 * M_PI / slices, step_phi = M_PI / stacks;

	if (sg.wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
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

			glNormal3f(centre.x + x1, centre.y + y1, centre.z + z1);
			glVertex3f(centre.x + x1, centre.y + y1, centre.z + z1);
			if (sg.drawNormals) { drawNormals(centre.x + x1, centre.y + y1, centre.z + z1); }
			glNormal3f(centre.x + x2, centre.y + y2, centre.z + z2);
			glVertex3f(centre.x + x2, centre.y + y2, centre.z + z2);
			if (sg.drawNormals) { drawNormals(centre.x + x2, centre.y + y2, centre.z + z2); }
		}
		glEnd();

	}
	drawAxes(r * 1.5);
}

void drawCylinder(vec3f centre, float r, float slices, float length)
{
	//TODO: fill hole, normals
	float theta, phi = 1;
	float x, y, z1, z2;
	float sin_phi, sin_phi_step, sin_theta, cos_theta;
	float step_theta = 2.0 * M_PI / slices;

	if (sg.wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	z1 = centre.z - (length);
	z2 = centre.z + (length);

	sin_phi = r * sinf(phi);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= slices; i++)
	{
		theta = i / (float)slices * 2.0 * M_PI;

		sin_theta = sinf(theta);
		cos_theta = cosf(theta);

		x = sin_phi * cos_theta;
		y = sin_phi * sin_theta;

		glNormal3f(centre.x + x, centre.y + y, centre.z + z1);
		glVertex3f(centre.x + x, centre.y + y, centre.z + z1);
		if (sg.drawNormals) { drawNormals(centre.x + x, centre.y + y, centre.z + z1); }
		glNormal3f(centre.x + x, centre.y + y, centre.z + z2);
		glVertex3f(centre.x + x, centre.y + y, centre.z + z2);
		if (sg.drawNormals) { drawNormals(centre.x + x, centre.y + y, centre.z + z2); }
	}
	glEnd();
	drawAxes(r * 1.5);
}

void drawLimb(vec2f shoulder)
{
	glPushMatrix();//SHOULDER
		//translate to shoulder
	glTranslatef(shoulder.x, 0, shoulder.y);
	//shoulder rotations for animation go here
	glPushMatrix();//UPPER ARM
		//translate to centre of upper arm
		//render upper arm
	glPushMatrix();//ELBOW
		//translate to elbow
		//elbow rotations for animation go here
	glPushMatrix();//LOWER ARM
		//translate to centre of lower arm
		//render lower arm
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}