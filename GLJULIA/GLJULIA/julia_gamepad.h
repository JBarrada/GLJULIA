#ifndef JULIA_GAMEPAD_H
#define JULIA_GAMEPAD_H

#include <math.h>

#include "gamepad_input.h"
#include "julia_state.h"
#include "drawing_tools.h"

class JuliaGamepad {
private:
	double expo(double);
public:
	JuliaState *julia_state;
	GamePadXbox *gamepad;
	DrawingTools *drawing;

	JuliaGamepad();
	JuliaGamepad(GamePadXbox *gamepad, JuliaState *julia_state, DrawingTools *drawing);

	double VIBRATE_STRENGTH;

	double STICK2D_PAD;
	double STICK2D_SIZE;
	double STICK2D_I_SIZE;

	int rstick_mode;

	bool lock_zoom;
	bool lock_center;
	bool lock_c;
	bool lock_color;

	void update_state();

	void draw_state2d(double x, double y, State2D state, float r, float g, float b);
	void draw_hud();
};

#endif
