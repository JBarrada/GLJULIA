#version 400

#extension GL_ARB_gpu_shader_fp64 : enable

varying vec2 vp;

in vec3 VertexPosition;

void main(void) {
	gl_Position = vec4(VertexPosition, 1.0);
	vp = vec2(VertexPosition);
}