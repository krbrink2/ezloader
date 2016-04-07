/*
	Loads in a .obj file, using ezloader.
*/
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "ezloader.h"

// Globals
GLint callListIndex;
GLfloat angle;

void init(){
	int maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	//printf("%i, %i\n", maj, min);

	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);

	//char fname[] = "toyplane.obj";
	//char fname[] = "Avent.obj";
	char fname[] = "teapot_0.obj";

	glClearColor(.2, .2, .2, 0);
	glEnable(GL_DEPTH_TEST);

	// load object to display
	FILE *fp;
	int read;
	GLfloat x, y, z;
	char ch;
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
	gluLookAt(6*cos(angle/20), 3-cos(angle/10), 2, 0, 0, 0, 0, 1, 0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat matamb[] = {.83, .36, .1, 1.0};
	GLfloat matdiff[] = {.83, .36, .1, 1.0};
	GLfloat matspec[] = {1, 1, 1, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, matamb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matdiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matspec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);
	GLfloat amb[] = {0.2,0.2,0.2};
	GLfloat diff[] = {1.0,1.0,1.0};
	GLfloat spec[] = {0,1,1};
	GLfloat lpos[] = {0, 3, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	glPushMatrix();
	//glTranslatef(3, -1, 0);
	//glRotatef(angle, 0, 1, 0);
	//glScalef(.01, .01, .01);
	glTranslatef(0, 0, 0);
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
	gluPerspective(70, (float)w/h, 0.01, 10);
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