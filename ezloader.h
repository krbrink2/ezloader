#ifndef EZLOADER_H_
#define EZLOADER_H_
#include <GL/glut.h>
#include <stdio.h>
#define MAX_TOKEN_SIZE 128

typedef struct element_t{
	char type;
	short numVertices;
	int vertexIndices[3];
	int textureVertexIndices[3];
	int vertexNormalIdices[3];
} element_t;

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
int parse(FILE *fp);
int drawGroupCallList(group_t * group, GLint callListIndex);

#endif // EZLOADER_H_