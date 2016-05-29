#ifndef EZLOADER_H_
#define EZLOADER_H_
#include <GL/glut.h>
#include <stdio.h>
#define MAX_TOKEN_SIZE	(128)

// global declarations
// 	None

// function declarations
int ezload(FILE *fp);
int ezloadCallList(GLint callListIndex, FILE *fp);

#endif // EZLOADER_H_