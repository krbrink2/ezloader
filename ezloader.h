#ifndef EZLOADER_H_
#define EZLOADER_H_
#include <GL/glut.h>

typedef struct element_t{
	char type;
	int vertexIndices[3];
} element_t;

typedef struct group_t{
	char name[64];
	char matName[64];
	int numVertices;
	// Pointer to array of arrays of 3 doubles.
	double * vertices;
	double * textureVertices; // ignore for now.
	double * normals;
	int numElements;
	element_t * elements;
} group_t;

// global declarations

// function declarations
int ezloadCallList(GLint callListIndex, FILE *fp);
int parse(FILE *fp);
int drawGroupCallList(group_t * group, GLint callListIndex);

#endif // EZLOADER_H_