#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

/* The plan:
	This loader will only handle points, lines, and faces, and ignore the rest.

	Create a group_t struct, with name, material name, number of vertices, vertex array, texture vertex array, normal array, and number of elements, and element_t array.

	Create element_t struct, with type, and vertex index array.

	To parse:
	Tokenize line.
	If begins with g, start new group.
	If begins with v, add vertex.
	If begins with vt, ignore.
	If begins with vn, add normal.
	If begins with p/l/f, add element.

	To load:
	glBegin();
	For each element, set normal and send it.
	glEnd();
*/

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
}

int ezloadCallList(GLint callListIndex, FILE *fp){
	glPointSize(2.0);
	glNewList(callListIndex, GL_COMPILE);
	{
		int line = 0;
		glPushMatrix();
		glBegin(GL_TRIANGLE_FAN);//GL_POINTS);
		while(!feof(fp)){
	 	 	read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
	 	 	line++;
	 	 	if (line == 131727){
	 	 		//printf("LINE: %d\n", line);
	 	 		//printf("%c, %f, %f, %f\n", ch, x, y, z);
	 	 		drawingPropeller = true;
	 	 		//glRotatef(angle, 0, 1, 0);
	 	 	}
	 	 	//printf("%d, %c, %f, %f, %f\n", line, ch, x, y, z);

	  		if(read==4 && ch=='v')
	  		{
	  			//printf("w");
	   			glVertex3f(x,y,z);
	  		}
	  		else if (ch != '\r'){
	  			//printf("E\n");
	  			glEnd();
	  			glBegin(GL_TRIANGLE_FAN);
	  		}
		}
		glEnd();
		glPopMatrix();
		
	}
	glEndList();
}