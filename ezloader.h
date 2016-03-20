#ifndef EZLOADER_H_
#define EZLOADER_H_
#include <GL/glut.h>
#include <stdio.h>
#define MAX_TOKEN_SIZE 64

typedef struct element_t{
	char type;
	int vertexIndices[3];
} element_t;

typedef struct group_t{
	char name[MAX_TOKEN_SIZE + 1];
	char matName[MAX_TOKEN_SIZE + 1];
	int numVertices;
	int arraySize;	// number of GLfloats in the below
	// Pointer to array of arrays of 3 doubles.
	GLfloat * vertices;
	GLfloat * textureVertices; // ignore for now.
	GLfloat * vertexNormals;
	int numElements;
	element_t * elements;
} group_t;

// global declarations
// @TODO Is this how I should be handling globals?
char mtllibName[MAX_TOKEN_SIZE + 1];

// function declarations
int ezloadCallList(GLint callListIndex, FILE *fp);
int parse(FILE *fp);
int drawGroupCallList(group_t * group, GLint callListIndex);

#endif // EZLOADER_H_