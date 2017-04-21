#include <iostream>
#include <cmath>
#include <glew.h>
#include <glut.h>

#include "shader_utils.h"
#include "gamepad_input.h"

#define VIBRATE_STRENGTH 0.2

#define HUD_PAD 10
#define HUD_SIZE 60
#define HUD_I_SIZE 6

GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

static GLuint program;
static GLuint program_float;
static GLuint buffer;

int SCREEN_W = 600;
int SCREEN_H = 300;
double SCREEN_W_2 = (SCREEN_W / 2.0);
double SCREEN_H_2 = (SCREEN_H / 2.0);

bool float_mode = false;

double zoom_value = 7;
double center_point[] = {0.0, 0.0};

float c_value[] = { 0.279, 0.0 };

int julia_power_value = 2;

double exp_zoom = exp(zoom_value * log(1.1));

bool move_drag = false;
int move_drag_x;
int move_drag_y;

bool c_value_drag = 0;
int c_value_drag_x;
int c_value_drag_y;

float color_offset_value = 0.0;
float color_range_value = 1.0;

int max_calc_value = 500;

bool move_cruise = false;
bool move_lock = false;
double move_cruise_speed_y = 0.0;
double move_cruise_speed_x = 0.0;

bool c_value_cruise = false;
bool c_value_lock = false;
double c_value_cruise_speed_y = 0.0;
double c_value_cruise_speed_x = 0.0;

bool zoom_cruise = false;
bool zoom_lock = false;
double zoom_cruise_speed = 0.0;

bool shoulder_down = false;
bool rstick_down = false;
bool dpad_down = false;
bool back_down = false;

int rstick_mode = 0;

bool color_cruise = false;
bool color_lock = false;
double color_cruise_speed_y = 0.0;
double color_cruise_speed_x = 0.0;

float expo(float);


float vertex[] = {
	-1.0,  1.0, 0.0,
	 1.0,  1.0, 0.0,
	-1.0, -1.0, 0.0,
	 1.0, -1.0, 0.0,
};

GLuint vtxindex[] = {0,1,2,3};

int make_resources() {
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	return 1;
}


void draw_box(float x, float y, float width, float height) {
	float off_x = (x / ((float)SCREEN_W)) - 1.0;
	float off_y = (y / ((float)SCREEN_H)) - 1.0;
	width = (width / ((float)SCREEN_W_2));
	height = (height / ((float)SCREEN_H_2));

	glBegin(GL_QUADS);
	glVertex2f(off_x+0, off_y+0);
	glVertex2f(off_x+width, off_y+0);
	glVertex2f(off_x+width, off_y+height);
	glVertex2f(off_x+0, off_y+height);
	glEnd();
}

void draw_stick(float x, float y, float stickx, float sticky, float cruisex, float cruisey, bool cruise, float r, float g, float b) {
	draw_box(x, y, HUD_SIZE, HUD_SIZE);

	glColor4f(1, 1, 1, 0.5);
	float stickx_hud = (stickx + 1.0) * HUD_SIZE;
	float sticky_hud = (sticky + 1.0) * HUD_SIZE;
	draw_box(x + stickx_hud - HUD_I_SIZE, y + sticky_hud - HUD_I_SIZE, HUD_I_SIZE, HUD_I_SIZE);

	if (cruise) {
		glColor4f(r, g, b, 0.5);
		stickx_hud = (cruisex + 1.0) * HUD_SIZE;
		sticky_hud = (cruisey + 1.0) * HUD_SIZE;
		draw_box(x + stickx_hud - HUD_I_SIZE, y + sticky_hud - HUD_I_SIZE, HUD_I_SIZE, HUD_I_SIZE);
	}
}

void draw_pad() {
	if (pad->is_connected()) {
		glUseProgram(0);
		
		float lstick_x = expo(pad->State.lstick_x);
		float lstick_y = expo(pad->State.lstick_y);
		
		float rstick_x = expo(pad->State.rstick_x);
		float rstick_y = expo(pad->State.rstick_y);
		
		glColor4f(1, 1, 1, 0.25);
		draw_stick(HUD_PAD, HUD_PAD, lstick_x, lstick_y, move_cruise_speed_x, move_cruise_speed_y, move_cruise, 0, 1, 0);
		
		/*
		if (rstick_mode == 0) 
			glColor4f(1, 1, 1, 0.25);
		else 
			glColor4f(1, 1, 1, 0.1);
		*/
		glColor4f(1,1,1, (rstick_mode == 0)? 0.25 : 0.1);
		draw_stick(HUD_PAD*2 + HUD_SIZE*2, HUD_PAD, rstick_x, rstick_y, c_value_cruise_speed_x, c_value_cruise_speed_y, c_value_cruise, 1, 0, 0);
		/*
		if (rstick_mode == 1)
			glColor4f(1, 1, 1, 0.25);
		else 
			glColor4f(1, 1, 1, 0.1);
		*/
		glColor4f(1,1,1, (rstick_mode == 1)? 0.25 : 0.1);
		draw_stick(HUD_PAD * 3 + HUD_SIZE * 4, HUD_PAD, rstick_x, rstick_y, color_cruise_speed_x, color_cruise_speed_y, color_cruise, 0, 0, 1);


	}
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (!float_mode) {
		glUseProgram(program);

		GLint center = glGetUniformLocation(program, "center");
		glUniform2dv(center, 1, center_point);
		GLint zoom = glGetUniformLocation(program, "zoom");
		glUniform1d(zoom, exp_zoom);
	} else {
		glUseProgram(program_float);

		GLint center = glGetUniformLocation(program, "center");
		float center_point_float[] = {(float)center_point[0], (float)center_point[1]};
		glUniform2fv(center, 1, center_point_float);
		GLint zoom = glGetUniformLocation(program, "zoom");
		glUniform1d(zoom, (float)exp_zoom);
	}
	
	GLint c_val = glGetUniformLocation(program, "c");
	glUniform2fv(c_val, 1, c_value);
	GLint aspect_ratio = glGetUniformLocation(program, "aspect_ratio");
	glUniform1f(aspect_ratio, SCREEN_W / (float)SCREEN_H);

	GLint color_offset = glGetUniformLocation(program, "color_offset");
	glUniform1f(color_offset, color_offset_value);
	GLint color_range = glGetUniformLocation(program, "color_range");
	glUniform1f(color_range, color_range_value);

	GLint max_calc = glGetUniformLocation(program, "max_calc");
	glUniform1i(max_calc, max_calc_value);

	GLint julia_power = glGetUniformLocation(program, "julia_power");
	glUniform1i(julia_power, julia_power_value);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, static_cast<void*>(vtxindex));

	glDisableClientState(GL_VERTEX_ARRAY);

	draw_pad();

	glutSwapBuffers();
}

void on_click(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			move_drag = true;
			move_drag_x = x;
			move_drag_y = y;
		} else if (state == GLUT_UP) {
			move_drag = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			c_value_drag = true;
			c_value_drag_x = x;
			c_value_drag_y = y;
		} else if (state == GLUT_UP) {
			c_value_drag = false;
		}
	}

	if (button == 3) {
		zoom_value -= 0.5;
		exp_zoom = exp(zoom_value * log(1.1));
	}
	if (button == 4) {
		zoom_value += 0.5;
		exp_zoom = exp(zoom_value * log(1.1));
	}
}

void on_move(int x, int y) {
	if (move_drag) {
		double fx = (x - move_drag_x) / SCREEN_W_2 * exp_zoom;
		double fy = (y - move_drag_y) / SCREEN_H_2 * exp_zoom);

		move_drag_x = x;
		move_drag_y = y;

		center_point[0] += -fx;
		center_point[1] += fy;
	}

	if (c_value_drag) {
		double fx = ((x - c_value_drag_x) / SCREEN_W_2 * exp_zoom) / 500.0;
		double fy = ((y - c_value_drag_y) / SCREEN_H_2 * exp_zoom) / 500.0;

		c_value_drag_x = x;
		c_value_drag_y = y;

		c_value[0] += -fx;
		c_value[1] += fy;
	}
}

void keyboard_down(unsigned char key, int x, int y) {
	
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
}

float expo(float val) {
	if (val < 0)
		return -1.0 * pow(val, 2);
	return pow(val, 3);
}

void gamepad_update() {
	if (pad->is_connected()) {
		pad->update();

		float lstick_x = expo(pad->State.lstick_x);
		float lstick_y = expo(pad->State.lstick_y);

		if (move_lock && fabs(pad->State.lstick_x) < 0.05 && fabs(pad->State.lstick_y) < 0.05) {
			move_lock = false;
			pad->vibrate(0.0, 0.0);
		}
		if (!move_lock) {
			center_point[0] += (lstick_x * 2.0) / SCREEN_W_2 * exp_zoom;
			center_point[1] += (lstick_y * 2.0) / SCREEN_H_2 * exp_zoom;
		}
		if (move_cruise || pad->State.buttons[GamePad_Button_A] == true) {
			if (move_cruise) {
				center_point[0] += (move_cruise_speed_x * 2.0) / SCREEN_W_2 * exp_zoom;
				center_point[1] += (move_cruise_speed_y * 2.0) / SCREEN_H_2 * exp_zoom;
			}
			if (!move_cruise && pad->State.buttons[GamePad_Button_A] == true) {
				if (fabs(pad->State.lstick_x) > 0.02 && fabs(pad->State.lstick_y) > 0.02) {
					move_cruise = true;
					move_cruise_speed_x += lstick_x;
					move_cruise_speed_y += lstick_y;
					move_lock = true;

					pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (move_cruise && !move_lock && pad->State.buttons[GamePad_Button_A] == true) {
				move_cruise = false;
				if (fabs(pad->State.lstick_x) < 0.05 && fabs(pad->State.lstick_y) < 0.05) {
					move_cruise_speed_x = 0.0;
					move_cruise_speed_y = 0.0;
				}
			}
		}


		float rstick_x = expo(pad->State.rstick_x);
		float rstick_y = expo(pad->State.rstick_y);

		if (c_value_lock && fabs(pad->State.rstick_x) < 0.05 && fabs(pad->State.rstick_y) < 0.05) {
			c_value_lock = false;
			pad->vibrate(0.0, 0.0);
		}
		if (color_lock && fabs(pad->State.rstick_x) < 0.05 && fabs(pad->State.rstick_y) < 0.05) {
			color_lock = false;
			pad->vibrate(0.0, 0.0);
		}
		if (rstick_mode == 0) {
			if (!c_value_lock) {
				c_value[0] += (rstick_x / 40.0) / SCREEN_W_2 * exp_zoom;
				c_value[1] += (rstick_y / 40.0) / SCREEN_H_2 * exp_zoom;
			}
		}
		else if (rstick_mode == 1) {
			if (!color_lock) {
				color_offset_value += rstick_x / 100.0;
				color_range_value += rstick_y / 100.0;
			}
		}

		if (c_value_cruise || pad->State.buttons[GamePad_Button_B] == true) {
			if (c_value_cruise) {
				c_value[0] += (c_value_cruise_speed_x / 40.0) / SCREEN_W_2 * exp_zoom;
				c_value[1] += (c_value_cruise_speed_y / 40.0) / SCREEN_H_2 * exp_zoom;
			}
			if (!c_value_cruise && pad->State.buttons[GamePad_Button_B] == true) {
				if (fabs(pad->State.rstick_x) > 0.02 && fabs(pad->State.rstick_y) > 0.02) {
					c_value_cruise = true;
					c_value_cruise_speed_x += rstick_x;
					c_value_cruise_speed_y += rstick_y;
					c_value_lock = true;

					pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (c_value_cruise && !c_value_lock && pad->State.buttons[GamePad_Button_B] == true) {
				c_value_cruise = false;
				if (fabs(pad->State.rstick_x) < 0.05 && fabs(pad->State.rstick_y) < 0.05) {
					c_value_cruise_speed_x = 0.0;
					c_value_cruise_speed_y = 0.0;
				}
			}
		}

		if (color_cruise || pad->State.buttons[GamePad_Button_X] == true) {
			if (color_cruise) {
				color_offset_value += color_cruise_speed_x / 100.0;
				color_range_value += color_cruise_speed_y / 100.0;
			}
			if (!color_cruise && pad->State.buttons[GamePad_Button_X] == true) {
				if (fabs(pad->State.rstick_x) > 0.02 && fabs(pad->State.rstick_y) > 0.02) {
					color_cruise = true;
					color_cruise_speed_x += rstick_x;
					color_cruise_speed_y += rstick_y;
					color_lock = true;

					pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (color_cruise && !color_lock && pad->State.buttons[GamePad_Button_X] == true) {
				color_cruise = false;
				if (fabs(pad->State.rstick_x) < 0.05 && fabs(pad->State.rstick_y) < 0.05) {
					color_cruise_speed_x = 0.0;
					color_cruise_speed_y = 0.0;
				}
			}
		}

		if (color_offset_value > 1.0) color_offset_value -= 1.0;
		if (color_offset_value < 0.0) color_offset_value += 1.0;

		if (color_range_value < 0.0) color_range_value = 0.0;

		float trigger_sum = expo(pad->State.lt - pad->State.rt);

		if (zoom_lock && fabs(pad->State.lt) < 0.05 && fabs(pad->State.rt) < 0.05) {
			zoom_lock = false;
			pad->vibrate(0.0, 0.0);
		}
		if (!zoom_lock) {
			zoom_value += trigger_sum / 5.0;
			if (trigger_sum != 0.0) exp_zoom = exp(zoom_value * log(1.1));
		}
		if (zoom_cruise || pad->State.buttons[GamePad_Button_Y] == true) {
			if (zoom_cruise) {
				zoom_value += zoom_cruise_speed / 5.0;
				if (zoom_cruise_speed != 0.0) exp_zoom = exp(zoom_value * log(1.1));
			}
			if (!zoom_cruise && pad->State.buttons[GamePad_Button_Y] == true) {
				zoom_cruise = true;
				zoom_cruise_speed += trigger_sum;
				zoom_lock = true;

				pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
			}
			else if (zoom_cruise && !zoom_lock && pad->State.buttons[GamePad_Button_Y] == true) {
				zoom_cruise = false;
				if (fabs(pad->State.lt) < 0.05 && fabs(pad->State.rt) < 0.05)
					zoom_cruise_speed = 0.0;
			}
		}

		if (pad->State.buttons[GamePad_Button_LEFT_SHOULDER] || pad->State.buttons[GamePad_Button_RIGHT_SHOULDER]) {
			if (!shoulder_down)
				color_offset_value += rand() / (float)RAND_MAX;
			shoulder_down = true;
		}
		else {
			shoulder_down = false;
		}


		if (pad->State.buttons[GamePad_Button_RIGHT_THUMB]) {
			if (!rstick_down) {
				if (((rstick_mode + 1) % 2) == 1) {
					pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			rstick_down = true;
		}
		else {
			if (rstick_down) {
				rstick_mode = (rstick_mode + 1) % 2;
				pad->vibrate(0.0, 0.0);
			}
			rstick_down = false;
		}

		// DPAD
		if (pad->State.buttons[0] || pad->State.buttons[1] || pad->State.buttons[2] || pad->State.buttons[3]) {
			if (!dpad_down) {
				if (pad->State.buttons[GamePad_Button_DPAD_UP])
					julia_power_value += 1;
				if (pad->State.buttons[GamePad_Button_DPAD_DOWN])
					julia_power_value -= 1;

				if (julia_power_value < 1) julia_power_value = 1;

				if (pad->State.buttons[GamePad_Button_DPAD_LEFT])
					max_calc_value -= 10;
				if (pad->State.buttons[GamePad_Button_DPAD_RIGHT])
					max_calc_value += 10;

				if (max_calc_value < 20) max_calc_value = 20;

				pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
			}
			dpad_down = true;
		}
		else {
			if (dpad_down) {
				pad->vibrate(0.0, 0.0);
			}
			dpad_down = false;
		}

		if (pad->State.buttons[GamePad_Button_BACK]) {
			if (!back_down) {
				/*
				zoom_value = 7.0;
				exp_zoom = exp(zoom_value * log(1.1));
				center_point[0] = 0.0;
				center_point[1] = 0.0;
				//c_value[0] = 0.0;
				//c_value[1] = 0.0;
				*/
				float_mode = !float_mode;
				pad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
			}
			back_down = true;
		}
		else {
			if (back_down) {
				pad->vibrate(0.0, 0.0);
			}
			back_down = false;
		}
	}
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	gamepad_update();
	glutTimerFunc(20, force_redraw, 0);
}

void reshape(int width, int height) {
	SCREEN_W = width;
	SCREEN_H = height;
	SCREEN_W_2 = (SCREEN_W / 2.0);
	SCREEN_H_2 = (SCREEN_H / 2.0);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	// try rgba and remove double
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("GLJULIA");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
		return 1;
	}

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutMouseFunc(on_click);
	glutMotionFunc(on_move);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	load_shader("julia.vert", "julia.frag", &program);
	load_shader("julia_float.vert", "julia_float.frag", &program_float);
	
	if (!make_resources()) {
		std::cout << "Failed to prepare resources" << std::endl;
		return 1;
	}

	glutMainLoop();

	return 0;
}
