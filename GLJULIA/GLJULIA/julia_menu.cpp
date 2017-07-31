#include "julia_menu.h"

JuliaMenu::JuliaMenu() {
	selected_item = 0;
}

JuliaMenu::JuliaMenu(GamePadXbox *gamepad, JuliaState *julia_state, DrawingTools *drawing, JuliaExplorer *julia_explorer, GLuint program_fp64, GLuint program_fp32) {
	this->gamepad = gamepad;
	this->julia_state = julia_state;
	this->drawing = drawing;

	this->julia_explorer = julia_explorer;

	this->program_fp64 = program_fp64;
	this->program_fp32 = program_fp32;

	selected_item = 0;
}

void JuliaMenu::draw_menu() {
	glUseProgram(0);

	int MENU_WIDTH = MENU_ITEM_W;
	int MENU_HEIGHT = MENU_ITEMS * MENU_ITEM_H;

	double x_offset = (drawing->SCREEN_W / 2.0) - (MENU_WIDTH / 2.0);
	double y_offset = (drawing->SCREEN_H / 2.0) - (MENU_HEIGHT / 2.0);

	glColor4f(1, 1, 1, 0.6);
	drawing->rectangle_filled(x_offset, y_offset, MENU_WIDTH, MENU_HEIGHT);

	glColor4f(1, 1, 1, 0.7);
	drawing->rectangle_filled(x_offset, y_offset + (selected_item) * MENU_ITEM_H, MENU_ITEM_W, MENU_ITEM_H);

	glColor4f(0, 0, 0, 0.7);
	drawing->text(2 + x_offset, (MENU_ITEM_H - 18)/2.0 + y_offset + (0) * MENU_ITEM_H, GLUT_BITMAP_HELVETICA_18, "save");
	drawing->text(2 + x_offset, (MENU_ITEM_H - 18)/2.0 + y_offset + (1) * MENU_ITEM_H, GLUT_BITMAP_HELVETICA_18, "load");
	drawing->text(2 + x_offset, (MENU_ITEM_H - 18)/2.0 + y_offset + (2) * MENU_ITEM_H, GLUT_BITMAP_HELVETICA_18, (julia_state->explore)? "exploring" :"explore");
	drawing->text(2 + x_offset, (MENU_ITEM_H - 18)/2.0 + y_offset + (3) * MENU_ITEM_H, GLUT_BITMAP_HELVETICA_18, (julia_state->fp64)? "fp64" : "fp32");
	// save
	// load
	// auto
	// fp64
}

void JuliaMenu::update_state() {
	if (gamepad->is_connected()) {
		gamepad->update();

		GamePadState gamepad_state = gamepad->State;

		// DPAD
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_UP] && gamepad_state.buttons[GamePad_Button_DPAD_UP]) {
			selected_item = (1 + selected_item) % MENU_ITEMS;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_DOWN] && gamepad_state.buttons[GamePad_Button_DPAD_DOWN]) {
			selected_item = (selected_item + (MENU_ITEMS - 1)) % MENU_ITEMS;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_LEFT] && gamepad_state.buttons[GamePad_Button_DPAD_LEFT]) {
			//if (julia_state->max_calc > 20) julia_state->max_calc -= 10;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_DPAD_RIGHT] && gamepad_state.buttons[GamePad_Button_DPAD_RIGHT]) {
			//julia_state->max_calc += 10;
		}

		if (gamepad_state.buttons_last[GamePad_Button_A] && !gamepad_state.buttons[GamePad_Button_A]) {
			switch (selected_item) {
				case 0: break;
				case 1: break;
				case 2:
					julia_state->explore = !julia_state->explore;
					if (julia_state->explore) {
						julia_explorer->init_explore();
						julia_state->menu_open = false;
					}
					break;
				case 3: 
					julia_state->fp64 = !julia_state->fp64;
					break;
			}
		}

		// START // B
		if (!gamepad_state.buttons_last[GamePad_Button_START] && gamepad_state.buttons[GamePad_Button_START]) {
			julia_state->menu_open = !julia_state->menu_open;
		}
		if (!gamepad_state.buttons_last[GamePad_Button_B] && gamepad_state.buttons[GamePad_Button_B]) {
			julia_state->menu_open = !julia_state->menu_open;
		}
	}
}