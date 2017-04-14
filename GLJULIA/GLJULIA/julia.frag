#version 400

#extension GL_ARB_gpu_shader_fp64 : enable

uniform dvec2 center;
uniform float zoom;
uniform float aspect_ratio;
varying vec2 vp;

dvec2 p;

uniform vec2 c;
uniform int max_calc = 200;

const double threshold = 2.0;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	p = (vp * zoom + center) / aspect_ratio * dvec2(aspect_ratio, 1.0);


    dvec2 z = dvec2(p.x, p.y);
    int count = -1;
    
    for (int i = 0; i < max_calc; i++) {
        dvec2 nz = dvec2(0, 0);
        nz.x = z.x * z.x  - z.y * z.y + c.x;
        nz.y = 2.0 * z.x * z.y + c.y;
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
