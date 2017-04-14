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

// specify our Ground Vertex Information
const Vertex groundVertices[] = {
		{ -15.0f, -5.0f, -15.0f, 0.0f, 1.0f, 0.0f }, // 0 - BL
		{  15.0f, -5.0f, -15.0f, 0.0f, 1.0f, 0.0f }, // 1 - BR
		{  15.0f, -5.0f,  15.0f, 0.0f, 1.0f, 0.0f }, // 2 - TR
		{ -15.0f, -5.0f,  15.0f, 0.0f, 1.0f, 0.0f }  // 3 - TL
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
glm::vec2 mousePosition( -9999.0f, -9999.0f );

glm::vec3 cameraAngles( 1.82f, 2.01f, 15.0f );
glm::vec3 eyePoint(   10.0f, 10.0f, 10.0f );
glm::vec3 lookAtPoint( 0.0f,  0.0f,  0.0f );
glm::vec3 upVector(    0.0f,  1.0f,  0.0f );

glm::vec3 materialKa( 0.0215, 0.0215, 0.3745 );
glm::vec3 materialKd( 0.07568, 0.07568, 0.81424 );
glm::vec3 materialKs( 0.333, 0.333, 0.467811 );
float materialShininess = 0.6*128;

glm::vec3 lightPosition( 3, 5, 3 );
glm::vec3 lightLd( 1, 1, 1 );
glm::vec3 lightLa( 1, 1, 1 );
glm::vec3 lightLs( 1, 1, 1 );

CSCI441::ShaderProgram *blinnPhongShaderProgram = NULL;
CSCI441::ShaderProgram *pointShaderProgram = NULL;

const GLuint CUBE = 0, GROUND = 1;
GLuint vaods[2];

//*************************************************************************************

//**************************************** Height Field ********************************
GLuint vaoHeightField;
float *HeightFieldVertices;
float **u, **v;
int heightFieldWidth, heightFieldDepth;
bool pause = true;

void hfInitialize(int width, int depth) {
	heightFieldWidth = width;
	heightFieldDepth = depth;

	//Declare arrays
	u = new float*[width];
	v = new float*[width];

	//VBO for vertex position
	//{x,y,z,x,y,z,.....
	//y = u
	HeightFieldVertices = new float[width*depth * 3];

	//Initialize arrays
	for (int x = 0; x < width; x++) {
		u[x] = new float[depth];
		v[x] = new float[depth];
		for (int z = 0; z < depth; z++) {
			//u[x][z] = something cool;
			u[x][z] = 5 + 1.0*(x+z)/5;
			v[x][z] = 0;

			//3 vertex numbers per one u/v number
			int vertArrayLoc = (x + z * width) * 3;
			HeightFieldVertices[vertArrayLoc] = x;
			HeightFieldVertices[vertArrayLoc + 1] = u[x][z];
			HeightFieldVertices[vertArrayLoc + 2] = z;
		}
	}
}

//Algorithm from hello world example in slides
void hfUpdate() {
	for (int x = 0; x < heightFieldWidth; x++) {
		for (int z = 0; z < heightFieldDepth; z++) {
			v[x][z] += (u[x-1 >= 0 ? x-1 : 0][z] + u[x][z-1 >= 0 ? z-1 : 0] +
						u[(x+1 < heightFieldWidth) ? x + 1 : (heightFieldWidth-1)][z] + u[x][(z+1 < heightFieldDepth) ? z+1 : (heightFieldDepth-1)])/4 - u[x][z];
			v[x][z] *= 0.67;
			u[x][z] += v[x][z];
			HeightFieldVertices[(x + z * heightFieldWidth) * 3 + 1] = u[x][z];
		}
	}
}

//*************************************************************************************

// Helper Funcs

void convertSphericalToCartesian() {
	eyePoint.x = cameraAngles.z * sinf( cameraAngles.x ) * sinf( cameraAngles.y );
	eyePoint.y = cameraAngles.z * -cosf( cameraAngles.y );
	eyePoint.z = cameraAngles.z * -cosf( cameraAngles.x ) * sinf( cameraAngles.y );
}

//*************************************************************************************

// GLFW Event Callbacks

// print errors from GLFW
static void error_callback(int error, const char* description) {
	fprintf(stderr, "[ERROR]: %s\n", description);
}

// handle key events
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if ((key == GLFW_KEY_ESCAPE || key == 'R') && action == GLFW_PRESS)
		hfInitialize(20, 20);
	if ((key == GLFW_KEY_ESCAPE || key == 'P') && action == GLFW_PRESS)
		pause = !pause;
	else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		shiftDown = true;
	else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		shiftDown = false;
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
						if( cameraAngles.z >= 50.0f ) cameraAngles.z = 50.0f;
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
	cameraAngles.z += totChgSq*0.01f;

	if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
	if( cameraAngles.z >= 50.0f ) cameraAngles.z = 50.0f;
	
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

	GLFWwindow *window = glfwCreateWindow(640, 480, "A1 Solution", NULL, NULL);
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
	float time = glfwGetTime();

	// query our current window size, determine the aspect ratio, and set our viewport size
	float ratio;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	ratio = windowWidth / (float)windowHeight;
	glViewport(0, 0, windowWidth, windowHeight);

	// create our Model, View, Projection matrices
	glm::mat4 mMtx, vMtx, pMtx;

	// compute our projection matrix
	pMtx = glm::perspective(45.0f, ratio, 0.001f, 100.0f);
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


	//Draw Height Field
	glBindVertexArray(vaoHeightField);

	//Select Shader
	pointShaderProgram->useProgram();

	mMtx = glm::translate(mMtx, glm::vec3(-10, -10, -10));
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
	hfInitialize(20, 20);
	setupOpenGL();						// setup OpenGL & GLEW
	setupShaders();						// load our shader programs, uniforms, and attribtues
	setupBuffers();						// load our models into GPU memory

	convertSphericalToCartesian();		// position our camera in a pretty place
	
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
