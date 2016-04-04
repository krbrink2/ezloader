#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>
#include <assert.h>
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
	GLfloat * assocNormals; // numVertices quartets of trios of GLfloats
	element_t * elements;
	int arraySize, numVertices;
	int elementArraySize, numElements;
	char groupName[MAX_TOKEN_SIZE + 1];
	char mtllibName[MAX_TOKEN_SIZE + 1];
	char matName[MAX_TOKEN_SIZE + 1];
	int arraysAreDirty;
	int vertexIndex, textureVertexIndex, vertexNormalIndex;

// Possibly rename? too similar to gl function
void generateVertexArrays(GLfloat * vertices, GLfloat * textureVertices, GLfloat * vertexNormals){
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0, vertexNormals);
	//glTexCoordPointer();
}

// Draws all faces in 
void flushFaces(){
	//@TODO
	/* Plan:
	Each vertex has a set of assocNormals.
	Every time a vertex is made, sets its assocNormals to zero vectors.
	Upon flushFaces():
	For each outstanding face, find face normal. For each vertex on this face, add this normal to the next non-zero assocNormal.
	For each vertex, sum & normalize its assocNorms, put into vertexNormals.
	Then generateVertexArrays(...).
	Then, for each face, draw it.
	*/
	;
}

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

/* Returns 0 on success.
*/
int ezload(FILE * fp){
	// glPointSize(...)?
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURECOORD_ARRAY);

	// Set initial bookkeeping values
	numVertices = 0;
	arraySize = elementArraySize = INITIAL_ARRAY_SIZE;	// number of GLfloats/element_t's
	arraysAreDirty = 1;			// That is, need to generateArrays
	vertexIndex = 0;			// Index of items that has NOT been created
	textureVertexIndex = 0;
	vertexNormalIndex = 0;
	numElements = 0;
	// Allocate arrays
	vertices = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	textureVertices = NULL;//malloc(INITIAL_ARRAY_SIZE*sizeof(GLfloat));
	vertexNormals = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	elements = malloc(INITIAL_ARRAY_SIZE * sizeof(element_t));
	assocNormals = malloc(INITIAL_ARRAY_SIZE * 12 * sizeof(GLfloat));

	// For each line in .obj...
	while(!feof(fp)){
		// Tokenize line
		char tokens[32][MAX_TOKEN_SIZE + 1]; // max 32 tokens of MAX_TOKEN_SIZE chars
		int i;
		char * line = NULL;
		size_t linelength = 0;
		getline(&line, &linelength, fp);// Allocates
		// Nullify each token
		for(i = 0; i < 16; i++){
			tokens[i][0] = '\0';
		}
		i = 0;
		char * temp;
		char str[MAX_TOKEN_SIZE + 1];
		temp = strtok(line, " ");
		while(temp != NULL){
			// Check if token is too large
			if(strlen(temp) > MAX_TOKEN_SIZE){
				printf("Token too large!\n");
				exit(1);
			}
			// copy token into tokens array
			strcpy(tokens[i++], temp);				
			temp = strtok(NULL, " ");
		}
		//printf("%s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
		free(line);
		// String successfully tokenized

		// Populate arrays
		if(!strcmp(tokens[0], "mtllib")){
			strcpy(mtllibName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "g")){
			//printf("New group\n");
			strcpy(groupName, tokens[1]);
		}
		else if (!strcmp(tokens[0], "usemtl")){
			strcpy(matName, tokens[1]);
		}
		else if(!strcmp(tokens[0], "v")){
			//printf("New vertex\n");
			numVertices++;
			// Realloc if needed
			if(numVertices*3 > arraySize){
				vertices = realloc(vertices, 2*arraySize*sizeof(GLfloat));
				// textureVertices...
				vertexNormals = realloc(vertexNormals, 2*arraySize*sizeof(GLfloat));
				assocNormals = realloc(assocNormals, 2*arraySize*12*sizeof(GLfloat));
				arraySize *= 2;
			}
			// Add trio to vertexArray
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[3], NULL);
			// Nullify assocNormals
			int assocNormalsIndex = (numVertices - 1)*12;
			int offset;
			for (offset = 0; offset < 12; offset++){
				assocNormals[assocNormalsIndex + offset] = 0;
			}
			arraysAreDirty = 1;
		}
		else if(!strcmp(tokens[0], "vt")){
			//printf("New texture vertex\n");
			// Note: these are in pairs, not trios
			//@TODO reconfigure reallocing to account for duos, not trios
			//@TODO allow this to dynamicallly reallocate here. Not tied to vertices.
			/*
			groupPtr->textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			groupPtr->textureVertices[textureVertexIndex++] = (GLfloat)strtod(tokens[2], NULL);*/
		}
		else if(!strcmp(tokens[0], "vn")){
			//printf("New vertex normal\n");
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[3], NULL);
			arraysAreDirty  = 1;
		}
		else if(!strcmp(tokens[0], "p")){
			printf("Skipping point\n");
		}
		else if(!strcmp(tokens[0], "l")){
			printf("Skipping line\n");
		}
		else if(!strcmp(tokens[0], "f")){
			//printf("New face\n");
			// Parse out indices
			int numIndices;
			if(tokens[4][0]){
				numIndices = 4;
			}
			else{
				numIndices = 3;
			}
			// by index, then by vertex/texture/normal
			GLint indices[4][3];
			//@TODO: can vertex/texture/normal even possibly be different with glarrays?
			// Build indices 2D array.
			// For tokens 1 thru numVertices...
			for(i = 0; i < numIndices; i++){
				// Split in three
				// strtoi, then into GLint
				int a = 0;
				temp = strtok(tokens[i + 1], "/");
				while(NULL != temp){
					// @NOTE: Not using texture vertices?
					//	i.e. 15//20? Wat do?
					if(temp[0] == '\0'){
						// Not using texture vertices here
					}
					indices[i][a++] = (GLint)strtol(temp, NULL, 0) - 1;
					temp = strtok(NULL, "/");
				}
			}
			// indices 2D array built.
			// Sanity check
			assert(indices[0][0] < numVertices);
			assert(indices[1][0] < numVertices);
			assert(indices[2][0] < numVertices);
			if(numIndices == 4){
				assert(indices[3][0] < numVertices);
			}

			// Renew GL vertex arrays if needed
			if(arraysAreDirty){
				generateVertexArrays(vertices, textureVertices, vertexNormals);
				arraysAreDirty = 0;
			}

			// Add new element
			if(numElements >= elementArraySize){
				elements = realloc(elements, 2*elementArraySize*sizeof(element_t));
				elementArraySize *= 2;
			}
			numElements++;
			int ind = numElements - 1;
			elements[ind].type = 'f';
			elements[ind].numVertices = numIndices;
			elements[ind].vertexIndices[0] = indices[0][0];
			elements[ind].vertexIndices[1] = indices[1][0];
			elements[ind].vertexIndices[2] = indices[2][0];
			elements[ind].textureVertexIndices[0] = indices[0][1];
			elements[ind].textureVertexIndices[1] = indices[1][1];
			elements[ind].textureVertexIndices[2] = indices[2][1];
			elements[ind].vertexNormalIndices[0] = indices[0][2];
			elements[ind].vertexNormalIndices[1] = indices[1][2];
			elements[ind].vertexNormalIndices[2] = indices[2][3];

			// Get surface normal
			GLfloat surfaceNormal[3];
			// Find u and v
			GLfloat u[3], v[3];
			u[0] = vertices[3*indices[1][0]]		- vertices[3*indices[0][0]];
			u[1] = vertices[3*indices[1][0] + 1] 	- vertices[3*indices[0][0] + 1];
			u[2] = vertices[3*indices[1][0] + 2] 	- vertices[3*indices[0][0] + 2];
			v[0] = vertices[3*indices[2][0]] 		- vertices[3*indices[1][0]];
			v[1] = vertices[3*indices[2][0] + 1] 	- vertices[3*indices[1][0] + 1];
			v[2] = vertices[3*indices[2][0] + 2] 	- vertices[3*indices[1][0] + 2];
			crossProduct(v, u, surfaceNormal);
			//printf("CP: %f, %f, %f\n", surfaceNormal[0], surfaceNormal[1], surfaceNormal[2]);
			// For each index, add surfaceNormal to next non-zero assocNormal
			for(i = 0; i < numIndices; i++){
				int thisIndex = indices[i][0];
				// Find first non-zero assocNormal
				int assocNormalIndex = 12*thisIndex;
				int j;
				for(j = 0; j < 4; j++){
					if(assocNormals[assocNormalIndex + 3*j] == 0
							&& assocNormals[assocNormalIndex + 3*j + 1] == 0
							&& assocNormals[assocNormalIndex + 3*j + 2] == 0){
						assocNormals[assocNormalIndex + 3*j] 		= surfaceNormal[0];
						assocNormals[assocNormalIndex + 3*j + 1] 	= surfaceNormal[1];
						assocNormals[assocNormalIndex + 3*j + 1]	= surfaceNormal[2];
						break;
					}
					else if(j == 3){
						printf("No free assocNormals found for vertex %i!\n", thisIndex);
						exit(1);
					}
				}
				printf("Added SurfNorm for vertex %i\n", thisIndex);
			}

			// OLD below
			if(numIndices == 3){
				assert(indices[0][0] < numVertices);
				assert(indices[1][0] < numVertices);
				assert(indices[2][0] < numVertices);
				glBegin(GL_TRIANGLES);
					glArrayElement(indices[0][0]);
					glArrayElement(indices[1][0]);
					glArrayElement(indices[2][0]);
				glEnd();
				//printf("Drew triangle: %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0]);
			}
			else if(numIndices == 4){
				assert(indices[0][0] < numVertices);
				assert(indices[1][0] < numVertices);
				assert(indices[2][0] < numVertices);
				assert(indices[3][0] < numVertices);
				glBegin(GL_QUADS);
					glArrayElement(indices[0][0]);
					glArrayElement(indices[1][0]);
					glArrayElement(indices[2][0]);
					glArrayElement(indices[3][0]);
				glEnd();
				//printf("Drew quad: %i, %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0], vertices[3][0]);
			}
			else{
				printf("Unrecognized number of vertices!\n");
				exit(1);
			}
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	// texcoords, too
}

int ezloadCallList(GLint callListIndex, FILE *fp){
	//glPointSize(2.0);
	glNewList(callListIndex, GL_COMPILE);
	{
		ezload(fp);
	}
	glEndList();
}