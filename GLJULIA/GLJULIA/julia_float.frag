#version 400

#extension GL_ARB_gpu_shader_fp64 : enable

varying vec2 p;

uniform vec2 c;
uniform int max_calc = 500;
const float threshold = 2.0;

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

void main() {
    vec2 z = vec2(p.x, p.y);
    int count = -1;
    
    for (int i = 0; i < max_calc; i++) {
        vec2 nz = vec2(0, 0);
		nz = (complex_pow(z, 2) + c);
        if (length(nz) > threshold) {
            count = i;
            break;
        }
        z = nz;
    }
    
    if (count == -1) {
        gl_FragColor = vec4(0, 0, 0, 0);
    } else {
		float brightness = float(mod(count, 100)) / 100.0;
		//gl_FragColor = vec4(brightness, brightness, brightness, 1.0);
		vec3 color = hsv2rgb(vec3(brightness, 1.0, 1.0));
		gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
    }
}
