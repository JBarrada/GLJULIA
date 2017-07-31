#ifndef JULIA_STATE_H
#define JULIA_STATE_H

#include <math.h>
#include <glew.h>
#include <glut.h>

struct State1D {
	double x_exp;
	double x;
	double dx;
	bool cruise;

	void set(double x);
	void add(double x);
};

struct State2D {
	double x;
	double y;
	double dx;
	double dy;
	bool cruise;
};

class JuliaState {
	GLuint program_fp64;
	GLuint program_fp32;
public:
	int max_calc;
	int power;

	State1D zoom;

	State2D center;
	State2D c;

	State2D color;

	bool fp64;

	bool explore;
	
	bool menu_open;
	bool hud_open;

	JuliaState();
	JuliaState(GLuint program_fp64, GLuint program_fp32);
	void update_program_variables(double SCREEN_W, double SCREEN_H, double ASPECT);
};

#endif
