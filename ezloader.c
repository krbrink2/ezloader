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
	*/

/* The plan:
	This loader will only handle points, lines, and faces, and ignore the rest.

	Create a group_t struct, with name, material name, number of vertices, vertex array, texture vertex array, normal array, and number of elements, and element_t array.

	Create element_t struct, with type, and vertex index array.

	To parse:
	Tokenize line.
	If begins with g, start new group.
	If begins with v, add vertex.
	If begins with vt, ignore (for now).
	If begins with vn, add normal.
	If begins with p/l/f, add element.

	To load:
	glBegin();
	For each element, set normal and send it.
	glEnd();
*/

// Possibly rename? too similar to gl function
void generateVertexArrays(GLfloat * vertices, GLfloat * textureVertices, GLfloat * vertexNormals){
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0, vertexNormals);
	//glTexCoordPointer();
}

/* Returns 0 on success.
*/
int ezload(FILE * fp){
	// glPointSize(...)?
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURECOORD_ARRAY);
	char groupName[MAX_TOKEN_SIZE + 1];
	char mtllibName[MAX_TOKEN_SIZE + 1];
	char matName[MAX_TOKEN_SIZE + 1];
	int numVertices = 0;
	int arraySize = INITIAL_ARRAY_SIZE;	// number of GLfloats in the below
	GLfloat * vertices = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	GLfloat * textureVertices = NULL;//malloc(INITIAL_ARRAY_SIZE*sizeof(GLfloat));
	GLfloat * vertexNormals = malloc(INITIAL_ARRAY_SIZE * sizeof(GLfloat));
	int arraysAreDirty = 1;	// That is, need to generateArrays
	int vertexIndex = 0;
	int textureVertexIndex = 0;
	int vertexNormalIndex = 0;
	while(!feof(fp)){
		// Tokenize
		char * line = NULL;
		size_t linelength = 0;
		getline(&line, &linelength, fp);// Allocates
		char tokens[16][MAX_TOKEN_SIZE + 1]; // max 16 tokens of MAX_TOKEN_SIZE chars
		int i;
		for(i = 0; i < 16; i++){
			tokens[i][0] = '\0';
		}
		i = 0;
		char * temp;
		char str[MAX_TOKEN_SIZE + 1];
		temp = strtok(line, " ");
		while(temp != NULL){
			// copy token into tokens array
			if(strlen(temp) > MAX_TOKEN_SIZE){
				printf("Token too large!");
				exit(1);
			}
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
				//groupPtr->textureVertices = realloc(groupPtr->textureVertices, 2*groupPtr->arraySize*sizeof(GLfloat));
				vertexNormals = realloc(vertexNormals, 2*arraySize*sizeof(GLfloat));
				arraySize *= 2;
			}
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
			vertices[vertexIndex++] = (GLfloat)strtod(tokens[3], NULL);
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
			if(arraysAreDirty){
				generateVertexArrays(vertices, textureVertices, vertexNormals);
				arraysAreDirty = 0;
			}
			int numIndices;
			if(tokens[4][0]){
				numIndices = 4;
			}
			else{
				numIndices = 3;
			}
			// by index, then by vertex/texture/normal
			GLint vertices[4][3];
			//@TODO: can vertex/texture/normal even possibly be different with glarrays?
			// Build vertices 2D array.
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
					vertices[i][a++] = (GLint)strtol(temp, NULL, 0) - 1;
					temp = strtok(NULL, "/");
				}
			}
			// Draw
			if(numIndices == 3){
				assert(vertices[0][0] < numVertices);
				assert(vertices[1][0] < numVertices);
				assert(vertices[2][0] < numVertices);
				glBegin(GL_TRIANGLES);
					glArrayElement(vertices[0][0]);
					glArrayElement(vertices[1][0]);
					glArrayElement(vertices[2][0]);
				glEnd();
				//printf("Drew triangle: %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0]);
			}
			else if(numIndices == 4){
				assert(vertices[0][0] < numVertices);
				assert(vertices[1][0] < numVertices);
				assert(vertices[2][0] < numVertices);
				assert(vertices[3][0] < numVertices);
				glBegin(GL_QUADS);
					glArrayElement(vertices[0][0]);
					glArrayElement(vertices[1][0]);
					glArrayElement(vertices[2][0]);
					glArrayElement(vertices[3][0]);
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