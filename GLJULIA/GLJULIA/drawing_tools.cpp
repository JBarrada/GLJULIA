#include "drawing_tools.h"

DrawingTools::DrawingTools() {
	SCREEN_W = 1;
	SCREEN_H = 1;
}
DrawingTools::DrawingTools(double width, double height) {
	SCREEN_W = width;
	SCREEN_H = height;
}

void DrawingTools::reshape(double width, double height) {
	SCREEN_W = width;
	SCREEN_H = height;
}

double DrawingTools::screen_x(double x) {
	return (x / SCREEN_W)*2.0 - 1.0;
}
double DrawingTools::screen_y(double y) {
	return (y / SCREEN_H)*2.0 - 1.0;
}

void DrawingTools::circle_filled(double x, double y, double r) {
	glBegin(GL_POLYGON);
	double steps = 10;
	double step = (M_PI*2.0) / steps;
	for (int i = 0; i < steps; i++) {
		double sx = screen_x((cos(step*i)*r) + x);
		double sy = screen_y((sin(step*i)*r) + y);
		glVertex2f(sx, sy);
	}
	glEnd();
}
void DrawingTools::circle_hollow(double x, double y, double r) {
	glBegin(GL_LINE_LOOP);
	double steps = 10;
	double step = (M_PI*2.0) / steps;
	for (int i = 0; i < steps; i++) {
		double sx = screen_x((cos(step*i)*r) + x);
		double sy = screen_y((sin(step*i)*r) + y);
		glVertex2f(sx, sy);
	}
	glEnd();
}

void DrawingTools::rectangle_filled(double x, double y, double w, double h) {
	glBegin(GL_QUADS);
	glVertex2f(screen_x(x), screen_y(y));
	glVertex2f(screen_x(x+w), screen_y(y));
	glVertex2f(screen_x(x+w), screen_y(y+h));
	glVertex2f(screen_x(x), screen_y(y+h));
	glEnd();
}
void DrawingTools::rectangle_hollow(double x, double y, double w, double h) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(screen_x(x), screen_y(y));
	glVertex2f(screen_x(x + w), screen_y(y));
	glVertex2f(screen_x(x + w), screen_y(y + h));
	glVertex2f(screen_x(x), screen_y(y + h));
	glEnd();
}

void DrawingTools::text(double x, double y, void *font, string text) {
	double offset = 0;

	for (char c : text) {
		glRasterPos2f(screen_x(x + offset), screen_y(y));
		glutBitmapCharacter(font, c);
		offset += glutBitmapWidth(font, c);
	}
}
