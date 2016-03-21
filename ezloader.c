#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>

//@TODO extend to 2 dimensions
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

void generateVertexArrays(group_t * groupPtr){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURECOORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, groupPtr->vertices);
	glNormalPointer(GL_FLOAT, 0, groupPtr->vertexNormals);
	//glTexCoordPointer();
}

int ezloadCallList(GLint callListIndex, FILE *fp){
	//glPointSize(2.0);
	glNewList(callListIndex, GL_COMPILE);
	{
		group_t * groupPtr = NULL;
		int vertexIndex = 0;
		int textureVertexIndex = 0;
		int vertexNormalIndex = 0;
		int elementsIndex = 0;
		int readyToDraw = 0;
		while(!feof(fp)){
			char * line = NULL;
			size_t linelength = 0;
			getline(&line, &linelength, fp);// Allocates

			// Tokenize string
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
			// No switch with strings supported, so if-else ladder
			if(!strcmp(tokens[0], "mtllib")){
				strcpy(mtllibName, tokens[1]);
			}
			else if(!strcmp(tokens[0], "g")){
				//printf("New group\n");
				// If there's an old group_t, discard it.
				if(NULL != groupPtr){
					free(groupPtr->vertices);
					free(groupPtr->textureVertices);
					free(groupPtr->vertexNormals);
					free(groupPtr);
					groupPtr = NULL;
				}
				// Allocate new group_t
				groupPtr = (group_t*)malloc(sizeof(group_t));
				strcpy(groupPtr->name, tokens[1]);
				// Initialize group arrays
				groupPtr->arraySize = 256;	// initial number of GLfloats
				groupPtr->elementsArraySize = 256;
				groupPtr->vertices = malloc(groupPtr->arraySize*sizeof(GLfloat));
				//groupPtr->textureVertices = malloc(groupPtr->arraySize*sizeof(GLfloat));
				groupPtr->textureVertices = NULL;
				groupPtr->vertexNormals = malloc(groupPtr->arraySize*sizeof(GLfloat));
				groupPtr->elements = malloc(groupPtr->elementsArraySize*sizeof(element_t));
				groupPtr->numVertices = groupPtr->numElements = 0;
				vertexIndex = textureVertexIndex = vertexNormalIndex = 0;
				readyToDraw = 0;
			}
			else if (!strcmp(tokens[0], "usemtl")){
				strcpy(groupPtr->matName, tokens[1]);
			}
			else if(!strcmp(tokens[0], "v")){
				//printf("New vertex\n");
				groupPtr->numVertices++;
				// Realloc if needed
				if(groupPtr->numVertices*3 > groupPtr->arraySize){
					groupPtr->vertices = realloc(groupPtr->vertices, 2*groupPtr->arraySize*sizeof(GLfloat));
					//groupPtr->textureVertices = realloc(groupPtr->textureVertices, 2*groupPtr->arraySize*sizeof(GLfloat));
					groupPtr->vertexNormals = realloc(groupPtr->vertexNormals, 2*groupPtr->arraySize*sizeof(GLfloat));
					groupPtr->arraySize *= 2;
				}
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[3], NULL);
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
				groupPtr->vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[1], NULL);
				groupPtr->vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[2], NULL);
				groupPtr->vertexNormals[vertexNormalIndex++] = (GLfloat)strtod(tokens[3], NULL);
			}
			else if(!strcmp(tokens[0], "p")){
				printf("Skipping point\n");
			}
			else if(!strcmp(tokens[0], "l")){
				printf("Skipping line\n");
			}
			else if(!strcmp(tokens[0], "f")){
				//printf("New face\n");
				if(!readyToDraw){
					generateVertexArrays(groupPtr);
				}
				readyToDraw = 1;
				int numVertices;
				if(tokens[4][0]){
					numVertices = 4;
				}
				else{
					numVertices = 3;
				}
				// by vertex, then by vertex/texture/normal @TODO check order in .obj std
				GLint vertices[4][3];
				//@TODO: can vertex/texture/normal even possibly be different?
				// Build vertices 2D array.
				// For tokens 1 thru numVertices...
				for(i = 1; i <= numVertices; i++){
					// Split in three
					// strtoi, then into GLint
					int a = 0;
					temp = strtok(tokens[i], "/");
					while(NULL != temp){
						vertices[i][a++] = (GLint)strtol(temp, NULL, 0);
						temp = strtok(NULL, "/");
					}
				}
				// Draw
				if(numVertices == 3){
					glBegin(GL_TRIANGLES);
						glArrayElement(vertices[0][0]);
						glArrayElement(vertices[1][0]);
						glArrayElement(vertices[2][0]);
					glEnd();
					printf("Drew triangle: %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0]);
				}
				else if(numVertices == 4){
					glBegin(GL_QUADS);
						glArrayElement(vertices[0][0]);
						glArrayElement(vertices[1][0]);
						glArrayElement(vertices[2][0]);
						glArrayElement(vertices[3][0]);
					glEnd();
					printf("Drew quad: %i, %i, %i, %i\n", vertices[0][0], vertices[1][0], vertices[2][0], vertices[3][0]);
				}
				else{
					printf("Unrecognized number of vertices!\n");
					exit(1);
				}
				exit(0);
			}

			// test
			if(groupPtr != NULL && vertexNormalIndex == 3){

			}

	 	 	//read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
			/*
	  		if(read==4 && ch=='v')
	  		{
	  			//printf("w");
	   			glVertex3f(x,y,z);
	  		}
	  		else if (ch != '\r'){
	  			//printf("E\n");
	  			glEnd();
	  			glBegin(GL_TRIANGLE_FAN);
	  		}*/
		}
		free(groupPtr);
		groupPtr = NULL;
	}
	glEndList();
}