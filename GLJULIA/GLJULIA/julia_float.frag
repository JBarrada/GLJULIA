#version 400

uniform vec2 c;
uniform vec2 center;
uniform float zoom;
uniform float aspect_ratio;
uniform vec2 super_off;
uniform int max_calc = 500;
uniform int julia_power = 2;

uniform float color_range = 1.0;
uniform float color_offset = 0.0;

//varying vec2 p;
varying vec2 vp;

const float threshold = 2.0;
const int calc_mod = 200;


vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


vec2 complex_mult(vec2 a, vec2 b) {
	return vec2((a.x*b.x) - (a.y*b.y), (a.x*b.y) + (a.y*b.x));
}

vec2 complex_pow(vec2 a, int power) {
	vec2 b = a;
	for (int i = 1; i < power; i++) {
		b = complex_mult(b, a);
	}
	return b;
}

vec2 complex_div(vec2 a, vec2 b) {
	return vec2( ((a.x*b.x+a.y*b.y) / (b.x*b.x+b.y*b.y)), ((a.y*b.x-a.x*b.y) / (b.x*b.x+b.y*b.y)) );
}

vec2 complex_ln(vec2 a) {
	return vec2(log(length(a)), atan(a.y, a.x));
}

vec2 complex_exp(vec2 a) {
	float ex = exp(a.x);
	return vec2(ex*cos(a.y), ex*sin(a.y));
}

vec3 color(int count) {
	if (count == -1) {
		return vec3(0);
    } else {
		float brightness = float(mod(count, calc_mod)) / calc_mod;

		brightness *= color_range;
		brightness += color_offset;
		brightness -= floor(brightness);

		vec3 color = hsv2rgb(vec3(brightness, 1.0, 1.0));
		return color;
    }
}

int calculate(vec2 z) {
	int count = -1;
    for (int i = 0; i < max_calc; i++) {
        vec2 nz = (complex_pow(z, julia_power) + c);
        
		if (dot(nz, nz) > threshold) {
            count = i;
            break;
        }
        z = nz;
    }
	return count;
}

void main() {
	vec2 p = (vp * zoom + center) / aspect_ratio * vec2(aspect_ratio, 1.0);
	//vec2 p_off = p  + ((super_off * zoom ) / aspect_ratio * vec2(aspect_ratio, 1.0));
    vec2 z = p.xy;
	vec2 dz = vec2(1, 0);
    int count = -1;
    
    for (int i = 0; i < max_calc; i++) {
        vec2 nz = (complex_pow(z, julia_power) + c);
		vec2 ndz = julia_power * complex_mult(complex_pow(z, julia_power - 1), dz);
        
        z = nz;
		dz = ndz;

		if (dot(nz, nz) > threshold) {
            count = i;
            break;
        }
    }

	if (count == -1) {
	gl_FragColor = vec4(0, 0, 0, 1);
	} else {
	float de = length(z)*log(length(z)) / length(dz);
	//gl_FragColor = vec4(vec3(de), 1);
	
	de *= color_range;
	de += color_offset;
	de -= floor(de);

	vec3 color = hsv2rgb(vec3(de, 1.0, 1.0));
	gl_FragColor = vec4(color, 1);
	}
	/*
    if (count == -1) {
        gl_FragColor = vec4(0, 0, 0, 1);
    } else {
		float brightness = float(mod(count, calc_mod)) / calc_mod;

		brightness *= color_range;
		brightness += color_offset;
		brightness -= floor(brightness);

		vec3 color = hsv2rgb(vec3(brightness, 1.0, 1.0));
		gl_FragColor = vec4(color, 1);
    }
	*/
}
