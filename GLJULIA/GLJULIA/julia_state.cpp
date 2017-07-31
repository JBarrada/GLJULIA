#include "julia_state.h"

void State1D::set(double x) {
	this->x = x;
	this->x_exp = exp(this->x * log(1.1));
}
void State1D::add(double x) {
	this->x += x;
	this->x_exp = exp(this->x * log(1.1));
}

JuliaState::JuliaState() {
	max_calc = 500;
	power = 2;

	zoom = { exp(7.0 * log(1.1)), 7, 0, false};

	center = { 0,0,0,0,false };
	c = { 0.279,0,0,0,false };

	color = { 0,1,0,0,false };

	fp64 = false;

	explore = false;

	menu_open = false;
	hud_open = true;
}

JuliaState::JuliaState(GLuint program_fp64, GLuint program_fp32) {
	this->program_fp64 = program_fp64;
	this->program_fp32 = program_fp32;

	max_calc = 500;
	power = 2;

	zoom = { exp(7.0 * log(1.1)), 7, 0, false };

	center = { 0,0,0,0,false };
	c = { 0.279,0,0,0,false };

	color = { 0,1,0,0,false };

	fp64 = false;

	explore = false;

	menu_open = false;
	hud_open = true;
}

void JuliaState::update_program_variables(double SCREEN_W, double SCREEN_H, double ASPECT) {
	if (fp64) {
		glUseProgram(program_fp64);

		GLint prog_center = glGetUniformLocation(program_fp64, "center");
		GLdouble center_gl[] = { this->center.x, this->center.y };
		glUniform2dv(prog_center, 1, center_gl);
		GLint prog_zoom = glGetUniformLocation(program_fp64, "zoom");
		glUniform1d(prog_zoom, this->zoom.x_exp);

		GLint prog_c = glGetUniformLocation(program_fp64, "c");
		GLfloat c_gl[] = { this->c.x, this->c.y };
		glUniform2fv(prog_c, 1, c_gl);
		GLint prog_aspect_ratio = glGetUniformLocation(program_fp64, "aspect_ratio");
		glUniform1f(prog_aspect_ratio, ASPECT);

		GLint prog_color_offset = glGetUniformLocation(program_fp64, "color_offset");
		glUniform1f(prog_color_offset, color.x);
		GLint prog_color_range = glGetUniformLocation(program_fp64, "color_range");
		glUniform1f(prog_color_range, color.y);

		GLint prog_max_calc = glGetUniformLocation(program_fp64, "max_calc");
		glUniform1i(prog_max_calc, max_calc);

		GLint prog_julia_power = glGetUniformLocation(program_fp64, "julia_power");
		glUniform1i(prog_julia_power, power);

	}
	else {
		glUseProgram(program_fp32);
		GLint prog_center = glGetUniformLocation(program_fp32, "center");
		GLfloat center_gl[] = { this->center.x, this->center.y };
		glUniform2fv(prog_center, 1, center_gl);

		GLint prog_zoom = glGetUniformLocation(program_fp32, "zoom");
		glUniform1f(prog_zoom, this->zoom.x_exp);

		GLint prog_c_val = glGetUniformLocation(program_fp32, "c");
		GLfloat c_gl[] = { this->c.x, this->c.y };
		glUniform2fv(prog_c_val, 1, c_gl);
		GLint prog_aspect_ratio = glGetUniformLocation(program_fp32, "aspect_ratio");
		glUniform1f(prog_aspect_ratio, ASPECT);
		GLint prog_screen_off = glGetUniformLocation(program_fp32, "super_off");
		GLfloat screen_off_gl[] = { (1 / SCREEN_W), (1 / SCREEN_H) };
		glUniform2fv(prog_screen_off, 1, screen_off_gl);

		GLint prog_color_offset = glGetUniformLocation(program_fp32, "color_offset");
		glUniform1f(prog_color_offset, color.x);
		GLint prog_color_range = glGetUniformLocation(program_fp32, "color_range");
		glUniform1f(prog_color_range, color.y);

		GLint prog_max_calc = glGetUniformLocation(program_fp32, "max_calc");
		glUniform1i(prog_max_calc, max_calc);

		GLint prog_julia_power = glGetUniformLocation(program_fp32, "julia_power");
		glUniform1i(prog_julia_power, power);
	}
}