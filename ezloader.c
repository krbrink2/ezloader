#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#define INITIAL_ARRAY_SIZE	(512)
#define MAX_NUM_TOKENS		(32)

/* Considerations:
	Extend to two diminsions later?
	*/

// Typedefs
typedef struct element_t{
	char type;
	short numPoints;
	int vertexIndices[4];
	int textureVertexIndices[4];
	int vertexNormalIndices[4];
} element_t;

// Globals
	GLfloat * vertices;
	GLfloat * textureVertices;
	GLfloat * vertexNormals;
	element_t * elements;

	int arraySize, numVertices;
	int elementArraySize, numElements;
	int vertexIndex, textureVertexIndex, vertexNormalIndex;
		// Each above points to the next open spot
		// Don't need elementIndex because have numElements.

	char groupName[MAX_TOKEN_SIZE + 1];
	char mtllibName[MAX_TOKEN_SIZE + 1];
	char matName[MAX_TOKEN_SIZE + 1];

// Takes the crossproduct of u and v, and stores it in product.
void crossProduct(GLfloat u[], GLfloat v[], GLfloat product[]){
	GLfloat p[3];
	p[0] = u[1]*v[2] - u[2]*v[1];
	p[1] = u[2]*v[0] - u[0]*v[2];
	p[2] = u[0]*v[1] - u[1]*v[0];
	product[0] = p[0];
	product[1] = p[1];
	product[2] = p[2];
}

// Given a vector, normalizes it
void normalize(GLfloat v[]){
	GLfloat twonorm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if(twonorm == 0.){
		return;
	}
	v[0] /= twonorm;
	v[1] /= twonorm;
	v[2] /= twonorm;
}

// Reallocs vertices, textureVertices and vertexNormals to double size
void lengthenArrays(){
	vertices 		= realloc(vertices, 		2*arraySize*sizeof(GLfloat));
	textureVertices = realloc(textureVertices, 	2*arraySize*sizeof(GLfloat));
	vertexNormals 	= realloc(vertexNormals, 	2*arraySize*sizeof(GLfloat));
	arraySize *= 2;
}

// Possibly rename? too similar to gl function
void generateVertexArrays(){
	// Vertex indices
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	// Textures
	if(textureVertexIndex > 0){
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
	}
	else{
		// No texures
		glDisable(GL_TEXTURE_2D);
	}

	// I am assuming surfaces normals are automatically normalized.
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, vertexNormals);
}

// Draws all elements from the array
void flushElements(){
	generateVertexArrays();
	int i;
	// for each element...
	for(i = 0; i < numElements; i++){
		int numPoints = elements[i].numPoints;
		assert(elements[i].vertexIndices[0] < numVertices);
		assert(elements[i].vertexIndices[1] < numVertices);
		assert(elements[i].vertexIndices[2] < numVertices);
		if(numPoints == 3){
			glBegin(GL_TRIANGLES);
				glArrayElement(elements[i].vertexIndices[0]);
				glArrayElement(elements[i].vertexIndices[1]);
				glArrayElement(elements[i].vertexIndices[2]);
			glEnd();
			//printf("Drew triangle: %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0]);
		}
		else if(numPoints == 4){
			assert(elements[i].vertexIndices[3] < numVertices);
			glBegin(GL_QUADS);
				glArrayElement(elements[i].vertexIndices[0]);
				glArrayElement(elements[i].vertexIndices[1]);
				glArrayElement(elements[i].vertexIndices[2]);
				glArrayElement(elements[i].vertexIndices[3]);
			glEnd();
			//printf("Drew quad: %i, %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0], vertices[3][0]);
		}
		else{
			printf("Unrecognized number of vertices!\n");
			exit(1);
		}
	}
	free(elements);
	numElements = 0;
	elementArraySize = INITIAL_ARRAY_SIZE;
	elements = malloc(INITIAL_ARRAY_SIZE * sizeof(element_t));
}

// Deallocate all dynamic memory.
void freeAll(){
	free(vertices);
	free(textureVertices);
	free(vertexNormals);
	free(elements);
}

/* Returns 0 on success.
*/
int ezload(FILE * fp){
	// Set initial bookkeeping values
	numVertices = 0;
	arraySize = elementArraySize = INITIAL_ARRAY_SIZE;	// number of GLfloats/element_t's
	vertexIndex = textureVertexIndex = vertexNormalIndex = numElements = 0;

	// Allocate arrays
	vertices 		= malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	textureVertices = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	vertexNormals 	= malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	elements 		= malloc(INITIAL_ARRAY_SIZE * sizeof(element_t));

	// Mark textures and normals as uninitialized
	textureVertices[0] 	= -1;
	vertexNormals[0] 	= -1;

	// Process each line in fp one at a time.
	while(!feof(fp)){
		// Tokenize line
		char tokens[MAX_NUM_TOKENS][MAX_TOKEN_SIZE + 1];
		int i, j;
		char * line = calloc(1, MAX_TOKEN_SIZE + 1);
		size_t lineLength = MAX_TOKEN_SIZE + 1;
		//const char delim[] = " \n";

		getline(&line, &lineLength, fp);// Allocates
		// Initialize each token to NULL
		for(i = 0; i < MAX_NUM_TOKENS; i++){
			for(j = 0; j < MAX_TOKEN_SIZE + 1; j++){
				tokens[i][j] = '\0';
			}
		}
		char * temp = strtok(line, " \n");	//@ERROR: tons of uninitialization.
		i = 0;
		while(temp != NULL){
			// Check if token is too large
			if(strlen(temp) > MAX_TOKEN_SIZE){
				printf("Token too large!\n");
				freeAll();
				return 1;
			}
			// copy token into tokens array
			strcpy(tokens[i++], temp);
			temp = strtok(NULL, " \n");
		}
		free(line);
		// Line has now been successfully tokenized.

		if(!strcmp(tokens[0], "mtllib")){
			strcpy(mtllibName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "g")){
			//@TODO Concatenate words before strcpy'ing
			strcpy(groupName, tokens[1]);
			flushElements();
		}
		else if (!strcmp(tokens[0], "usemtl")){
			strcpy(matName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "v")){
			numVertices++;
			// Realloc if no room left.
			if(vertexIndex + 3 > arraySize){
				lengthenArrays();
			}
			// Add trio to vertexArray, and nullify this vertexNormal
			vertexNormals[vertexIndex] = 0;
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertexNormals[vertexIndex] = 0;
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertexNormals[vertexIndex] = 0;
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[3], NULL);
		}
		else if(!strcmp(tokens[0], "vt")){
			// Realloc if no room left.
			if(textureVertexIndex + 2 > arraySize){
				lengthenArrays();
			}
			textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
		}
		else if(!strcmp(tokens[0], "vn")){
			// Realloc if no room left.
			if(vertexNormalIndex + 3 > arraySize){
				lengthenArrays();
			}
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[3], NULL);
		}
		else if(!strcmp(tokens[0], "p")){
			printf("ezloader: Points not supported!\n");
			freeAll();
			return 1;
		}
		else if(!strcmp(tokens[0], "l")){
			printf("ezloader: Lines not supported!\n");
			freeAll();
			return 1;
		}
		else if(!strcmp(tokens[0], "f")){
			// Parse out points
			int numPoints;	// 3 if triangle, 4 if quad, etc.
			if('\0' != tokens[4][0])
				numPoints = 4;
			else
				numPoints = 3;
			GLint points[4][3];
				// By point, then by vertex/texture/normal
			// Mark these textures and normals as uninitialized
			points[0][1] = points[0][2] = -1;
			//@TODO: can vertex/texture/normal even possibly be different with glarrays?
			// Build points 2D array.
			// Example: parse "3/1/4 1/5/9 3/6/7"
			for(i = 0; i < numPoints; i++){
				int vtn = 0;	// vertex/texture/normal
				temp = strtok(tokens[i + 1], "/");
				while(NULL != temp){
					if(0 == strlen(temp))
						//@TODO: This is NOT how strtok works. This will NOT properly read v//n.
						// Got two '/' next to each other: no texture
						points[i][vtn++] = -1;
					else
						points[i][vtn++] = (GLint)strtol(temp, NULL, 0) - 1;
					temp = strtok(NULL, "/");
				}
			}
			// Points 2D array has been built.
			// Sanity check
			assert(points[0][0] < numVertices);
			assert(points[1][0] < numVertices);
			assert(points[2][0] < numVertices);
			if(numPoints == 4)
				assert(points[3][0] < numVertices);

			// Add new element. Realloc if not enough room left.
			if(numElements >= elementArraySize){
				elements = realloc(elements, 2*elementArraySize*sizeof(element_t));
				elementArraySize *= 2;
			}
			numElements++;
			elements[numElements - 1].type 						= 'f';
			elements[numElements - 1].numPoints 				= numPoints;
			elements[numElements - 1].vertexIndices[0] 			= points[0][0];
			elements[numElements - 1].vertexIndices[1] 			= points[1][0];
			elements[numElements - 1].vertexIndices[2] 			= points[2][0];
			elements[numElements - 1].textureVertexIndices[0] 	= points[0][1];
			elements[numElements - 1].textureVertexIndices[1] 	= points[1][1];
			elements[numElements - 1].textureVertexIndices[2] 	= points[2][1];
			elements[numElements - 1].vertexNormalIndices[0] 	= points[0][2];
			elements[numElements - 1].vertexNormalIndices[1] 	= points[1][2];
			elements[numElements - 1].vertexNormalIndices[2] 	= points[2][2];
			if(4 == numPoints){
				elements[numElements - 1].vertexIndices[3] 			= points[3][0];
				elements[numElements - 1].textureVertexIndices[2] 	= points[3][1];
				elements[numElements - 1].vertexNormalIndices[2] 	= points[3][2];
			}

			// If was not given normal, then autogenerate.
			if(-1 == points[0][2]){
				GLfloat surfaceNormal[3], u[3], v[3];
				// u is first vertex to second, v is second to third
				u[0] = vertices[3*points[1][0]]			- vertices[3*points[0][0]];
				u[1] = vertices[3*points[1][0] + 1] 	- vertices[3*points[0][0] + 1];
				u[2] = vertices[3*points[1][0] + 2] 	- vertices[3*points[0][0] + 2];
				v[0] = vertices[3*points[2][0]] 		- vertices[3*points[1][0]];
				v[1] = vertices[3*points[2][0] + 1] 	- vertices[3*points[1][0] + 1];
				v[2] = vertices[3*points[2][0] + 2] 	- vertices[3*points[1][0] + 2];
				crossProduct(v, u, surfaceNormal);
				for(i = 0; i < numPoints; i++){
					int thisIndex = points[i][0];
					//@TODO: figure out why this is minus, not plus
					vertexNormals[thisIndex*3]		-= surfaceNormal[0];
					vertexNormals[thisIndex*3 + 1] 	-= surfaceNormal[1];
					vertexNormals[thisIndex*3 + 2] 	-= surfaceNormal[2];
				}
			}
		}
	}
	flushElements();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return 0;
}

int ezloadCallList(GLint callListIndex, FILE *fp){
	int retVal;
	glNewList(callListIndex, GL_COMPILE);
	{
		retVal = ezload(fp);
	}
	glEndList();
	return retVal;
}