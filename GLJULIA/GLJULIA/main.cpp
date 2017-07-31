#include <iostream>
#include <cmath>
#include <glew.h>
#include <glut.h>

#include "julia_state.h"
#include "julia_gamepad.h"
#include "julia_explorer.h"
#include "julia_menu.h"
#include "drawing_tools.h"
#include "shader_utils.h"
#include "gamepad_input.h"

JuliaState julia_state;

static GLuint program_fp64;
static GLuint program_fp32;

int SCREEN_W = 600;
int SCREEN_H = 300;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

DrawingTools drawing(SCREEN_W, SCREEN_H);
GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);
JuliaGamepad julia_gamepad(pad, &julia_state, &drawing);
JuliaMenu julia_menu;

JuliaExplorer julia_explorer;
bool explore = false;

bool move_drag = false;
int move_drag_x;
int move_drag_y;

bool c_value_drag = 0;
int c_value_drag_x;
int c_value_drag_y;


void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	julia_state.update_program_variables(SCREEN_W, SCREEN_H, ASPECT);
	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();

	if (julia_state.menu_open) {
		julia_menu.draw_menu();
	}
	else if (julia_state.hud_open) {
		julia_gamepad.draw_hud();
	}

	/*
	glColor4f(1, 1, 1, 1);
	glBegin(GL_POINTS);
	glVertex2f(0, 0);
	glEnd();
	*/
	glutSwapBuffers();
}

void on_click(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			move_drag = true;
			move_drag_x = x;
			move_drag_y = y;
		}
		else if (state == GLUT_UP) {
			move_drag = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			c_value_drag = true;
			c_value_drag_x = x;
			c_value_drag_y = y;
		}
		else if (state == GLUT_UP) {
			c_value_drag = false;
		}
	}

	if (button == 3) {
		julia_state.zoom.add(-0.5);
	}
	if (button == 4) {
		julia_state.zoom.add(0.5);
	}
}

void on_move(int x, int y) {
	if (move_drag) {
		double fx = (x - move_drag_x) / (SCREEN_W / 2.0) * julia_state.zoom.x_exp;
		double fy = (y - move_drag_y) / (SCREEN_H / 2.0) * julia_state.zoom.x_exp;

		move_drag_x = x;
		move_drag_y = y;

		julia_state.center.x += -fx;
		julia_state.center.y += fy;
	}

	if (c_value_drag) {
		double fx = ((x - c_value_drag_x) / (SCREEN_W / 2.0) * julia_state.zoom.x_exp) / 500.0;
		double fy = ((y - c_value_drag_y) / (SCREEN_H / 2.0) * julia_state.zoom.x_exp) / 500.0;

		c_value_drag_x = x;
		c_value_drag_y = y;

		julia_state.c.x += -fx;
		julia_state.c.y += fy;
	}
}

void keyboard_down(unsigned char key, int x, int y) {
	if (key == 'g')
		explore = !explore;
	//julia_explorer.explore(SCREEN_W, SCREEN_H);
	if (key == 'h')
		julia_state.zoom.cruise = !julia_state.zoom.cruise;
	/*
	switch(key) {
	case 'w': color_range_value += 0.01; break;
	case 'a': color_offset_value += 0.01; break;
	case 's': color_range_value -= 0.01; break;
	case 'd': color_offset_value -= 0.01; break;

	case '=': max_calc_value += 10; break;
	case '-': max_calc_value -= 10; break;
	}

	if (color_offset_value > 1.0) color_offset_value -= 1.0;
	if (color_offset_value < 0.0) color_offset_value += 1.0;

	if (color_range_value < 0.0) color_range_value = 0.0;

	if (max_calc_value < 20) max_calc_value = 20;
	*/
}

void force_redraw(int value) {
	glutPostRedisplay();

	if (julia_state.menu_open) {
		julia_menu.update_state();
	}
	else {
		julia_gamepad.update_state();
	}

	if (julia_state.explore) {
		julia_explorer.explore(ASPECT);
	}

	glutTimerFunc(20, force_redraw, 0);
}

void reshape(int width, int height) {
	SCREEN_W = width;
	SCREEN_H = height;
	ASPECT = (float)SCREEN_W / (float)SCREEN_H;

	drawing.reshape(SCREEN_W, SCREEN_H);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));

	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("GLJULIA");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutMouseFunc(on_click);
	glutMotionFunc(on_move);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glLineWidth(2.0);

	load_shader("julia.vert", "julia.frag", &program_fp64);
	load_shader("julia_float.vert", "julia_float.frag", &program_fp32);

	julia_state = JuliaState(program_fp64, program_fp32);
	julia_explorer = JuliaExplorer(&julia_state);
	julia_menu = JuliaMenu(pad, &julia_state, &drawing, &julia_explorer, program_fp64, program_fp32);

	glutMainLoop();

	return 0;
}
