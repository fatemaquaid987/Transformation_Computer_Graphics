

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include "matrix.h"
#include "initShader.h"
#include <math.h>
#include <time.h>
#include<stdio.h>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

GLuint ctm_location;
mat4 ctm = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
int num_vertices;
float last_x = 0, last_y = 0, cur_x = 0, cur_y = 0, theta = 0;
int dome_on = 0;


vec4 v4(float x, float y, float z, float w)
{
	vec4 result = { x, y, z, w };
	return result;
}

mat4 matTrans(mat4 m)                        //Performs transformation of a matrix, rows become columns, columns become rows
{
	mat4 result = {
		{ m.x.x, m.y.x, m.z.x, m.w.x },
		{ m.x.y, m.y.y, m.z.y, m.w.y },
		{ m.x.z, m.y.z, m.z.z, m.w.z },
		{ m.x.w, m.y.w, m.z.w, m.w.w }
	};

	return result;
}

vec4 unit(vec4 v)
{
	GLfloat mag = magnitude(v);
	//printf("%f", mag);
	if (mag <= 0.0001)
	{
		mag = 0.0001;
	}
	vec4 result = scalarMult(1 / mag, v);
	return result;

}
float magnitude(vec4 u)
{
	GLfloat u_mag = sqrt(u.x*u.x + u.y*u.y + u.z*u.z + u.w*u.w);
	return u_mag;
}

vec4 scalarMult(float s, vec4 v)             //Multiplies scalar and 4x1 vector 
{
	vec4 result;
	result.x = s*v.x;
	result.y = s*v.y;
	result.z = s*v.z;
	result.w = s*v.w;
	return result;

}
vec4 vecAdd(vec4 u, vec4 v)                 //Adds 2 4x1 vectors
{
	vec4 result;
	result.x = u.x + v.x;
	result.y = u.y + v.y;
	result.z = u.z + v.z;
	result.w = u.w + v.w;
	return result;
}

vec4 matVecMult(mat4 m, vec4 v)            //multiplies 4x4 matrix with a 4x1 vector resulting in a 4x1 vector.
{
	vec4 result;
	result = vecAdd(vecAdd(scalarMult(v.x, m.x), scalarMult(v.y, m.y)), vecAdd(scalarMult(v.z, m.z), scalarMult(v.w, m.w)));
	return result;
}

mat4 matMult(mat4 m1, mat4 m2)              //Multiplies 2 4x4 matrices resulting in a 4x4 matrix
{
	mat4 result = { matVecMult(m1, m2.x), matVecMult(m1, m2.y), matVecMult(m1, m2.z), matVecMult(m1, m2.w) };
	return result;

}

GLfloat vecDot(vec4 u, vec4 v)               //Performs 4x1 vector dot multiplication , returns a  floating point number
{
	GLfloat d = (u.x*v.x) + (u.y*v.y) + (u.z*v.z) + (u.w*v.w);
	return d;
}

vec4 vecCross(vec4 u, vec4 v)             //Performs 4x1 vector cross multiplication, returns a 4x1 vector
{
	vec4 result;
	result.x = u.y*v.z - u.z*v.y;
	result.y = u.z*v.x - u.x*v.z;
	result.z = u.x*v.y - u.y*v.x;
	result.w = 0;
	return result;
}

mat4 translate(GLfloat x, GLfloat y, GLfloat z) //translation matrix
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	result.w.x = x;
	result.w.y = y;
	result.w.z = z;
	return result;
}


mat4 Scale(GLfloat x, GLfloat y, GLfloat z) //scale matrix
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	result.x.x = x;
	result.y.y = y;
	result.z.z = z;
	return result;
}

mat4 rotate_z(GLfloat theta)  //rotation matrix about z-axis
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	result.x.x = cos(theta);
	result.y.x = -sin(theta);
	result.x.y = sin(theta);
	result.y.y = cos(theta);
	return result;
}

mat4 rotate_x(GLfloat theta) //rotation matrix about x-axis
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	
	result.y.y = cos(theta);
	result.z.y = -sin(theta);
	result.y.z = sin(theta);
	result.z.z = cos(theta);
	
	return result;
}

mat4 rotate_y(GLfloat theta) //rotation matrix about y axis
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	result.x.x = cos(theta);
	result.z.x = sin(theta);
	result.x.z = -sin(theta);
	result.z.z = cos(theta);
	
	return result;
}

mat4 rotate_arb(GLfloat theta, vec4 v) //rotation about arbitrary axis
{
	mat4 result = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } };
	GLfloat d = sqrt((v.y*v.y )+ (v.z*v.z));
	if (d==0)                      // if y and z components are zero, rotate about x-axis
	{
		
		return rotate_x(theta);

	}
	if (d <= 0.0001)          //to avoid division by zero 
	{
		d = 0.0001;
	}
	mat4 rot_x = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } }; //rotation matrix to bring vector on x-z plane

	rot_x.y.y = v.z/d;
	rot_x.z.y = -v.y/d;
	rot_x.y.z = v.y/d;
	rot_x.z.z = v.z/d;


	mat4 rot_y = { { 1.0, 0.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 1.0 } }; //rotation matrix to align vector with z-axis
	rot_y.x.x = d;
	rot_y.z.x = -v.x;
	rot_y.x.z = v.x;
	rot_y.z.z = d;

	
	result = matMult(matTrans(rot_x), matMult(matTrans(rot_y), matMult(rotate_z(theta), matMult(rot_y, rot_x)))); //final rotation matrix

	return result;
}




vec4* spiral(int *num_vertices)                             //creates a spiral
{
	float theta, theta_r, theta10_r, angle, y_s;
	int index = 0;
	y_s = 0;
	*num_vertices = 216*(72*4);                                 //number of vertices in spiral
	vec4 *vertices = (vec4 *)malloc(sizeof(vec4) * (216*(72*4))); //array of vertices

	    //makes one ring of spiral 
		for (theta = 0; theta <= 350; theta = theta + 10)     //loops over and makes triangles 
		{
			theta_r = theta * 3.14 / 180.0;                  //angle 
			theta10_r = (theta + 10) * 3.14 / 180.0;          //angle + 10

			vertices[index] = v4(sin(theta_r), 0.1+y_s, cos(theta_r), 1.0);   //vertex 0
			vertices[index + 1] = v4(sin(theta_r), 0-y_s, cos(theta_r), 1.0); //vertex 1
			
			vertices[index + 4] = v4(sin(theta_r), 0.1+y_s, cos(theta_r), 1.0);//vertex 4
			if (theta <= 170)
			{
				y_s = y_s + 0.006;
			}
			else
			{
				y_s = y_s - 0.006;
			}
			vertices[index + 5] = v4(sin(theta10_r), 0.1+y_s, cos(theta10_r), 1.0);//vertex 5
			vertices[index + 2] = v4(sin(theta10_r), 0 - y_s, cos(theta10_r), 1.0);//vertex 2
			vertices[index + 3] = v4(sin(theta10_r), 0 - y_s, cos(theta10_r), 1.0);   //vertex 3

			index += 6;
		}
		
	float ang_10;
	theta_r = 0;
	theta10_r = 0;
	index = 0;
	for (int i = 0; i < 216; i++)
	{
		vertices[i + 216] = vertices[i]; //moves ring vertices 216 vertices forward
	}
	//makes two circles at the beginning and end of spiral
	for (theta = 0; theta <= 350; theta = theta + 10)     //loops over and makes triangles 
	{
		theta_r = theta * 3.14 / 180.0;                  //angle 
		theta10_r = (theta + 10) * 3.14 / 180.0;          //angle + 10
		vertices[index] = v4(0, 0, 0, 1.0);   //vertex 0
		vertices[index + 1] = v4(sin(theta_r), 0, cos(theta_r), 1.0); //vertex 1
		vertices[index + 2] = v4(sin(theta10_r), 0, cos(theta10_r), 1.0); //vertex 2

		vertices[index+108] = v4(0, 0, 0, 1.0);   //vertex 0
		vertices[index +108+ 1] = v4(sin(theta_r), 0, cos(theta_r), 1.0); //vertex 1
		vertices[index +108+ 2] = v4(sin(theta10_r), 0, cos(theta10_r), 1.0); //vertex 2

		index = index + 3;
	}
	//performs suitable scaling and translation of first 216 vertices(circles)
	for (int i = 0; i < 216; i++)
	{
		vertices[i] = matVecMult(matMult(translate(0.1*sin(0), 0.1*cos(0), 0), Scale(0.5, 0.5, 0.5)), vertices[i]);
		vertices[i] = matVecMult(rotate_y(-3.14 / 2), vertices[i]);

		if (i >= 108)
		{
			vertices[i] = matVecMult(rotate_z(-5 * 3.14 / 180), vertices[i]);
			vertices[i] = matVecMult(translate(0, -0.13, -0.02*(72 * 4-1)), vertices[i]);
			
		}
	}

	//does similar transformations for first ring vertices
	for (int j = 216; j < 216*2; j++)
	{
		
		vertices[j] = matVecMult(matMult(translate(0.1*sin(0), 0.1*cos(0), 0),Scale(0.5,0.5,0.5)), vertices[j]);
		vertices[j] = matVecMult(rotate_y(-3.14 / 2), vertices[j]);
		
	}
	//makes other rings by performing suitable rotation , one on top of the other
	for (int j = 2; j < (72*4); j++)
	{
		
		
			for (int i = 0; i < 216; i++)
			{
				ang_10 = 5 * 3.14 / 180.0;
				vertices[i + (216 * j)] = matVecMult(matMult(translate(0.1*sin(ang_10), 0.1*cos(ang_10), 0), rotate_z(ang_10)), vertices[i + (216 * (j - 1))]);
				vertices[i + (216 * j)] = matVecMult(translate(0, 0, -0.02), vertices[i + (216 * j)]);

			}
		

	}
	//brings spiral at the centre
	for (int j = 0; j < 216 * (72*4); j++)
	{
		
		vertices[j] = matVecMult(rotate_x(3.14/2), vertices[j]);
		vertices[j] = matVecMult(matMult(Scale(0.2, 0.2, 0.2), translate(0.8, -2.7, 0)), vertices[j]);
	}
	return vertices;
}



vec4 *genRandomColors(int num_vertices)         //assignes random colours to each triangle in the cone
{
	GLfloat r, g, b;
	int index = 0, i;

	srand(time(0));

	vec4 *colors = (vec4 *)malloc(sizeof(vec4) * num_vertices);

	for (i = 0; i < num_vertices / 6; i++)
	{
		r = rand() / (float)RAND_MAX;           //random values for r, g, b
		g = rand() / (float)RAND_MAX;
		b = rand() / (float)RAND_MAX;

		colors[index] = v4(r, g, b, 1.0);
		colors[index + 1] = v4(r, g, b, 1.0);
		colors[index + 2] = v4(r, g, b, 1.0);
		colors[index + 3] = v4(r, g, b, 1.0);
		colors[index + 4] = v4(r, g, b, 1.0);
		colors[index + 5] = v4(r, g, b, 1.0);

		index += 6;
	}

	return colors;
}

void init(void)
{
	GLuint program = initShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	int spiral_num_vertices;
	vec4 *spiral_vertices = spiral(&spiral_num_vertices);
	vec4 *spiral_colors = genRandomColors(spiral_num_vertices);
	num_vertices = spiral_num_vertices;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * 2 * num_vertices, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_vertices, spiral_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec4) * num_vertices, spiral_colors);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(vec4) * num_vertices));

	ctm_location = glGetUniformLocation(program, "ctm");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glDepthRange(1, 0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&ctm);

	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	glutSwapBuffers();
}




void keyboard(unsigned char key, int mousex, int mousey)
{
	if (key == 'q')
		exit(0);
	if(key == '3')       //if key is 3, scale down
	{
		ctm = matMult( Scale(1/1.02, 1/1.02, 1/1.02), ctm);
	}
	else if (key == '4') //if key is 4, scale up
	{
		ctm = matMult(Scale(1.02, 1.02, 1.02), ctm);
	}
	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)  //when user presses left mouse button, 
	{
		x = (GLfloat)x;
		y = (GLfloat)y;
		dome_on = 1;                       //left button pressed 
		last_x = cur_x = x;                   //last point = current point
		last_y = cur_y = y;

	}
	else
	{
		dome_on = 0;                        //left button not pressed
	}
}

vec4 getVec(GLfloat x, GLfloat y)            //returns a vector on dome
{
	GLfloat open_x = (GLfloat)(x - 255.0) / 255.0;
	GLfloat open_y = (GLfloat)(y - 255.0) / 255.0;
	open_y = -open_y;
	vec4 u = { 0,0,0,0 };
	if (((open_x*open_x + open_y*open_y) <= 1) && ((open_x <= 1) && (open_x >= -1)) && ((open_y <= 1) && (open_y >= -1))) //if inside the 1-radius dome
	{
		GLfloat z = sqrt(1 - open_x*open_x - open_y*open_y); //z-coordinate
															
		u.x = open_x;
		u.y = open_y;
		u.z = z;
	}
	
	return u;
}
void motion(int x, int y)
{
	x = (GLfloat)x;
	y = (GLfloat)y;
	if (dome_on == 1)                 //if left button is pressed
	{
		cur_x = x;
		cur_y = y;
		vec4 op = getVec(last_x, last_y); //vector 1
		vec4 on = getVec(cur_x, cur_y);   //vector 2
		vec4 zero = { 0,0,0,0 };
		if (((on.x != 0) && (on.y != 0) && (on.z != 0))&& ((on.x != 0) && (on.y != 0) && (on.z != 0))) //if none of the vectors is zero(if inside the 1-radius dome)
		{
			vec4 cross = vecCross(op, on);   //cross product
			float dot = vecDot(op, on); //dot product
			float theta = (dot);
			if (theta >= 1)
			{
				theta = 1;
			}
			else if (theta <= -1)
			{
				theta = -1;
			}
			theta = acos(theta); //theta
			cross = unit(cross); //convert to unit vector
			mat4 m = rotate_arb(theta, cross); //perform rotation
			ctm = matMult(m, ctm); //update ctm
			last_x = cur_x;
			last_y = cur_y; //update last

		}

	}
	glutPostRedisplay();
}
	

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Spiral");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}