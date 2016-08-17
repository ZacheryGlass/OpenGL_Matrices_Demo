// File name:		OpenGL_Matrices_Demo.cpp
// Author:			Zachery Glass
// Date:			May 15, 2016
// Description:		This program draw multiple shapes and demonstrates the uses of a rotation, translation, sheer and scale matrices. 
//					The user must press the UP and DOWN arrow keys to change movement speed of the shapes.

#include <iostream>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../../include/Angel.h"
using namespace std;
using namespace std::chrono;

// ---------------------------------------------------------------------------------
GLuint  initShaders(); // function prototype

// ------------------------------- Global Variables -----------------------------------------------
GLuint positionVBO = 0;
GLuint colorVBO = 0;
GLfloat deltaX = 0;
GLfloat deltaY = 0;
unsigned long int  elapsedMsCountLoc = 0;
unsigned long int  elapsedMsCount = 0;
unsigned long int  previousMsCount = 0;
unsigned long int  beginMsCount = 0;
GLfloat delta = 0;
GLfloat  animationTime = 0;


mat4	rotationMatrix;
GLint	rotationMatrixParamLoc;
mat4	translationMatrix;
GLint	translationMatrixParamLoc;
mat4	shearMatrix = mat4(1.0);
GLint	shearMatrixParamLoc;
mat4	scaleMatrix = mat4(1.0);
GLint	scaleMatrixParamLoc;
GLfloat	timeMultiplier = 0.0;


GLfloat  position[60]
= { -0.9, +0.0, +0.9, +0.0,
	0.0, -0.9, +0.0, +0.9,
	// Triangle
	-0.298,  0.598,
	-0.596, -0.106,
	 0,		-0.106,
	// Pentagon
	 0.095, 0.114,
	 0.300, 0.397,
	 0.097, 0.682,
	-0.240, 0.570,
	-0.240, 0.227,
	// Square
	 0.205, -0.901,
	 0.205, -0.300,
	-0.397, -0.300,
	-0.397, -0.901, 
	// house outer
	0.501, 0.098, 
	0.501, 0.344, 
	0.651, 0.496,
	0.801, 0.344,
	0.801, 0.098, 
	// house chimney
	0.553, 0.396, 
	0.553, 0.496, 
	0.599, 0.496, 
	0.599, 0.445, 
	// door
	0.651, 0.098, 
	0.651, 0.249, 
	0.731, 0.249, 
	0.731, 0.098
};

GLfloat  color[90]
= { 1.0, 1.0, 1.0,  1.0, 1.0, 1.0,
	1.0, 1.0, 1.0,  1.0, 1.0, 1.0,
	// Triangle
	1.0, 0.8, 0.8,  1.0, 0.8, 0.8,
	1.0, 0.8, 0.8,
	// Pentagon
	0.5, 0.8, 1.0,  0.5, 0.8, 1.0,
	0.5, 0.8, 1.0,  0.5, 0.8, 1.0,
	0.5, 0.8, 1.0,
	// Square
	0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
	// house
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00,  0.93, 0.48, 0.00,
	0.93, 0.48, 0.00

};
// ------------------------------- Global Variables -----------------------------------------------

void keyboardCallbackFunction(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		timeMultiplier += .1 ;
		break;
	case GLUT_KEY_DOWN:
		if (timeMultiplier > 0 )
			timeMultiplier -= .1 ;
		if (timeMultiplier < 0)
			timeMultiplier = 0;
		break;
	}
	// Functions to trigger screen redisplay
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glutPostRedisplay();
}// end keyboardCallbackFunction

 // ---------------------------------------------------------------------------------

void idleCallbackFunction()
{
	milliseconds nowInMs = duration_cast<milliseconds>
		(high_resolution_clock::now().time_since_epoch());
	unsigned long int currMsCount = nowInMs.count();
	if (beginMsCount == 0) // then first call to this function
		beginMsCount = currMsCount;
	elapsedMsCount = currMsCount - beginMsCount;

	//-----------------------------------------------------------------------

	// calculate theta paramater 
	delta = elapsedMsCount - previousMsCount;
	animationTime += (timeMultiplier * ( delta ) );

	GLfloat t = atan(1) / 500 * animationTime; // t = time in seconds (roughly)
	

	GLfloat theta = atan(1) * 2 * sin(t);
	theta *= 57.2958; // convert from Radians to Degrees
	rotationMatrix = RotateZ(theta); // for Rotation
	glUniformMatrix4fv(rotationMatrixParamLoc, 1, GL_FALSE, rotationMatrix);// pass rotationMatrix to shader

	// calulate paramaters
	GLfloat deltaX = 0.5 * sin(t); // fluctuates form -0.5 to 0.5
	GLfloat deltaY = -0.25 * sin(t); // fluctuates from 0.2 to 0.7
	translationMatrix = Translate(deltaX, deltaY, 0); // for Translation
	glUniformMatrix4fv(translationMatrixParamLoc, 1, GL_FALSE, translationMatrix);// pass translationMatrix to shader

	// calculate angle with respect to time & multiplier
	GLfloat alpha = (2 * atan(1)) + (atan(1) * 4.0 / 6.0) * sin(t); // fluctuates correctly
	shearMatrix[0][1] = 1 / tan(alpha);
	glUniformMatrix4fv(shearMatrixParamLoc, 1, GL_FALSE, shearMatrix); // pass shearMatrix to shader

	scaleMatrix[1][1] = sin(animationTime / 636.5);
	glUniformMatrix4fv(scaleMatrixParamLoc, 1, GL_FALSE, scaleMatrix); // pass scaleMatrix to shader
	//------------------------------------------------------------------------//

	previousMsCount = elapsedMsCount;
	glutPostRedisplay();// trigger screen redisplay
} // end idleCallbackFunction

  // ---------------------------------------------------------------------------------

void displayCallbackFunction()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_LINES, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 5);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_LINE_LOOP, 16, 5);
	glDrawArrays(GL_LINE_LOOP, 21, 4);
	glDrawArrays(GL_LINE_LOOP, 25, 4);
	glutSwapBuffers();
	glFlush();
} // end displayCallbackFunction

  // ---------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
	// -------------------------------------------------------------------------------
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(620, 20);
	glutCreateWindow("Glass Homework 01");
	glewInit();
	// ------------------------------------------------------------------------------
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	// ------------------------------------------------------------------------------
	GLuint myVAO = 0;
	glGenVertexArrays(1, &myVAO);
	glBindVertexArray(myVAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// ------------------------------------------------------------------------------
	GLuint program = initShaders();
	glUseProgram(program);
	// ---------------------------------------------------------------------------

	elapsedMsCountLoc = glGetUniformLocation(program, "elapsedMsCount");
	glUniform1ui(elapsedMsCountLoc, elapsedMsCount);

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "\n\tRenderer ....................... " << renderer << endl;
	cout << "\n\tOpenGL version supported ....... " << version << endl;

	// -------------- Creating the Transformation Matrix ------------------------------	
	rotationMatrixParamLoc = glGetUniformLocation(program, "rotationMatrix");
	translationMatrixParamLoc = glGetUniformLocation(program, "translationMatrix");
	scaleMatrixParamLoc = glGetUniformLocation(program, "scaleMatrix");
	shearMatrixParamLoc = glGetUniformLocation(program, "shearMatrix");

	//--------------------------------------------------------------------------------------------------------------
	glClearColor(0.2, 0.2, 0.2, 1.0);
	// ---
	glutDisplayFunc(displayCallbackFunction);
	glutSpecialFunc(keyboardCallbackFunction);
	glutIdleFunc(idleCallbackFunction);
	// --------------------------
	glutMainLoop();
} // end main

  // ---------------------------------------------------------------------------------

GLuint initShaders()
{
	string vertexShaderAsString
		= "#version 420\n"
		"layout(location = 0) in vec2 vertexPosition ;\n"
		"layout(location = 1) in vec3 vertexColor ;\n"
		"out vec3 color ;\n"
		"uniform mat4 translationMatrix ; \n"
		"uniform mat4 scaleMatrix ; \n"
		"uniform mat4 rotationMatrix ;\n"
		"uniform mat4 shearMatrix ;\n"
		"void main ()\n"
		"  {\n"
		"if ( gl_VertexID < 4 ) \n"
		"  gl_Position =  vec4(vertexPosition,0,1); \n"
		"else if ( gl_VertexID < 7 ) \n"
		"  gl_Position =  vec4(vertexPosition,0,1) * rotationMatrix; \n"
		"else if ( gl_VertexID < 12 )\n"
		"  gl_Position =  vec4(vertexPosition,0,1) * translationMatrix ; \n"
		"else if (gl_VertexID < 16)\n"
		"  gl_Position =  vec4(vertexPosition,0,1) * shearMatrix ; \n"
		"else\n"
		"  gl_Position =  vec4(vertexPosition,0,1) * scaleMatrix ; \n"
		"color = vertexColor;\n"
		"  }\n";

	string fragmentShaderAsString
		= "#version 420\n"
		"in vec3 color;\n"
		"void main ()\n"
		" { gl_FragColor = vec4(color,1.0); }\n";
	GLuint program = glCreateProgram();

	// ---------------------------------------------- prepare vertex shader ----------------------------------------------

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char * vertex_cstr = new char[vertexShaderAsString.length() + 1];
	strcpy_s(vertex_cstr, vertexShaderAsString.length() + 1, vertexShaderAsString.c_str());
	glShaderSource(vertexShader, 1, &vertex_cstr, NULL);
	delete[] vertex_cstr;
	glCompileShader(vertexShader);
	GLint  compiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cerr << "vertexShader failed to compile:" << std::endl;
		GLint  logSize;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
		char * logMsg = new char[logSize];
		glGetShaderInfoLog(vertexShader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
		exit(EXIT_FAILURE);
	}
	glAttachShader(program, vertexShader);
	// ----------------------- prepare fragment shader -----------------------
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char * fragment_cstr = new char[fragmentShaderAsString.length() + 1];
	strcpy_s(fragment_cstr, fragmentShaderAsString.length() + 1, fragmentShaderAsString.c_str());
	glShaderSource(fragmentShader, 1, &fragment_cstr, NULL);
	delete[] fragment_cstr;
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cerr << "fragmentShader failed to compile:" << std::endl;
		GLint  logSize;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logSize);
		char * logMsg = new char[logSize];
		glGetShaderInfoLog(fragmentShader, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
		exit(EXIT_FAILURE);
	}
	glAttachShader(program, fragmentShader);
	// ------------------------- link shader program -------------------------
	glLinkProgram(program);
	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char * logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;
		exit(EXIT_FAILURE);
	}
	return program;
} // end initShaders function

  // ---------------------------------------------------------------------------------