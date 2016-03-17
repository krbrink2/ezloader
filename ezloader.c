#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <string.h>
#define MAX_TOKEN_SIZE 64

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
	group_t * group = NULL;
	glNewList(callListIndex, GL_COMPILE);
	{
		while(!feof(fp)){
			/* Tokenize line.
			If begins with g, start new group.
			If begins with v, add vertex.
			If begins with vt, ignore (for now).
			If begins with vn, add normal.
			If begins with p/l/f, add element. */
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
				if(strlen(temp) > MAX_TOKEN_SIZE){	// SEGFAULT
					printf("Token too large!");
					exit(1);
				}
				strcpy(tokens[i++], temp);				
				temp = strtok(NULL, " ");
			}
			printf("%s %s %s %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
			free(line);
			// String successfully tokenized


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
	}
	glEndList();
}