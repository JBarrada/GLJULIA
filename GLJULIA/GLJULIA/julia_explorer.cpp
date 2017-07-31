#include "julia_explorer.h"

JuliaExplorer::JuliaExplorer() {
}
JuliaExplorer::JuliaExplorer(JuliaState *julia_state) {
	this->julia_state = julia_state; 
}

complex complex_mult(complex a, complex b) {
	return { (a.r*b.r) - (a.i*b.i), (a.r*b.i) + (a.i*b.r) };
}
complex complex_pow(complex a, int power) {
	complex b = a;
	for (int i = 1; i < power; i++) {
		b = complex_mult(b, a);
	}
	return b;
}

int JuliaExplorer::calculate(complex z) {
	int count = -1;
	for (int i = 0; i < julia_state->max_calc; i++) {
		complex nz = complex_pow(z, julia_state->power);
		nz.r += julia_state->c.x;
		nz.i += julia_state->c.y;

		if ((nz.r*nz.r) + (nz.i*nz.i) > 2.0) {
			count = i;
			return count;
		}
		z = nz;
	}

	return count;
}

void JuliaExplorer::init_explore() {
	last_zoom = julia_state->zoom.x;

	julia_state->zoom.cruise = true;
	julia_state->zoom.dx = -0.1;

	julia_state->center.cruise = true;
//	julia_state->color.cruise = true;
}

void JuliaExplorer::explore(double ASPECT) {
	vector<sample> samples;
	int sample_index = 0;

	int max_calc = 0;
	complex current = { 0,0 };
	for (int y = 0; y < SAMPLES_H; y++) {
		for (int x = 0; x < SAMPLES_W; x++) {
			sample current;
			current.screen.r = (x / (double)SAMPLES_W) * 2.0 - 1.0;
			current.screen.i = (y / (double)SAMPLES_H) * 2.0 - 1.0;

			current.julia.r = (current.screen.r * julia_state->zoom.x_exp + julia_state->center.x);
			current.julia.i = (current.screen.i * julia_state->zoom.x_exp + julia_state->center.y) / ASPECT;

			current.calc = calculate(current.julia);
			
			if (current.calc > max_calc) {
				max_calc = current.calc;
			}

			samples.push_back(current);
		}
	}

	//glBegin(GL_QUADS);
	double max_score = 0;
	double target_calc = max_calc * target_complexity;
	sample temp_best = { 0,0,0,0,0 };
	for (int i = 0; i < (SAMPLES_W*SAMPLES_H); i++) {
		double dx = samples[i].screen.r - target.screen.r;
		double dy = samples[i].screen.i - target.screen.i;

		double samples_adjusted = samples[i].calc;
		if (samples_adjusted > target_calc) {
			samples_adjusted = target_calc - (samples_adjusted - target_calc);
		}

		double score = samples_adjusted *pow(1.0 - fmin(((dx * dx) + (dy * dy)) / 2.0, 1.0), 15.0);
		if (score > max_score) {
			max_score = score;
			temp_best = samples[i];
		}
		/*
		double brightness = (score / (double)julia_state->max_calc);		
		glColor4f(brightness, brightness, brightness, 1);
		glVertex2f(off_x, off_y);
		glVertex2f(off_x + (2 / (double)sample_w), off_y);
		glVertex2f(off_x + (2 / (double)sample_w), off_y + (2 / (double)sample_h));
		glVertex2f(off_x, off_y + (2 / (double)sample_h));
		*/
	}
	/*
	glColor4f(1, 0, 0, 1);
	glVertex2f(best_off_x, best_off_y);
	glVertex2f(best_off_x + (2 / (double)sample_w), best_off_y);
	glVertex2f(best_off_x + (2 / (double)sample_w), best_off_y + (2 / (double)sample_h));
	glVertex2f(best_off_x, best_off_y + (2 / (double)sample_h));
	glEnd();
	*/
	
	target = temp_best;

	double delta_zoom = fabs(last_zoom - julia_state->zoom.x) * 10.0;
	last_zoom = julia_state->zoom.x;

	if (julia_state->zoom.cruise && julia_state->zoom.dx > 0 && julia_state->zoom.x > 19.0) {
		julia_state->zoom.dx *= -1;
	}

	if (julia_state->zoom.cruise && julia_state->zoom.dx < 0 && julia_state->zoom.x < -131.0 && !julia_state->fp64) {
		julia_state->zoom.dx *= -1;
	}
	if (julia_state->zoom.cruise && julia_state->zoom.dx < 0 && julia_state->zoom.x < -342.0 && julia_state->fp64) {
		julia_state->zoom.dx *= -1;
	}

	julia_state->center.dx = target.screen.r * delta_zoom;
	julia_state->center.dy = target.screen.i * delta_zoom;

	//julia_state->color.dy = max_calc / 200.0;
}