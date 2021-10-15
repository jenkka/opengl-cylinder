// JUAN CARLOS GONZALEZ GUERRA
// W = FORWARD
// S = BACK
// A = LEFT
// D = RIGHT
// E = UP
// Q = DOWN
// ARROWS = ROTATE

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Cylinder.h"
#include "Utils.h"
#include "Mat4.h"
#include "Transforms.h"
#include <stdio.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define RESET 0xFFFF

typedef enum { IDLE, LEFT, RIGHT, UP, DOWN, FRONT, BACK } MOTION_TYPE;
typedef float vec3[3];

static GLuint vertexPositionLoc,  vertexNormalLoc, projectionMatrixLoc,  viewMatrixLoc;
static GLuint vertexColorLoc, vertexPositionLoc, vertexNormalLoc;
static GLuint cameraPositionLoc;
static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc, materialALoc, materialDLoc, materialSLoc, exponentLoc;
static Mat4   projectionMatrix, viewMatrix;
static GLuint programId, modelMatrixLoc;
static Mat4   modelMatrix;

static MOTION_TYPE motionType 	= 0;

static float cameraSpeed     	= 0.4;
static float cameraX        	= 0;
static float cameraZ         	= 5;
static float cameraY		 	= 0;
static float cameraAngle     	= 0.0;
static float xAngle = 0;
static float yAngle = 0;

static float ambientLight[]  		= {1.0, 1.0, 1.0};
static float materialA[]     		= {0.3, 0.3, 0.3};

static float diffuseLight[]  	= {1.0, 1.0, 1.0};
static float lightPosition[] 	= {-5.0, 0.0, -1.0};
static float materialD[]     	= {0.6, 0.6, 0.6};
static float materialS[]		= {0.4, 0.4, 0.4};
static float exponent			= 16;

float length = 6, bottomRadius = 0.5, topRadius = 1.5;
int slices = 30, stacks = 30;
vec3 topColor 		= { 1.0, 0.8, 0.0 };
vec3 bottomColor 	= { 1.0, 0.8, 0.0 };

Cylinder cylinder;

static void initShaders()
{
	//CREATING CYLINDER
	cylinder = cylinderCreate(length, bottomRadius, topRadius, slices, stacks, bottomColor, topColor);

	GLuint vShader = compileShader("shaders/gouraud.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	GLuint fShader = compileShader("shaders/color.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;

	programId = glCreateProgram();
	glAttachShader(programId, vShader);
	glAttachShader(programId, fShader);
	glLinkProgram(programId);

	vertexPositionLoc   = glGetAttribLocation(programId, "vertexPosition");
	vertexNormalLoc     = glGetAttribLocation(programId, "vertexNormal");
	vertexColorLoc    	= glGetAttribLocation(programId, "vertexColor");
	modelMatrixLoc      = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLoc       = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId, "projMatrix");

	ambientLightLoc     = glGetUniformLocation(programId, "ambientLight");
	diffuseLightLoc     = glGetUniformLocation(programId, "diffuseLight");
	lightPositionLoc    = glGetUniformLocation(programId, "lightPosition");
	materialALoc        = glGetUniformLocation(programId, "materialA");
	materialDLoc        = glGetUniformLocation(programId, "materialD");
	materialSLoc        = glGetUniformLocation(programId, "materialS");
	exponentLoc 		= glGetUniformLocation(programId, "exponent");
	cameraPositionLoc   = glGetUniformLocation(programId, "camera");

	//BINDING CYLINDER
	cylinderBind(cylinder, vertexPositionLoc, vertexColorLoc, vertexNormalLoc);
}

static void initLights()
{
	glUseProgram(programId);
	glUniform3fv(ambientLightLoc,  1, ambientLight);
	glUniform3fv(diffuseLightLoc,  1, diffuseLight);
	glUniform3fv(lightPositionLoc, 1, lightPosition);
	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);
	glUniform1f(exponentLoc, exponent);
}

static void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(RESET);

	switch(motionType) {
  		case  LEFT  : 	 cameraX -= cameraSpeed; break;
  		case  RIGHT :  	cameraX += cameraSpeed; break;
		case  FRONT :  	cameraZ -= cameraSpeed; break;
		case  BACK  :  	cameraZ += cameraSpeed; break;
		case  UP    :	cameraY += cameraSpeed; break;
		case  DOWN  :	cameraY -= cameraSpeed; break;
		case  IDLE  :  ;
	}

//	Envío de proyección, vista y posición de la cámara al programa 1 (cuarto, rombo)
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
	mIdentity(&viewMatrix);
	rotateY(&viewMatrix, -cameraAngle);
	translate(&viewMatrix, -cameraX, -cameraY, -cameraZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);
	glUniform3f(cameraPositionLoc, cameraX, cameraY, cameraZ);

	//Create cylinder
	rotateX(&modelMatrix, xAngle);
	rotateY(&modelMatrix, yAngle);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	cylinderDraw(cylinder);
	mIdentity(&modelMatrix);

	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
}

static void timerFunc(int id)
{
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
}

static void specialKeyReleasedFunc(int key,int x, int y)
{
	motionType = IDLE;
}

static void keyReleasedFunc(unsigned char key,int x, int y)
{
	motionType = IDLE;
}

static void specialKeyPressedFunc(int key, int x, int y)
{
	switch(key) {
		case 100:	yAngle	   	-=5; break;
		case 102:	yAngle	   	+=5; break;
		case 101: 	xAngle	   	+=5; break;
		case 103: 	xAngle	   	-=5; break;
	}
}

static void keyPressedFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'a':
		case 'A': motionType = LEFT; break;
		case 'd':
		case 'D': motionType = RIGHT; break;
		case 'w':
		case 'W': motionType = FRONT; break;
		case 's':
		case 'S': motionType = BACK; break;
		case 'e':
		case 'E': motionType = UP; break;
		case 'q':
		case 'Q': motionType = DOWN; break;

		case 27 : exit(0);
	}
 }


int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cylinder");
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);
    glutKeyboardUpFunc(keyReleasedFunc);
    glutSpecialFunc(specialKeyPressedFunc);
    glutSpecialUpFunc(specialKeyReleasedFunc);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    initShaders();
    initLights();

    glClearColor(0.1, 0.1, 0.3, 1.0);
    glutMainLoop();

	return 0;
}
