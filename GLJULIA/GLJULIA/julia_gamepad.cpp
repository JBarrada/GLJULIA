#include "julia_gamepad.h"

JuliaGamepad::JuliaGamepad() {
	VIBRATE_STRENGTH = 0.2;

	STICK2D_PAD = 10;
	STICK2D_SIZE = 80;
	STICK2D_I_SIZE = 6;

	rstick_mode = 0;
}

JuliaGamepad::JuliaGamepad(GamePadXbox *gamepad, JuliaState *julia_state, DrawingTools *drawing) {
	this->gamepad = gamepad;
	this->julia_state = julia_state;
	this->drawing = drawing;

	VIBRATE_STRENGTH = 0.2;

	STICK2D_PAD = 10;
	STICK2D_SIZE = 80;
	STICK2D_I_SIZE = 6;

	rstick_mode = 0;
}

double JuliaGamepad::expo(double value) {
	if (value < 0)
		return -1.0 * pow(value, 2);
	return pow(value, 3);
}

void JuliaGamepad::update_state() {
	if (gamepad->is_connected()) {
		gamepad->update();

		GamePadState gamepad_state = gamepad->State;

		const float TRAVEL_CONST = 128.0;

		float lstick_x = expo(gamepad_state.lstick_x);
		float lstick_y = expo(gamepad_state.lstick_y);

		float rstick_x = expo(gamepad_state.rstick_x);
		float rstick_y = expo(gamepad_state.rstick_y);

		float trigger_sum = expo(gamepad_state.lt - gamepad_state.rt);

		bool lstick_active = fabs(gamepad_state.lstick_x) > 0.02 || fabs(gamepad_state.lstick_y) > 0.02;
		bool rstick_active = fabs(gamepad_state.rstick_x) > 0.02 || fabs(gamepad_state.rstick_y) > 0.02;
		bool triggers_active = fabs(gamepad_state.lt) > 0.02 || fabs(gamepad_state.rt) > 0.02;

		// CENTER
		if (lock_center && !lstick_active) {
			lock_center = false;
			gamepad->vibrate(0.0, 0.0);
		}
		if (!lock_center) {
			julia_state->center.x += (lstick_x * 2.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
			julia_state->center.y += (lstick_y * 2.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
		}
		if (julia_state->center.cruise || gamepad_state.buttons[GamePad_Button_A] == true) {
			if (julia_state->center.cruise) {
				julia_state->center.x += (julia_state->center.dx * 2.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
				julia_state->center.y += (julia_state->center.dy * 2.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
			}
			if (!julia_state->center.cruise && gamepad_state.buttons[GamePad_Button_A] == true) {
				if (lstick_active) {
					julia_state->center.cruise = true;
					julia_state->center.dx += lstick_x;
					julia_state->center.dy += lstick_y;
					lock_center = true;

					gamepad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (julia_state->center.cruise && !lock_center && gamepad_state.buttons[GamePad_Button_A] == true) {
				julia_state->center.cruise = false;
				if (!lstick_active) {
					julia_state->center.dx = 0.0;
					julia_state->center.dy = 0.0;
				}
			}
		}

		// C
		if (lock_c && !rstick_active) {
			lock_c = false;
			gamepad->vibrate(0.0, 0.0);
		}
		if (rstick_mode == 0) {
			if (!lock_c) {
				julia_state->c.x += (rstick_x / 40.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
				julia_state->c.y += (rstick_y / 40.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
			}
		}
		if (julia_state->c.cruise || gamepad_state.buttons[GamePad_Button_B] == true) {
			if (julia_state->c.cruise) {
				julia_state->c.x += (julia_state->c.dx / 40.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
				julia_state->c.y += (julia_state->c.dy / 40.0) / TRAVEL_CONST * julia_state->zoom.x_exp;
			}
			if (!julia_state->c.cruise && gamepad_state.buttons[GamePad_Button_B] == true) {
				if (rstick_active) {
					julia_state->c.cruise = true;
					julia_state->c.dx += rstick_x;
					julia_state->c.dy += rstick_y;
					lock_c = true;

					gamepad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (julia_state->c.cruise && !lock_c && gamepad_state.buttons[GamePad_Button_B] == true) {
				julia_state->c.cruise = false;
				if (!rstick_active) {
					julia_state->c.dx = 0.0;
					julia_state->c.dy = 0.0;
				}
			}
		}

		// COLOR
		if (lock_color && !rstick_active) {
			lock_color = false;
			gamepad->vibrate(0.0, 0.0);
		}
		if (rstick_mode == 1) {
			if (!lock_color) {
				julia_state->color.x += rstick_x / 100.0;
				julia_state->color.y += rstick_y / 100.0;
			}
		}
		if (julia_state->color.cruise || gamepad_state.buttons[GamePad_Button_X] == true) {
			if (julia_state->color.cruise) {
				julia_state->color.x += julia_state->color.dx / 100.0;
				julia_state->color.y += julia_state->color.dy / 100.0;
			}
			if (!julia_state->color.cruise && gamepad_state.buttons[GamePad_Button_X] == true) {
				if (rstick_active) {
					julia_state->color.cruise = true;
					julia_state->color.dx += rstick_x;
					julia_state->color.dy += rstick_y;
					lock_color = true;

					gamepad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (julia_state->color.cruise && !lock_color && gamepad_state.buttons[GamePad_Button_X] == true) {
				julia_state->color.cruise = false;
				if (!rstick_active) {
					julia_state->color.dx = 0.0;
					julia_state->color.dy = 0.0;
				}
			}
		}

		if (julia_state->color.x > 1.0) julia_state->color.x -= 1.0;
		if (julia_state->color.x < 0.0) julia_state->color.x += 1.0;
		if (julia_state->color.y < 0.0) julia_state->color.y = 0.0;

		// ZOOM
		if (lock_zoom && !triggers_active) {
			lock_zoom = false;
			gamepad->vibrate(0.0, 0.0);
		}
		if (!lock_zoom) {
			julia_state->zoom.add(trigger_sum / 5.0);
		}
		if (julia_state->zoom.cruise || gamepad_state.buttons[GamePad_Button_Y] == true) {
			if (julia_state->zoom.cruise) {
				julia_state->zoom.add(julia_state->zoom.dx / 5.0);
			}
			if (!julia_state->zoom.cruise && gamepad_state.buttons[GamePad_Button_Y] == true) {
				if (fabs(gamepad_state.lt) > 0.02 || fabs(gamepad_state.rt) > 0.02) {
					julia_state->zoom.cruise = true;
					julia_state->zoom.dx += trigger_sum;
					lock_zoom = true;

					gamepad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
				}
			}
			else if (julia_state->zoom.cruise && !lock_zoom && gamepad_state.buttons[GamePad_Button_Y] == true) {
				julia_state->zoom.cruise = false;
				if (fabs(gamepad_state.lt) < 0.05 && fabs(gamepad_state.rt) < 0.05)
					julia_state->zoom.dx = 0.0;
			}
		}

		// SHOULDERS
		if ((!gamepad_state.buttons_last[GamePad_Button_LEFT_SHOULDER] && gamepad_state.buttons[GamePad_Button_LEFT_SHOULDER]) ||
			(!gamepad_state.buttons_last[GamePad_Button_RIGHT_SHOULDER] && gamepad_state.buttons[GamePad_Button_RIGHT_SHOULDER])) {
			julia_state->color.x += rand() / (float)RAND_MAX;
		}

		// RSTICK CLICK
		if (!gamepad_state.buttons_last[GamePad_Button_RIGHT_THUMB] && gamepad_state.buttons[GamePad_Button_RIGHT_THUMB]) {
			if (((rstick_mode + 1) % 2) == 1) {
				gamepad->vibrate(VIBRATE_STRENGTH, VIBRATE_STRENGTH);
			}
		}
		else if (gamepad_state.buttons_last[GamePad_Button_RIGHT_THUMB] && !gamepad_state.buttons[GamePad_Button_RIGHT_THUMB]) {
			rstick_mode = (rstick_mode + 1) % 2;
			gamepad->vibrate(0.0, 0.0);
		}

		// DPAD
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_UP] && gamepad_state.buttons[GamePad_Button_DPAD_UP]) {
			julia_state->power += 1;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_DOWN] && gamepad_state.buttons[GamePad_Button_DPAD_DOWN]) {
			if (julia_state->power > 1) julia_state->power -= 1;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_LEFT] && gamepad_state.buttons[GamePad_Button_DPAD_LEFT]) {
			if (julia_state->max_calc > 20) julia_state->max_calc -= 10;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_RIGHT] && gamepad_state.buttons[GamePad_Button_DPAD_RIGHT]) {
			julia_state->max_calc += 10;
		}

		// BACK
		if (!gamepad_state.buttons_last[GamePad_Button_BACK] && gamepad_state.buttons[GamePad_Button_BACK]) {
			julia_state->fp64 = !julia_state->fp64;
		}

		// START
		if (!gamepad_state.buttons_last[GamePad_Button_START] && gamepad_state.buttons[GamePad_Button_START]) {
			julia_state->menu_open = !julia_state->menu_open;
		}
	}
}

void JuliaGamepad::draw_state2d(double x, double y, State2D state, float r, float g, float b) {
	drawing->rectangle_filled(x, y, STICK2D_SIZE, STICK2D_SIZE);

	glColor4f(1, 1, 1, 0.5);
	float stickx_hud = (state.x + 1.0) / 2.0 * STICK2D_SIZE;
	float sticky_hud = (state.y + 1.0) / 2.0 * STICK2D_SIZE;
	drawing->circle_filled(x + stickx_hud, y + sticky_hud, STICK2D_I_SIZE);

	if (state.cruise) {
		glColor4f(r, g, b, 0.8);
		stickx_hud = (state.dx + 1.0) / 2.0 * STICK2D_SIZE;
		sticky_hud = (state.dy + 1.0) / 2.0 * STICK2D_SIZE;
		drawing->circle_hollow(x + stickx_hud, y + sticky_hud, STICK2D_I_SIZE);
	}
}

void JuliaGamepad::draw_hud() {
	if (gamepad->is_connected()) {
		glUseProgram(0);

		GamePadState gamepad_state = gamepad->State;

		float lstick_x = expo(gamepad_state.lstick_x);
		float lstick_y = expo(gamepad_state.lstick_y);

		float rstick_x = expo(gamepad_state.rstick_x);
		float rstick_y = expo(gamepad_state.rstick_y);

		float trigger_sum = expo(gamepad_state.lt - gamepad_state.rt);

		State2D lstick = { lstick_x, lstick_y, julia_state->center.dx,julia_state->center.dy, julia_state->center.cruise };
		State2D rstick_mode0 = { rstick_x, rstick_y, julia_state->c.dx,julia_state->c.dy, julia_state->c.cruise };
		State2D rstick_mode1 = { rstick_x, rstick_y, julia_state->color.dx,julia_state->color.dy, julia_state->color.cruise };

		glColor4f(1, 1, 1, 0.4);
		draw_state2d(STICK2D_PAD, STICK2D_PAD, lstick, 0, 1, 0);

		glColor4f(1, 1, 1, (rstick_mode == 0) ? 0.4 : 0.2);
		draw_state2d(STICK2D_PAD * 2 + STICK2D_SIZE, STICK2D_PAD, rstick_mode0, 1, 0, 0);

		glColor4f(1, 1, 1, (rstick_mode == 1) ? 0.4 : 0.2);
		draw_state2d(STICK2D_PAD * 3 + STICK2D_SIZE * 2, STICK2D_PAD, rstick_mode1, 0, 0, 1);

		
		glColor4f(1, 1, 1, 0.4);
		drawing->rectangle_filled(STICK2D_PAD, STICK2D_PAD * 2 + STICK2D_SIZE, STICK2D_PAD * 2 + STICK2D_SIZE * 3, STICK2D_I_SIZE*2);
		glColor4f(1, 1, 1, 0.5);
		drawing->rectangle_filled(STICK2D_PAD + (STICK2D_PAD * 2 + STICK2D_SIZE * 3)*(trigger_sum + 1.0)/2.0 - STICK2D_I_SIZE, STICK2D_PAD * 2 + STICK2D_SIZE, STICK2D_I_SIZE*2, STICK2D_I_SIZE*2);
		if (julia_state->zoom.cruise) {
			glColor4f(1, 1, 0, 0.8);
			drawing->rectangle_hollow(STICK2D_PAD + (STICK2D_PAD * 2 + STICK2D_SIZE * 3)*(julia_state->zoom.dx + 1.0)/2.0 - STICK2D_I_SIZE, STICK2D_PAD * 2 + STICK2D_SIZE, STICK2D_I_SIZE*2, STICK2D_I_SIZE*2);
		}
	}
}