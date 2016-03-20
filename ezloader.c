#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>

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

int ezloadCallList(GLint callListIndex, FILE *fp){
	//glPointSize(2.0);
	glNewList(callListIndex, GL_COMPILE);
	{
		group_t * groupPtr = NULL;
		int vertexIndex = 0;
		int textureVertexIndex = 0;
		int vertexNormalIndex = 0;
		while(!feof(fp)){
			char * line = NULL;
			size_t linelength = 0;
			getline(&line, &linelength, fp);// Allocates
			// Tokenize string
			char tokens[16][MAX_TOKEN_SIZE + 1]; // max 16 tokens of MAX_TOKEN_SIZE chars
			char * temp;
			int i = 0;
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
			/* Tokenize line.
			If begins with g, start new group.
			If begins with v, add vertex.
			If begins with vt, ignore (for now).
			If begins with vn, add normal.
			If begins with p/l/f, add element. */
			// String successfully tokenized
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
				groupPtr->arraySize = 128;	// initial number
				groupPtr->vertices = malloc(groupPtr->arraySize*3*sizeof(GLfloat	));
				groupPtr->textureVertices = malloc(groupPtr->arraySize*3*sizeof(GLfloat));
				groupPtr->vertexNormals = malloc(groupPtr->arraySize*3*sizeof(GLfloat));
				vertexIndex = textureVertexIndex = vertexNormalIndex = 0;
			}
			else if (!strcmp(tokens[0], "usemtl")){
				strcpy(groupPtr->matName, tokens[1]);
			}
			else if(!strcmp(tokens[0], "v")){
				//printf("New vertex\n");
				groupPtr->numVertices++;
				// Realloc if needed
				if(groupPtr->numVertices > 3*groupPtr->arraySize){
					groupPtr->vertices = realloc(groupPtr->vertices, 2*groupPtr->arraySize*sizeof(GLfloat));
					groupPtr->textureVertices = realloc(groupPtr->textureVertices, 2*groupPtr->arraySize*sizeof(GLfloat));
					groupPtr->vertexNormals = realloc(groupPtr->vertexNormals, 2*groupPtr->arraySize*sizeof(GLfloat));
					groupPtr->arraySize = 2*groupPtr->arraySize;
				}
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[1], NULL);
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[2], NULL);
				groupPtr->vertices[vertexIndex++] = (GLfloat)strtod(tokens[3], NULL);
			}
			else if(!strcmp(tokens[0], "vt")){
				//printf("New texture vertex\n");
			}
			else if(!strcmp(tokens[0], "vn")){
				//printf("New vertex normal\n");
			}
			else if(!strcmp(tokens[0], "p")){
				//printf("New plane\n");
			}
			else if(!strcmp(tokens[0], "l")){
				//printf("New line\n");
			}
			else if(!strcmp(tokens[0], "f")){
				//printf("New face\n");
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