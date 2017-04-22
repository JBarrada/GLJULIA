#version 400

#extension GL_ARB_gpu_shader_fp64 : enable

uniform vec2 c;
uniform dvec2 center;
uniform double zoom;
uniform float aspect_ratio;
uniform int max_calc = 500;
uniform int julia_power = 2;

uniform float color_range = 1.0;
uniform float color_offset = 0.0;

varying vec2 vp;

const double threshold = 2.0;
const int calc_mod = 200;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


dvec2 complex_mult(dvec2 a, dvec2 b) {
	return dvec2((a.x*b.x) - (a.y*b.y), (a.x*b.y) + (a.y*b.x));
}

dvec2 complex_pow(dvec2 a, int power) {
	dvec2 b = a;
	for (int i = 1; i < power; i++) {
		b = complex_mult(b, a);
	}
	return b;
}

dvec2 complex_div(dvec2 a, dvec2 b) {
	return dvec2( ((a.x*b.x+a.y*b.y) / (b.x*b.x+b.y*b.y)), ((a.y*b.x-a.x*b.y) / (b.x*b.x+b.y*b.y)) );
}

void main() {
	dvec2 p = (vp * zoom + center) / aspect_ratio * dvec2(aspect_ratio, 1.0);

    dvec2 z = p.xy;
    int count = -1;
    
    for (int i = 0; i < max_calc; i++) {
        dvec2 nz = (complex_pow(z, julia_power) + c);
        
		// length(nz)
		if (dot(nz, nz) > threshold) {
            count = i;
            break;
        }
        z = nz;
    }
    
    if (count == -1) {
        gl_FragColor = vec4(0, 0, 0, 1);
    } else {
		float brightness = float(mod(count, calc_mod)) / calc_mod;
		
		brightness *= color_range;
		brightness += color_offset;

		brightness -= floor(brightness);

		vec3 color = hsv2rgb(vec3(brightness, 1.0, 1.0));
		gl_FragColor = vec4(color, 1.0);
    }
}
