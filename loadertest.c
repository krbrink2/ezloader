/*
	Loads in a .obj file, using ezloader.
*/
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <SOIL/SOIL.h>
#include "ezloader.h"

// Globals
GLint callListIndex;
GLfloat angle;
GLfloat scaler = .01;
GLfloat ytrans = -0;
static GLuint texNames[2];

void init(){
	int maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	//printf("%i, %i\n", maj, min);

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	//char fname[] = "Avent.obj";
	//char fname[] = "pitcher.obj";
	//char fname[] = "wateringcan.obj";
	//char fname[] = "teapot_0.obj";
	//char fname[] = "toyplane.obj";
	//char fname[] = "ducky.obj";
	//char fname[] = "pan_obj.obj";
	//char fname[] = "dlamp.obj";
	//char fname[] = "dtable.obj";
	char fname[] = "teapot.obj";
	//char fname[] = "sword.obj";
	//char fname[] = "shitplane.obj";

	glClearColor(.2, .2, .2, 0);
	glEnable(GL_DEPTH_TEST);

	// Load texture
	glGenTextures(2, texNames);
	//texNames[0] = SOIL_load_OGL_texture("metal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// load object to display
	FILE *fp;
	callListIndex=glGenLists(1);
	fp=fopen(fname,"r");
	if (!fp) 
	    {
	        printf("can't open file %s\n", fname);
		  exit(1);
	    }
	// LOAD HERE
	ezloadCallList(callListIndex, fp);
	//ezload(fp);
	fclose(fp);

	angle = 0;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//gluLookAt(3*cos(angle/20), -cos(angle/10), 2, 0, 0, -1, 0, 1, 0);
	//gluLookAt(6, 1, 0, 0, 0, 0, 0, 1, 0);
	//gluLookAt(1.4, 4, 0, 1.3, 1.5, 0, 0, 1, 0);
	gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//GLfloat matamb[] = {.83, .36, .1, 1.0};
	//GLfloat matdiff[] = {.83, .36, .1, 1.0};
	GLfloat matamb[] = {.9, .9, .9, 1.0};
	GLfloat matdiff[] = {.4, .4, .4, 1.0};
	GLfloat matspec[] = {1, 1, 1, 1.0};
	//GLfloat matspec[] = {.01, .01, .01, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, matamb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matdiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matspec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);
	GLfloat amb[] = {0.2,0.2,0.2};
	GLfloat diff[] = {1.0,1.0,1.0};
	GLfloat spec[] = {1,1,1};
	GLfloat lpos[] = {10*sin(angle/30), 10*cos(angle/30), 10};
	//GLfloat lpos[] = {0, 100, 0};
	//GLfloat lpos[] = {4*cos(angle/10), 4*sin(angle/10), 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texNames[0]);
	// The below will autogenerate texture coordinates
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glPushMatrix();
		glTranslatef(0, ytrans, 0);
		//glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, -1, 0);

		//glTranslatef(0, ytrans, 0);
		glScalef(scaler, scaler, scaler);
		//glRotatef(angle, 0, 1, 0);
		glCallList(callListIndex);
	glPopMatrix();


	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();

	angle += .5;
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// field of view, aspect ratio, clipping near/far
	gluPerspective(60, (float)w/h, 0.01, 10);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 'a':
			break;
		case 27:
			exit(0);
			break;
	}
}

void mouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		printf("push left mouse button.\n");
}

int main(int argc, char* argv[]){
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 450);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}