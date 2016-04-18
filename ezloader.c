#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#define INITIAL_ARRAY_SIZE (512)


/* Considerations:
	Extend to two diminsions later?
	Process-and-discard information, or store for user to alter?
		For now, process-and-discard. Get simple version running.
	Later, use texturing.
	What if not using texture vertices?
	Can vertices be shared between groups? Hows that affect normals?
	*/

// Globals
	GLfloat * vertices;
	GLfloat * textureVertices;
	GLfloat * vertexNormals;
	element_t * elements;

	int arraySize, numVertices;
	//int textureVerticesArraySize, numTextureVertices;
	//int vertexNormalsArraySize,
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
	if(twonorm == 0){
		return;
	}
	//printf("Old vector: %f %f %f\n", v[0], v[1], v[2]);
	v[0] /= twonorm;
	v[1] /= twonorm;
	v[2] /= twonorm;
	//printf("twonorm: %f\n", twonorm);
	//printf("New vector: %f %f %f\n", v[0], v[1], v[2]);
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

	// textures
	if(textureVertexIndex > 0){
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
	}
	else{
		// No texures
		glDisable(GL_TEXTURE_2D);
	}

	// Normals - even if auto generated
	//int i;
	// Normalize all vertexNormals
		// I am assuming this is done automatically
	//for(i = 0; i < numVertices; i++){
		//normalize(&(vertexNormals[i*3]));
	//}
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, vertexNormals);
}

// Draws all elements from the array
void flushFaces(){
	/* Plan:
	Each vertex has a set of assocNormals.
	Every time a vertex is made, sets its assocNormals to zero vectors.
	Upon flushFaces():
	For each outstanding face, find face normal. For each vertex on this face, add this normal to the next non-zero assocNormal.
	For each vertex, sum & normalize its assocNorms, put into vertexNormals.
	Then generateVertexArrays(...).
	Then, for each face, draw it.
	*/
	generateVertexArrays();
	int i;
	// for each element...
	for(i = 0; i < numElements; i++){
		int numIndices = elements[i].numPoints;
		assert(elements[i].vertexIndices[0] < numVertices);
		assert(elements[i].vertexIndices[1] < numVertices);
		assert(elements[i].vertexIndices[2] < numVertices);
		if(numIndices == 3){
			glBegin(GL_TRIANGLES);
				glArrayElement(elements[i].vertexIndices[0]);
				glArrayElement(elements[i].vertexIndices[1]);
				glArrayElement(elements[i].vertexIndices[2]);
			glEnd();
			//printf("Drew triangle: %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0]);
		}
		else if(numIndices == 4){
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

/* Returns 0 on success.
*/
int ezload(FILE * fp){
	// glPointSize(...)?
	// glEnableClientState in generateVertexArrays

	// Set initial bookkeeping values
	//numVertices = 0;
	arraySize = elementArraySize = INITIAL_ARRAY_SIZE;	// number of GLfloats/element_t's
	vertexIndex = textureVertexIndex = vertexNormalIndex = numElements = 0;
		// No items have been created

	// Allocate arrays
	vertices 		= malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	textureVertices = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	vertexNormals 	= malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	elements 		= malloc(INITIAL_ARRAY_SIZE * sizeof(element_t));

	// Mark textures and normals as uninitialized
	textureVertices[0] 	= -1;
	vertexNormals[0] 	= -1;

	// For each line in .obj...
	while(!feof(fp)){
		// Tokenize line
		char tokens[32][MAX_TOKEN_SIZE + 1]; // max 32 tokens of MAX_TOKEN_SIZE chars
		int i;
		char * line = NULL;
		size_t linelength = 0;
		getline(&line, &linelength, fp);// Allocates
		// Initialize each token to NULL
		for(i = 0; i < 16; i++){
			tokens[i][0] = '\0';
		}
		i = 0;
		char * temp;
		//char str[MAX_TOKEN_SIZE + 1];
		temp = strtok(line, " \n");
		while(temp != NULL){
			// Check if token is too large
			if(strlen(temp) > MAX_TOKEN_SIZE){
				printf("Token too large!\n");
				exit(1);
			}
			// copy token into tokens array
			strcpy(tokens[i++], temp);		// @ERROR
			temp = strtok(NULL, " \n");
		}
		//printf("%s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
		free(line);
		// Line successfully tokenized

		// Choose operation
		if(!strcmp(tokens[0], "mtllib")){
			strcpy(mtllibName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "g")){
			//printf("New group\n");
			//@TODO Concatenate words before strcpy'ing
			strcpy(groupName, tokens[1]);
			flushFaces();
		}
		else if (!strcmp(tokens[0], "usemtl")){
			strcpy(matName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "v")){
			//printf("New vertex\n");
			numVertices++;
			// Realloc if no room left
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
			// Check if arrays are large enough
			if(textureVertexIndex + 2 > arraySize){
				lengthenArrays();
			}
			textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
		}
		else if(!strcmp(tokens[0], "vn")){
			// Check if arrays are large enough
			if(vertexNormalIndex + 3 > arraySize){
				lengthenArrays();
			}
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[3], NULL);
		}
		else if(!strcmp(tokens[0], "p")){
			printf("Skipping point\n");
		}
		else if(!strcmp(tokens[0], "l")){
			printf("Skipping line\n");
		}
		else if(!strcmp(tokens[0], "f")){
			// Parse out indices
			int numIndices;
			//printf("--Tokens: \n[%s], \n[%s], \n[%s], \n[%s]\n", tokens[0], tokens[1], tokens[2], tokens[3]);
				// Weird "\0" string behavior above
			if('\0' != tokens[4][0])
				numIndices = 4;
			else
				numIndices = 3;
			GLint indices[4][3];
				// By index, then by vertex/texture/normal
			// Mark these textures and normals as uninitialized
			indices[0][1] = indices[0][2] = -1;
			//@TODO: can vertex/texture/normal even possibly be different with glarrays?
			// Build indices 2D array.
			// For tokens 1 thru numVertices...
			for(i = 0; i < numIndices; i++){
				// Split in three
				// strtoi, then into GLint
				int vtn = 0;
				temp = strtok(tokens[i + 1], "/");
					// temp is the same as early in funciton, but currently not in use
				while(NULL != temp){
					if(temp[0] == '\0')
						// Got two '/' next to each other: no texture
						indices[i][vtn++] = -1;
					else
						indices[i][vtn++] = (GLint)strtol(temp, NULL, 0) - 1;
					temp = strtok(NULL, "/");
				}
			}
			// indices 2D array built.
			// Sanity check
			assert(indices[0][0] < numVertices);
			assert(indices[1][0] < numVertices);
			assert(indices[2][0] < numVertices);
			if(numIndices == 4)
				assert(indices[3][0] < numVertices);


			// Add new element
			if(numElements >= elementArraySize){
				elements = realloc(elements, 2*elementArraySize*sizeof(element_t));
				elementArraySize *= 2;
			}
			numElements++;
			elements[numElements - 1].type 						= 'f';
			elements[numElements - 1].numPoints 				= numIndices;
			elements[numElements - 1].vertexIndices[0] 			= indices[0][0];
			elements[numElements - 1].vertexIndices[1] 			= indices[1][0];
			elements[numElements - 1].vertexIndices[2] 			= indices[2][0];
			elements[numElements - 1].textureVertexIndices[0] 	= indices[0][1];
			elements[numElements - 1].textureVertexIndices[1] 	= indices[1][1];
			elements[numElements - 1].textureVertexIndices[2] 	= indices[2][1];
			elements[numElements - 1].vertexNormalIndices[0] 	= indices[0][2];
			elements[numElements - 1].vertexNormalIndices[1] 	= indices[1][2];
			elements[numElements - 1].vertexNormalIndices[2] 	= indices[2][2];
			if(4 == numIndices){
				elements[numElements - 1].vertexIndices[3] 			= indices[3][0];
				elements[numElements - 1].textureVertexIndices[2] 	= indices[3][1];
				elements[numElements - 1].vertexNormalIndices[2] 	= indices[3][2];
			}

			// If was not given normal, then autogenerate.
			if(-1 == indices[0][2]){
				// Get surface normal
				GLfloat surfaceNormal[3];
				// Find u and v
				GLfloat u[3], v[3];
				// u is first vertex to second, v is second to third
				u[0] = vertices[3*indices[1][0]]		- vertices[3*indices[0][0]];
				u[1] = vertices[3*indices[1][0] + 1] 	- vertices[3*indices[0][0] + 1];
				u[2] = vertices[3*indices[1][0] + 2] 	- vertices[3*indices[0][0] + 2];
				v[0] = vertices[3*indices[2][0]] 		- vertices[3*indices[1][0]];
				v[1] = vertices[3*indices[2][0] + 1] 	- vertices[3*indices[1][0] + 1];
				v[2] = vertices[3*indices[2][0] + 2] 	- vertices[3*indices[1][0] + 2];
				crossProduct(v, u, surfaceNormal);
				//printf("CP: %f, %f, %f\n", surfaceNormal[0], surfaceNormal[1], surfaceNormal[2]);
				// For each index, add surfaceNormal to its vertexNormals
				//printf("--numIndices: %i\n", numIndices);
				//printf("Tokens[4][0]: '%c'\n", tokens[4][0]);
				for(i = 0; i < numIndices; i++){
					int thisIndex = indices[i][0];
					//@TODO: figure out why this is minus, not plus
					vertexNormals[thisIndex*3]		-= surfaceNormal[0];
					vertexNormals[thisIndex*3 + 1] 	-= surfaceNormal[1];
					vertexNormals[thisIndex*3 + 2] 	-= surfaceNormal[2];
					//printf("Added SurfNorm for vertex %i\n", thisIndex);
				}
			}
		}
	}
	flushFaces();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return 0;
}

int ezloadCallList(GLint callListIndex, FILE *fp){
	//glPointSize(2.0);
	int retVal;
	glNewList(callListIndex, GL_COMPILE);
	{
		retVal = ezload(fp);
	}
	glEndList();
	return retVal;
}