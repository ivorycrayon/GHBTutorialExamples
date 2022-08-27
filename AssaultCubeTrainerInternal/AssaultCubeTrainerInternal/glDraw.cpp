#include "glDraw.h"

void GL::SetupOrtho()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0 , -1, 1); //some games have different near and far value - use an api hooking program to get these values
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}
void GL::RestoreGL()
{
	glPopMatrix(); //every game is different on these calls
	glPopAttrib();
}

void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS); //immediate mode drawing
	glVertex2f(x, y); //top left
	glVertex2f(x + width, y); //top right
	glVertex2f(x + width, y + height); //bottom right
	glVertex2f(x, y + height); //bottom left
	glEnd();
}
void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP); //we can define the line width
	glColor3ub(color[0], color[1], color[2]);

	glVertex2f(x -0.5f, y - 0.5f); 
	glVertex2f(x + width + 0.5f, y - 0.5f); 
	glVertex2f(x + width + 0.5f, y + height + 0.5f); 
	glVertex2f(x - 0.5f, y + height + 0.5f); 
	glVertex2f(x - 0.5f, y - 0.5f); 
	glEnd();
}