#include <iostream>
#include <cmath>
#include <glew.h>
#include <glut.h>

#include "shader_utils.h"

#define SCREEN_W 1200
#define SCREEN_H 600

// GLSL shader program object
static GLuint program;
// Vertex array object
static GLuint buffer;

double zoom_value = 7;
float center_point_const[] = {0.0, 0.0};

float c_value[] = { 0.279, 0.0 };

static int is_dragging = 0;
static int drag_start_x;
static int drag_start_y;

static int is_dragging_right = 0;
static int drag_start_right_x;
static int drag_start_right_y;

float vertex[] = {
	// x, y, z
	-1.0,  1.0, 0.0,
	 1.0,  1.0, 0.0,
	-1.0, -1.0, 0.0,
	 1.0, -1.0, 0.0,
};

// Vertex array index
GLuint vtxindex[] = {
	0,1,2,3
};

int make_resources() {
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	return 1;
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	GLint c_val = glGetUniformLocation(program, "c");
	glUniform2fv(c_val, 1, c_value);

	// vertex shader vars
	GLint center = glGetUniformLocation(program, "center");
	glUniform2fv(center, 1, center_point_const);
	GLint zoom = glGetUniformLocation(program, "zoom");
	glUniform1f(zoom, exp(zoom_value * log(1.1)));
	GLint aspect_ratio = glGetUniformLocation(program, "aspect_ratio");
	glUniform1f(aspect_ratio, SCREEN_W / (double)SCREEN_H);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, static_cast<void*>(vtxindex));

	glDisableClientState(GL_VERTEX_ARRAY);

	glutSwapBuffers();
}

void on_click(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			is_dragging = 1;
			drag_start_x = x;
			drag_start_y = y;
		}
		else  if (state == GLUT_UP) {
			is_dragging = 0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			is_dragging_right = 1;
			drag_start_right_x = x;
			drag_start_right_y = y;
		}
		else  if (state == GLUT_UP) {
			is_dragging_right = 0;
		}
	}

	if (button == 3) {
		zoom_value -= 0.5;
	}
	if (button == 4) {
		zoom_value += 0.5;
	}
}

void on_move(int x, int y) {
	if (is_dragging) {
		float fx = (x - drag_start_x) / (double)(SCREEN_W/2.0) * exp(zoom_value * log(1.1));
		float fy = (y - drag_start_y) / (double)(SCREEN_H/2.0) * exp(zoom_value * log(1.1));

		drag_start_x = x;
		drag_start_y = y;

		center_point_const[0] += -fx;
		center_point_const[1] += fy;
	}

	if (is_dragging_right) {
		float fx = (x - drag_start_right_x) / (double)(SCREEN_W / 2.0) * exp(zoom_value * log(1.1));
		float fy = (y - drag_start_right_y) / (double)(SCREEN_H / 2.0) * exp(zoom_value * log(1.1));

		fx /= 500.0;
		fy /= 500.0;

		drag_start_right_x = x;
		drag_start_right_y = y;

		c_value[0] += -fx;
		c_value[1] += fy;
	}
}

void force_redraw(int value) {
	glutPostRedisplay();
	glutTimerFunc(20, force_redraw, 0);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("GLJULIA");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
		return 1;
	}

	// register display function
	glutDisplayFunc(render);
	// register timer event handler
	glutTimerFunc(20, force_redraw, 0);
	// register special key down event handler
	//glutKeyboardFunc(keyboard);
	// register mouse click event handler
	glutMouseFunc(on_click);
	// register mouse move event handler
	glutMotionFunc(on_move);

	// load and compile vertex/fragment shader programs
	load_shader("julia_float.vert", "julia_float.frag", &program);
	
	if (!make_resources()) {
		std::cout << "Failed to prepare resources" << std::endl;
		return 1;
	}

	// start main loop
	glutMainLoop();

	return 0;
}
