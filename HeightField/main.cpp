/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: jpaone_a1
 *  File: main.cpp
 *
 *  Description:
 *      A1 Solution
 *
 *      Working VAO & VBO
 *      Blinn-Phong Shaders
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *  
 *  Notes:
 *
 */

//*************************************************************************************

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <string>

#include "./include/OpenGLUtils.h"
#include "./include/ShaderUtils.h"
#include "./include/ShaderProgram.h"

//*************************************************************************************

// Structure definitions

struct Vertex {
	float px, py, pz;
	float nx, ny, nz;
};

// specify our Cube Vertex information
const Vertex cubeVertices[] = {
	// near
	{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f }, // 0 - bln
	{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f }, // 1 - brn
	{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f }, // 2 - trn
	{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f }, // 3 - tln

												// back
	{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f }, // 4 - blf
	{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f }, // 5 - brf
	{ 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f }, // 6 - trf
	{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f }, // 7 - tlf

											   // right
	{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f }, // 8 - brn
	{ 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f }, // 9 - trn
	{ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f }, // 10 - brf
	{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f }, // 11 - trf

											  // top
	{ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f }, // 12 - trn
	{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f }, // 13 - tlf
	{ 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f }, // 14 - trf
	{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f }, // 15 - tln

											   // bottom
	{ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f }, // 16 - bln
	{ 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f }, // 17 - brn
	{ -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f }, // 18 - blf
	{ 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f }, // 19 - brf

											   // left
	{ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f }, // 20 - bln
	{ -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f }, // 21 - blf
	{ -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f }, // 22 - tln
	{ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f }  // 23 - tlf
};
// specify our Cube Index Ordering
const unsigned short cubeIndices[] = {
	0, 2, 1,   0, 3, 2, // near
	8, 9,10,  10, 9,11, // right
	12,13,14,  15,13,12, // top
	16,17,18,  17,19,18, // bottom
	4, 5, 6,   4, 6, 7, // back
	20,21,22,  21,23,22  // left
};

// specify our Ground Vertex Information
Vertex groundVertices[] = {
		{ 0.0f,	 -0.1f, 0.0f,	0.0f, 1.0f, 0.0f }, // 0 - BL
		{ 15.0f, -0.1f, 0.0f,	0.0f, 1.0f, 0.0f }, // 1 - BR
		{ 15.0f, -0.1f, 15.0f,	0.0f, 1.0f, 0.0f }, // 2 - TR
		{ 0.0f,	 -0.1f, 15.0f,	0.0f, 1.0f, 0.0f }  // 3 - TL
};
// specify our Ground Index Ordering
const unsigned short groundIndices[] = {
	0, 2, 1, 	0, 3, 2
};

struct BlinnPhongUniformLocations {
	GLint modelviewMatrix;
	GLint mvpMatrix;
	GLint normalMatrix;
	GLint lightPosition;
	GLint La;
	GLint Ld;
	GLint Ls;
	GLint Ka;
	GLint Kd;
	GLint Ks;
	GLint shininess;
} bpUniformLocs;

struct BlinnPhongAttributeLocations {
	GLint position;
	GLint normal;
} bpAttribLocs;

struct PointUniformLocations {
	GLint modelviewMatrix;
	GLint mvpMatrix;
	GLint normalMatrix;
	GLint lightPosition;
	GLint La;
	GLint Ld;
	GLint Ls;
	GLint Ka;
	GLint Kd;
	GLint Ks;
	GLint shininess;
} pUniformLocs;

struct PointAttributeLocations {
	GLint position;
} pAttribLocs;

//*************************************************************************************
//
// Global Parameters

int windowWidth, windowHeight;
bool shiftDown = false;
bool leftMouseDown = false;
glm::vec2 mousePosition(-9999.0f, -9999.0f);

glm::vec3 cameraAngles(-0.4f, 2.0f, 150.0f);
glm::vec3 eyePoint(10.0f, 10.0f, 10.0f);
glm::vec3 lookAtPoint(0.0f, 0.0f, 0.0f);
glm::vec3 upVector(0.0f, 1.0f, 0.0f);

glm::vec3 materialKa(0.0215, 0.0215, 0.3745);
glm::vec3 materialKd(0.07568, 0.07568, 0.81424);
glm::vec3 materialKs(0.333, 0.333, 0.467811);
float materialShininess = 0.6*128;

glm::vec3 lightPosition(3, 5, 3);
glm::vec3 lightLd(1, 1, 1);
glm::vec3 lightLa(1, 1, 1);
glm::vec3 lightLs(1, 1, 1);

CSCI441::ShaderProgram *blinnPhongShaderProgram = NULL;
CSCI441::ShaderProgram *pointShaderProgram = NULL;

const GLuint CUBE = 0, GROUND = 1;
GLuint vaods[2];

//*************************************************************************************

//**************************************** Height Field ********************************
//Variables
GLuint vaoHeightField;
float *HeightFieldVertices;
float **u, **v, **r, *g;
float v_tot, v_cur, v_avg;
int heightFieldWidth, heightFieldDepth;
bool drawObject = true;

//Location and size of a unit cube centered at location
glm::vec3 cubeSize(5.9, 5.9, 5.9);
glm::vec3 cubeLoc(40, 20, 40);
glm::vec3 cubeVelocity(0.0, 0.0, 0.0);
//unit is meter cube is wood
double cubeMass = 510 * cubeSize.x * cubeSize.y * cubeSize.z;
double gravity = 2* 9.8;
double fluidDensity = 1000;

// C is wave speed, timeStep is self explanatory, h is the relative width of a column to wave speed
float c = 23, timeStep = 1.0 / 60.0, h = 1;
bool pause   = true,	// default to paused
	 pausing = false,	// pause after 1 tick
	 wrap    = false,	// no wrap => clamp?
	 clamp   = true,	// no clamp => ghost			// this is probably the best option as of now
	 ghost   = false;	// use g[] as ghost? (or avg)

//width and height > 1
void hfInitialize(int width, int depth) {
	heightFieldWidth = width;
	heightFieldDepth = depth;

	groundVertices[1].px = groundVertices[2].px = groundVertices[2].pz = groundVertices[3].pz = 1.0*heightFieldWidth;
	lookAtPoint.x = width / 2;
	lookAtPoint.z = depth / 2;

	//VBO for vertex position
	//(x,y,z,x,y,z,.....
	//y = u
	HeightFieldVertices = new float[width*depth*3];
	
	//Declare arrays
	u = new float*[width];
	v = new float*[width];
	r = new float*[width];
	g = new float[2*width+2*depth];	//don't need corners
	
	//Initialize arrays
	v_tot = 0;
	for (int x = 0; x < width; x++) {
		u[x] = new float[depth];
		v[x] = new float[depth];
		r[x] = new float[depth];
		for (int z = 0; z < depth; z++) {
			if (x < width/2 && z < depth/2) u[x][z] = 15;
			else if (x >= width/2 && z >= depth/2) u[x][z] = 15;
			else u[x][z] = 5;
			//u[x][z] = 0;
			//u[x][z] = 10 + 15 * cosf(8*M_PI*x/width) * sinf(8*M_PI*z/depth);
			
			v[x][z] = 0;
			r[x][z] = 0;
			
			v_tot += u[x][z];

			//3 vertex numbers per one u/v number
			int vertArrayLoc = (x + z * width) * 3;
			HeightFieldVertices[vertArrayLoc] = x;
			HeightFieldVertices[vertArrayLoc + 1] = u[x][z];
			HeightFieldVertices[vertArrayLoc + 2] = z;
		}
	}
	
	v_avg = v_tot / heightFieldWidth / heightFieldDepth;
}

/*
 * Algorithm for height field propagation
 * based on 4 surrounding columns
 * with ghost boundaries for reflection
 */
void hfUpdate() {
	//Update ghost boundaries
	for (int i = 0; i < 2*heightFieldWidth+2*heightFieldDepth; i++) {
		/*
		 *	This is what g[] looks like around u[][]
		 *	assuming x is cols and z is rows
		 *	(I didn't really look to find that out but it shouldn't matter)
		 *	w = width, d = depth
		 *	--------------------------------------------
		 *	             g[0]    g[x]    g[w-1]
		 *	          -----------------------------
		 *	g[2w]     | (0,0)   (x,0)   (w-1,0)   | g[2w+d]
		 *	          |  ...     ...     ...      |
		 *	g[z+2w]	  | (0,z)   (x,z)   (w-1,z)   | g[z+2w+d]
		 *	          |  ...     ...     ...      |
		 *	g[2w+d-1] | (0,d-1) (x,d-1) (w-1,d-1) | g[2w+2d-1]
		 *	          -----------------------------
		 *	             g[w]    g[x+w]  g[2w-1]
		 *	--------------------------------------------
		 */
		
		//broken if corners start with different heights, Based on slope
		if (i < heightFieldWidth) {
			g[i] = 2*u[i][0] - u[i][1];
		} else if (i < 2*heightFieldWidth) {
			g[i] = 2*u[i-heightFieldWidth][heightFieldDepth-1] - u[i-heightFieldWidth][heightFieldDepth-2];
		} else if (i < 2*heightFieldWidth+heightFieldDepth) {
			g[i] = 2*u[0][i-2*heightFieldWidth] - u[1][i-2*heightFieldWidth];
		} else {
			g[i] = 2*u[heightFieldWidth-1][i-2*heightFieldWidth-heightFieldDepth] - u[heightFieldWidth-2][i-2*heightFieldWidth-heightFieldDepth];
		}
	}

	//Update velocity of columns
	float damping = 0.99;
	for (int x = 0; x < heightFieldWidth; x++) {
		for (int z = 0; z < heightFieldDepth; z++) {
			float force = 0;
			if (wrap) {
				//wrapping boundaries
				force += ((z - 1 >= 0 ? u[x][z - 1] : u[x][heightFieldDepth - 1]) +
					(z + 1 < heightFieldDepth ? u[x][z + 1] : u[x][0]) +
					(x - 1 >= 0 ? u[x - 1][z] : u[heightFieldWidth - 1][z]) +
					(x + 1 < heightFieldWidth ? u[x + 1][z] : u[0][z]));
			}
			else if (clamp) {
				//clamping boundaries
				force += ((z - 1 >= 0 ? u[x][z - 1] : u[x][0]) +
					(z + 1 < heightFieldDepth ? u[x][z + 1] : u[x][heightFieldDepth - 1]) +
					(x - 1 >= 0 ? u[x - 1][z] : u[0][z]) +
					(x + 1 < heightFieldWidth ? u[x + 1][z] : u[heightFieldWidth - 1][z]));
			}
			else if (ghost) {
				//reflecting boundaries using g[] as ghost
				force += ((z - 1 >= 0 ? u[x][z - 1] : g[x]) +
					(z + 1 < heightFieldDepth ? u[x][z + 1] : g[x + heightFieldWidth]) +
					(x - 1 >= 0 ? u[x - 1][z] : g[z + 2 * heightFieldWidth]) +
					(x + 1 < heightFieldWidth ? u[x + 1][z] : g[z + 2 * heightFieldWidth + heightFieldDepth]));
			}
			else {
				//reflecting boundaries using average as ghost
				force += ((z - 1 >= 0 ? u[x][z - 1] : v_avg) +
					(z + 1 < heightFieldDepth ? u[x][z + 1] : v_avg) +
					(x - 1 >= 0 ? u[x - 1][z] : v_avg) +
					(x + 1 < heightFieldWidth ? u[x + 1][z] : v_avg));
			}
			force -= 4 * u[x][z];
			force *= c*c/(h*h);
			v[x][z] += force*timeStep;
			v[x][z] *= damping;
		}
	}


	//IMPORTANT:  ONLY WORKS WITH CUBES AND DOESN'T CHECK EDGE CONDITIONS
	//Updates r, object, and u relative to object
	if (drawObject) {
		//Finds the corners of my cube in heightfield indices
		int bottom = floor(cubeLoc.z - cubeSize.z / 2);
		int top = floor(cubeLoc.z + cubeSize.z / 2);
		int left = floor(cubeLoc.x - cubeSize.x / 2);
		int right = floor(cubeLoc.x + cubeSize.x / 2);

		//Amount of water displaced
		float displacement = 0.0;

		//difference in r
		float difference = 0.0;


		//Updates rNew and calculates displacement;
		for (int x = left; x <= right; x++) {
			for (int z = bottom; z <= top; z++) {
				//Calculates current column displacement, clamping between zero and cube height
				float newDisp = v_avg/*u[x][z]*/ - (cubeLoc.y - cubeSize.y / 2);
				newDisp = newDisp >= 0.0 ? newDisp : 0.0;
				newDisp = newDisp <= cubeSize.y ? newDisp : cubeSize.y;

				//Updates displacement, difference and r
				displacement += newDisp;
				difference += newDisp - r[x][z];
				r[x][z] = newDisp;

				//Attempt to fix feedback loop issue
				//if (cubeLoc.y - cubeSize.y / 2 < u[x][z])
					//u[x][z] = cubeLoc.y;

			}
		}

		//Update u with difference, add around shape perimeter
		float added = 4 * difference / (2 * (top - bottom + right - left));
		for (int x = left; x <= right; x++) {
			u[x][bottom - 1] += added;
			u[x][top + 1] += added;
		}
		for (int y = bottom; y <= top; y++) {
			u[left - 1][y] += added;
			u[right + 1][y] += added;
		}

		//Move cube
		cubeVelocity.y += (-gravity + fluidDensity*gravity*displacement / cubeMass)*timeStep;
		cubeVelocity *= 0.99;
		cubeLoc += cubeVelocity*timeStep;
	}
	
	//Update heights
	v_cur = 0;
	for (int x = 0; x < heightFieldWidth; x++) {
		for (int z = 0; z < heightFieldDepth; z++) {
			u[x][z] += v[x][z]*timeStep;
			//Clamping
			if (u[x][z] > 100) u[x][z] = 100;
			if (u[x][z] < -100) u[x][z] = -100;
			
			v_cur += u[x][z];
		}
	}

	//Prevent addition or removal of water from the system
	//Transfer data to GPU array
	float v_dif = v_avg - v_cur / heightFieldWidth / heightFieldDepth;
	for (int x = 0; x < heightFieldWidth; x++) {
		for (int z = 0; z < heightFieldDepth; z++) {
			u[x][z] += v_dif;
			HeightFieldVertices[(x + z * heightFieldWidth) * 3 + 1] = u[x][z];
		}
	}
	
	if (pausing) pause = true;
}

//*************************************************************************************

// Helper Funcs

void convertSphericalToCartesian() {
	eyePoint.x = cameraAngles.z * sinf( cameraAngles.x ) * sinf( cameraAngles.y ) + lookAtPoint.x;
	eyePoint.y = cameraAngles.z * -cosf( cameraAngles.y ) + lookAtPoint.y;
	eyePoint.z = cameraAngles.z * -cosf( cameraAngles.x ) * sinf( cameraAngles.y ) + lookAtPoint.z;
}

//*************************************************************************************

// GLFW Event Callbacks

// print errors from GLFW
static void error_callback(int error, const char* description) {
	fprintf(stderr, "[ERROR]: %s\n", description);
}

// handle key events
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//Quit
	if ((key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	
	//Draw object
	if (key == 'D' && action == GLFW_PRESS)
		drawObject = !drawObject;
	
	//Restart
	if (key == 'R' && action == GLFW_PRESS)
		hfInitialize(heightFieldWidth, heightFieldDepth);
	//Pause
	if (key == 'P' && action == GLFW_PRESS) {
		if (shiftDown) pausing = !pausing;
		else pause = !pause;
	}
	if (key == 'O' && action == GLFW_PRESS) {
		cubeLoc.y = 20;
		cubeVelocity.y = 0;
	}
	//Wrapping
	if (key == 'W' && action == GLFW_PRESS)
		wrap = !wrap;
	//Clamping
	if (key == 'C' && action == GLFW_PRESS)
		clamp = !clamp;
	//Ghost type
	if (key == 'G' && action == GLFW_PRESS)
		ghost = !ghost;
	//Bloop
	if (key == 'B' && action == GLFW_PRESS)
		for (int x = 0; x < heightFieldWidth; x++) {
			for (int z = 0; z < heightFieldDepth; z++) {
				if ((x == heightFieldWidth/4 || 
					x == heightFieldWidth*3/4 || 
					z == heightFieldDepth/4 || 
					z == heightFieldDepth*3/4) && 
					x >= heightFieldWidth/4 && 
					x <= heightFieldWidth*3/4 && 
					z >= heightFieldDepth/4 && 
					z <= heightFieldDepth*3/4)
					u[x][z] += 5;
				HeightFieldVertices[(x + z * heightFieldWidth) * 3 + 1] = u[x][z];
			}
		}
	//Normalize
	if (key == 'N' && action == GLFW_PRESS)
		for (int x = 0; x < heightFieldWidth; x++) {
			for (int z = 0; z < heightFieldDepth; z++) {
				u[x][z] = v_tot / heightFieldWidth / heightFieldDepth;
				v[x][z] = 0;
				HeightFieldVertices[(x + z * heightFieldWidth) * 3 + 1] = u[x][z];
			}
		}
	//Log debug to file
	if (key == 'F' && action == GLFW_PRESS) {
		FILE *f = NULL;
		if (shiftDown) f = fopen("debug.log", "a");
		else f = fopen("debug.log", "w");
		if (f == NULL) {
			fprintf(stderr, "[ERROR]: cannot open file debug.log to write");
			return;
		}
		fprintf(f, " -- g[] -- \n");
		fprintf(f, " - top - \n");
		for (int i = 0; i < heightFieldWidth; i++) {
			if (i == 0) fprintf(f, "%f", g[i]);
			else fprintf(f, ", %f", g[i]);
		}
		fprintf(f, "\n - bottom - \n");
		for (int i = heightFieldWidth; i < 2*heightFieldWidth; i++) {
			if (i == heightFieldWidth) fprintf(f, "%f", g[i]);
			else fprintf(f, ", %f", g[i]);
		}
		fprintf(f, "\n - left - \n");
		for (int i = 2*heightFieldWidth; i < 2*heightFieldWidth+heightFieldDepth; i++) {
			if (i == 2*heightFieldWidth) fprintf(f, "%f", g[i]);
			else fprintf(f, ", %f", g[i]);
		}
		fprintf(f, "\n - right - \n");
		for (int i = 2*heightFieldWidth+heightFieldDepth; i < 2*heightFieldWidth+2*heightFieldDepth; i++) {
			if (i == 2*heightFieldWidth+heightFieldDepth) fprintf(f, "%f", g[i]);
			else fprintf(f, ", %f", g[i]);
		}
		fprintf(f, "\n\n ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n\n");
		fprintf(f, " -- u[][] -- ");
		for (int x = 0; x < heightFieldWidth; x++) {
			fprintf(f, "\n - x = %d\n", x);
			for (int z = 0; z < heightFieldDepth; z++) {
				if (z == 0) fprintf(f, "%f", u[x][z]);
				else fprintf(f, ", %f", u[x][z]);
			}
		}
		fprintf(f, "\n\n ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n\n");
		fprintf(f, " -- v[][] -- ");
		for (int x = 0; x < heightFieldWidth; x++) {
			fprintf(f, "\n - x = %d\n", x);
			for (int z = 0; z < heightFieldDepth; z++) {
				if (z == 0) fprintf(f, "%f", v[x][z]);
				else fprintf(f, ", %f", v[x][z]);
			}
		}
		fprintf(f, "\n\n ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n\n");
		fprintf(f, " -- Camera Position -- \n");
		fprintf(f, "(%f, %f, %f)", cameraAngles.x, cameraAngles.y, cameraAngles.z);
		fprintf(f, "\n\n ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n\n");
		fclose(f);
	}
	else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		shiftDown = true;
	else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		shiftDown = false;
	
	char txt[50] = "Height Fields (";
	if (wrap) strcat(txt, " w ");
	if (clamp) strcat(txt, " c ");
	if (ghost) strcat(txt, " g ");
	if (pausing) strcat(txt, " p ");
	strcat(txt, ")");
	glfwSetWindowTitle(window, txt);
}

// handle mouse clicks
static void mouseClick_callback(GLFWwindow* window, int button, int action, int mods) {
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
		leftMouseDown = true;
	} else {
		leftMouseDown = false;
		mousePosition.x = -9999.0f;
		mousePosition.y = -9999.0f;
	}
}

// handle mouse positions
static void mousePos_callback(GLFWwindow* window, double xpos, double ypos) {
	// make sure movement is in bounds of the window
	// glfw captures mouse movement on entire screen
	if( xpos > 0 && xpos < windowWidth ) {
		if( ypos > 0 && ypos < windowHeight ) {
			// active motion
			if( leftMouseDown ) {
				if( (mousePosition.x - -9999.0f) < 0.001f ) {
					mousePosition.x = xpos;
					mousePosition.y = ypos;
				} else {
					if( !shiftDown ) {
						cameraAngles.x += (xpos - mousePosition.x)*0.005f;
						cameraAngles.y += (ypos - mousePosition.y)*0.005f;

						if( cameraAngles.y < 0 ) cameraAngles.y = 0.0f + 0.001f;
						if( cameraAngles.y >= M_PI ) cameraAngles.y = M_PI - 0.001f;
					} else {
						double totChgSq = (xpos - mousePosition.x) + (ypos - mousePosition.y);
						cameraAngles.z += totChgSq*0.01f;

						if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
						if( cameraAngles.z >= 210.0f ) cameraAngles.z = 210.0f;
					}
					convertSphericalToCartesian();


					mousePosition.x = xpos;
					mousePosition.y = ypos;
				}
			}
			// passive motion
			else {

			}
		}
	}
}

// handle scroll events
static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset ) {
	double totChgSq = yOffset;
	cameraAngles.z += totChgSq*0.1f;

	if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
	if( cameraAngles.z >= 70.0f ) cameraAngles.z = 70.0f;
	
	convertSphericalToCartesian();
}

//*************************************************************************************

// Setup Funcs

// setup GLFW
GLFWwindow* setupGLFW() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	// create a 4.1 Core OpenGL Context
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	GLFWwindow *window = glfwCreateWindow(1280, 720, "Height Fields", NULL, NULL);
	if( !window ) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// register callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseClick_callback);
	glfwSetCursorPosCallback(window, mousePos_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// return our window
	return window;
}

// setup OpenGL parameters
void setupOpenGL() {
	glEnable( GL_DEPTH_TEST );							// turn on depth testing
	glDepthFunc( GL_LESS );								// use less than test
	glFrontFace( GL_CCW );								// front faces are CCW
	glEnable(GL_BLEND);									// turn on alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// blend w/ 1-a
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );				// clear our screen to black

	// initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewResult = glewInit();

	// check for an error
	if( glewResult != GLEW_OK ) {
		printf( "[ERROR]: Error initalizing GLEW\n");
		exit(EXIT_FAILURE);
	}

	// print information about our current OpenGL set up
	CSCI441::OpenGLUtils::printOpenGLInfo();
}

// load our shaders and get locations for uniforms and attributes
void setupShaders() {
	// load our shader program
	blinnPhongShaderProgram        = new CSCI441::ShaderProgram( "shaders/blinnPhong.v.glsl", "shaders/blinnPhong.f.glsl" );

	// query all of our uniform locations
	bpUniformLocs.modelviewMatrix  = blinnPhongShaderProgram->getUniformLocation( "modelviewMatrix" );
	bpUniformLocs.mvpMatrix        = blinnPhongShaderProgram->getUniformLocation( "MVP_Matrix" );
	bpUniformLocs.normalMatrix     = blinnPhongShaderProgram->getUniformLocation( "normalMatrix" );
	bpUniformLocs.lightPosition    = blinnPhongShaderProgram->getUniformLocation( "light.position" );
	bpUniformLocs.La			   = blinnPhongShaderProgram->getUniformLocation( "light.La" );
	bpUniformLocs.Ld			   = blinnPhongShaderProgram->getUniformLocation( "light.Ld" );
	bpUniformLocs.Ls			   = blinnPhongShaderProgram->getUniformLocation( "light.Ls" );
	bpUniformLocs.Ka			   = blinnPhongShaderProgram->getUniformLocation( "material.Ka" );
	bpUniformLocs.Kd			   = blinnPhongShaderProgram->getUniformLocation( "material.Kd" );
	bpUniformLocs.Ks			   = blinnPhongShaderProgram->getUniformLocation( "material.Ks" );
	bpUniformLocs.shininess	 	   = blinnPhongShaderProgram->getUniformLocation( "material.shininess" );

	// query all of our attribute locations
	bpAttribLocs.position          = blinnPhongShaderProgram->getAttributeLocation( "vPos" );
	bpAttribLocs.normal            = blinnPhongShaderProgram->getAttributeLocation( "vNorm" );

	// load our shader program
	pointShaderProgram = new CSCI441::ShaderProgram("shaders/point.v.glsl", "shaders/point.g.glsl", "shaders/point.f.glsl");

	//Uniforms
	pUniformLocs.modelviewMatrix = blinnPhongShaderProgram->getUniformLocation("modelviewMatrix");
	pUniformLocs.mvpMatrix = blinnPhongShaderProgram->getUniformLocation("MVP_Matrix");
	pUniformLocs.normalMatrix = blinnPhongShaderProgram->getUniformLocation("normalMatrix");
	pUniformLocs.lightPosition = blinnPhongShaderProgram->getUniformLocation("light.position");
	pUniformLocs.La = blinnPhongShaderProgram->getUniformLocation("light.La");
	pUniformLocs.Ld = blinnPhongShaderProgram->getUniformLocation("light.Ld");
	pUniformLocs.Ls = blinnPhongShaderProgram->getUniformLocation("light.Ls");
	pUniformLocs.Ka = blinnPhongShaderProgram->getUniformLocation("material.Ka");
	pUniformLocs.Kd = blinnPhongShaderProgram->getUniformLocation("material.Kd");
	pUniformLocs.Ks = blinnPhongShaderProgram->getUniformLocation("material.Ks");
	pUniformLocs.shininess = blinnPhongShaderProgram->getUniformLocation("material.shininess");
	
	//Attributes
	pAttribLocs.position = pointShaderProgram->getAttributeLocation("vPos");

}

// load in our model data to VAOs and VBOs
void setupBuffers() {
	// generate our vertex array object descriptors
	glGenVertexArrays( 1, vaods );

	// will be used to stroe VBO descriptors for ARRAY_BUFFER and ELEMENT_ARRAY_BUFFER
	GLuint vbods[2];

	//------------ BEGIN CUBE VAO ------------

	// bind our Cube VAO
	glBindVertexArray(vaods[CUBE]);

	// generate our vertex buffer object descriptors for the CUBE
	glGenBuffers(2, vbods);
	// bind the VBO for our Cube Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
	// send the data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// bind the VBO for our Cube Element Array Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
	// send the data to the GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// enable our position attribute
	glEnableVertexAttribArray(bpAttribLocs.position);
	// map the position attribute to data within our buffer
	glVertexAttribPointer(bpAttribLocs.position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	// enable our normal attribute
	glEnableVertexAttribArray(bpAttribLocs.normal);
	// map the normal attribute to data within our buffer
	glVertexAttribPointer(bpAttribLocs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));

	//------------  END  CUBE VAO ------------

	//------------ BEGIN GROUND VAO ------------

	// Draw Ground
	glBindVertexArray( vaods[GROUND] );

	// generate our vertex buffer object descriptors for the GROUND
	glGenBuffers( 2, vbods );
	// bind the VBO for our Ground Array Buffer
	glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
	// send the data to the GPU
	glBufferData( GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW );

	// bind the VBO for our Ground Element Array Buffer
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
	// send the data to the GPU
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );

	// enable our position attribute
	glEnableVertexAttribArray( bpAttribLocs.position );
	// map the position attribute to data within our buffer
	glVertexAttribPointer( bpAttribLocs.position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) 0 );

	// enable our normal attribute
	glEnableVertexAttribArray( bpAttribLocs.normal );
	// map the normal attribute to data within our buffer
	glVertexAttribPointer( bpAttribLocs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (3*sizeof(float)) );

	//------------  END  GROUND VAO------------

	//------------ BEGIN HEIGHT FIELD VAO ------------

	// Draw Ground
	glGenVertexArrays(1, &vaoHeightField);
	glBindVertexArray(vaoHeightField);

	// generate our vertex buffer object descriptors
	glGenBuffers(2, vbods);
	// bind the VBO for our Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
	// send the data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*heightFieldDepth*heightFieldWidth*3, HeightFieldVertices, GL_DYNAMIC_DRAW);

	// enable our position attribute
	glEnableVertexAttribArray(pAttribLocs.position);
	// map the position attribute to data within our buffer
	glVertexAttribPointer(bpAttribLocs.position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	//------------  END HEIGHT FIELD VAO------------
}

// handles drawing everything to our buffer
void renderScene(GLFWwindow *window) {
	time = glfwGetTime();

	// query our current window size, determine the aspect ratio, and set our viewport size
	float ratio;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	ratio = windowWidth / (float)windowHeight;
	glViewport(0, 0, windowWidth, windowHeight);

	// create our Model, View, Projection matrices
	glm::mat4 mMtx, vMtx, pMtx;

	// compute our projection matrix
	pMtx = glm::perspective(45.0f, ratio, 0.001f, 1000.0f);
	// compute our view matrix based on our current camera setup
	vMtx = glm::lookAt(eyePoint, lookAtPoint, upVector);

	// precompute the modelview matrix
	glm::mat4 mvMtx = vMtx * mMtx;
	// precompute the modelviewprojection matrix
	glm::mat4 mvpMtx = pMtx * mvMtx;
	// precomute the normal matrix
	glm::mat3 nMtx = glm::transpose(glm::inverse(glm::mat3(mvMtx)));

	// use our Blinn-Phone Shading Program
	blinnPhongShaderProgram->useProgram();

	glDisable(GL_CULL_FACE);

	//Set color
	materialKa = glm::vec3(0.2745, 0.01175, 0.01175);
	materialKd = glm::vec3(0.81424, 0.04136, 0.04136);
	materialKs = glm::vec3(0.927811, 0.626959, 0.626959);
	materialShininess = 0.6 * 128;

	// set all the dynamic uniform values
	glUniformMatrix4fv(bpUniformLocs.modelviewMatrix, 1, GL_FALSE, &mvMtx[0][0]);
	glUniformMatrix4fv(bpUniformLocs.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);
	glUniformMatrix3fv(bpUniformLocs.normalMatrix, 1, GL_FALSE, &nMtx[0][0]);
	glUniform3fv(bpUniformLocs.lightPosition, 1, &lightPosition[0]);
	glUniform3fv(bpUniformLocs.La, 1, &lightLa[0]);
	glUniform3fv(bpUniformLocs.Ld, 1, &lightLd[0]);
	glUniform3fv(bpUniformLocs.Ls, 1, &lightLs[0]);
	glUniform3fv(bpUniformLocs.Ka, 1, &materialKa[0]);
	glUniform3fv(bpUniformLocs.Ks, 1, &materialKd[0]);
	glUniform3fv(bpUniformLocs.Kd, 1, &materialKs[0]);
	glUniform1f(bpUniformLocs.shininess, materialShininess);

	// bind our Ground VAO
	glBindVertexArray(vaods[GROUND]);
	// draw our ground!
	//glDrawElements(GL_TRIANGLES, sizeof(groundIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, (void*)0);

	glm::mat4 model = mMtx;

	mMtx = glm::translate(mMtx, cubeLoc);
	mMtx = glm::scale(mMtx, cubeSize);

	// update our modelview matrix
	mvMtx = vMtx * mMtx;
	// update our modelviewprojection matrix
	mvpMtx = pMtx * mvMtx;
	// update our normal matrix
	nMtx = glm::transpose(glm::inverse(glm::mat3(mvMtx)));

	// update our dynamic uniform values
	glUniformMatrix4fv(bpUniformLocs.modelviewMatrix, 1, GL_FALSE, &mvMtx[0][0]);
	glUniformMatrix4fv(bpUniformLocs.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);
	glUniformMatrix3fv(bpUniformLocs.normalMatrix, 1, GL_FALSE, &nMtx[0][0]);

	// bind our Cube VAO
	glBindVertexArray(vaods[CUBE]);
	// draw our cube!
	if (drawObject)
		glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, (void*)0);

	mMtx = model;

	//Draw Height Field
	glBindVertexArray(vaoHeightField);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	materialKa = glm::vec3(0.0215, 0.0215, 0.3745);
	materialKd = glm::vec3(0.07568, 0.07568, 0.81424);
	materialKs = glm::vec3(0.333, 0.333, 0.467811);
	materialShininess = 0.6 * 128;

	//Select Shader
	pointShaderProgram->useProgram();

	//mMtx = glm::translate(mMtx, glm::vec3(-10, -10, -10));
	// update our modelview matrix
	mvMtx = vMtx * mMtx;
	// update our modelviewprojection matrix
	mvpMtx = pMtx * mvMtx;
	// update our normal matrix
	nMtx = glm::transpose(glm::inverse(glm::mat3(mvMtx)));

	//Pass uniforms
	glUniformMatrix4fv(pUniformLocs.modelviewMatrix, 1, GL_FALSE, &mvMtx[0][0]);
	glUniformMatrix4fv(pUniformLocs.mvpMatrix, 1, GL_FALSE, &mvpMtx[0][0]);
	glUniformMatrix3fv(pUniformLocs.normalMatrix, 1, GL_FALSE, &nMtx[0][0]);
	glUniform3fv(pUniformLocs.lightPosition, 1, &lightPosition[0]);
	glUniform3fv(pUniformLocs.La, 1, &lightLa[0]);
	glUniform3fv(pUniformLocs.Ld, 1, &lightLd[0]);
	glUniform3fv(pUniformLocs.Ls, 1, &lightLs[0]);
	glUniform3fv(pUniformLocs.Ka, 1, &materialKa[0]);
	glUniform3fv(pUniformLocs.Ks, 1, &materialKd[0]);
	glUniform3fv(pUniformLocs.Kd, 1, &materialKs[0]);
	glUniform1f(pUniformLocs.shininess, materialShininess);

	//Draw
	glPointSize(4.0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*heightFieldDepth*heightFieldWidth * 3, HeightFieldVertices, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, heightFieldWidth*heightFieldDepth);
	
	//Update
	if (!pause)
		hfUpdate();
}

// program entry point
int main( int argc, char *argv[] ) {
	GLFWwindow *window = setupGLFW();	// setup GLFW and get our window
	hfInitialize(200, 200);
	setupOpenGL();						// setup OpenGL & GLEW
	setupShaders();						// load our shader programs, uniforms, and attribtues
	setupBuffers();						// load our models into GPU memory
	
	convertSphericalToCartesian();		// position our camera in a pretty place
	
	char txt[50] = "Height Fields (";
	if (wrap) strcat(txt, " w ");
	if (clamp) strcat(txt, " c ");
	if (ghost) strcat(txt, " g ");
	if (pausing) strcat(txt, " p ");
	strcat(txt, ")");
	glfwSetWindowTitle(window, txt);
	
	// as long as our window is open
	while( !glfwWindowShouldClose(window) ) {
		// clear the prior contents of our buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// render our scene
		renderScene( window );

		// swap the front and back buffers
		glfwSwapBuffers(window);
		// check for any events
		glfwPollEvents();
	}
	
	// destroy our window
	glfwDestroyWindow(window);
	// end GLFW
	glfwTerminate();

	// delete our shader program
	delete blinnPhongShaderProgram;
	delete pointShaderProgram;

	// SUCCESS!!
	return EXIT_SUCCESS;
}
