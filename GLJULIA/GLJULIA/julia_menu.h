#ifndef JULIA_MENU_H
#define JULIA_MENU_H

#include <math.h>
#include <glew.h>
#include <glut.h>

#include "gamepad_input.h"
#include "julia_state.h"
#include "julia_explorer.h"
#include "drawing_tools.h"

class JuliaMenu {
	GLuint program_fp64;
	GLuint program_fp32;
public:
	JuliaState *julia_state;
	GamePadXbox *gamepad;
	DrawingTools *drawing;
	JuliaExplorer *julia_explorer;

	int selected_item;

	int MENU_ITEM_W = 100;
	int MENU_ITEM_H = 30;
	int MENU_ITEMS = 4;

	JuliaMenu();
	JuliaMenu(GamePadXbox *gamepad, JuliaState *julia_state, DrawingTools *drawing, JuliaExplorer *julia_explorer, GLuint program_fp64, GLuint program_fp32);

	void draw_menu();
	void update_state();

};

#endif