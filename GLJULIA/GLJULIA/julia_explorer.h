#ifndef JULIA_EXPLORER
#define JULIA_EXPLORER

#include <math.h>
#include <glew.h>
#include <glut.h>
#include <vector>

#include "julia_state.h"

using namespace std;

struct complex {
	double r;
	double i;
};

struct sample {
	complex screen;
	complex julia;
	int calc;
};

class JuliaExplorer {
public:
	JuliaState *julia_state;

	sample target;

	double target_complexity = 0.5;

	int SAMPLES_W = 40;
	int SAMPLES_H = 40;

	double last_zoom = 0;

	JuliaExplorer();
	JuliaExplorer(JuliaState *julia_state);

	int calculate(complex z);
	void init_explore();
	void explore(double ASPECT);

};

#endif
