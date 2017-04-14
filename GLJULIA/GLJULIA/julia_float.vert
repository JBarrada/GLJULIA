#version 400

#extension GL_ARB_gpu_shader_fp64 : enable

uniform vec2 center;
uniform float zoom;
uniform float aspect_ratio;

varying vec2 p;

in vec3 VertexPosition;

void main(void)
{
	gl_Position = vec4(VertexPosition, 1.0);
    p = (vec2(VertexPosition) * zoom + center) / aspect_ratio * vec2(aspect_ratio, 1.0);
}