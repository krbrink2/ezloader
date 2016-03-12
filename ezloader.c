#include "ezloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

ezloadCallList
	glPointSize(2.0);
	glNewList(callListIndex, GL_COMPILE);
	{
		int line = 0;
		bool drawingPropeller = false;
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