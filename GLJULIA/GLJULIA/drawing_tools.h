#ifndef DRAWING_TOOLS_H
#define DRAWING_TOOLS_H

#include <cmath>
#include <glew.h>
#include <glut.h>

#include <string>

using namespace std;

#define M_PI 3.14159265359

class DrawingTools {
public:
	double SCREEN_W;
	double SCREEN_H;

	DrawingTools();
	DrawingTools(double width, double height);

	void reshape(double width, double height);

	double screen_x(double x);
	double screen_y(double y);

	void circle_filled(double x, double y, double r);
	void circle_hollow(double x, double y, double r);

	void rectangle_filled(double x, double y, double w, double h);
	void rectangle_hollow(double x, double y, double w, double h);

	void text(double x, double y, void *font, string text);

};

#endif
