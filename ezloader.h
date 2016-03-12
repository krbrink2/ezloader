#ifndef EZLOADER_H_
#define EZLOADER_H_

typedef struct group_t{
	char[64] name;
	char[64] matName;
	int numVertices;
	double[3] * vertices;
	double[3] * textureVertices; // ignore for now.
	double[3] * normals;
	int numElements;
	element_t * elements;
} grout_t;

typedef struct element_t{
	char type;
	int[3] vertexIndices;
} element_t;

// global declarations

// function declarations
int ezloadCallList(GLint callListIndex, FILE *fp);
int parse(FILE *fp);
int drawGroupCallList(group_t * group, GLint callListIndex);

#endif // EZLOADER_H_