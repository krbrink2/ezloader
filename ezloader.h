#ifndef EZLOADER_H_
#define EZLOADER_H_
#include <GL/glut.h>
#include <stdio.h>
#define MAX_TOKEN_SIZE 128

// I'm sure I could move these typedefs
typedef struct element_t{
	char type;
	short numPoints;
	int vertexIndices[4];
	int textureVertexIndices[4];
	int vertexNormalIndices[4];
} element_t;

// @TODO remove this
typedef struct group_t{
	char name[MAX_TOKEN_SIZE + 1];
	char matName[MAX_TOKEN_SIZE + 1];
	int numVertices;
	int arraySize;	// number of GLfloats in the below
	GLfloat * vertices;
	GLfloat * textureVertices; // ignore for now.
	GLfloat * vertexNormals;
	int numElements;
	int elementsArraySize;
	element_t * elements;
} group_t;

// global declarations

// function declarations
int ezload(FILE *fp);
int ezloadCallList(GLint callListIndex, FILE *fp);

#endif // EZLOADER_H_