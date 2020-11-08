#version 440
layout(location = 0) in vec3 vertex_position;

out vec2 vertexUV;
out vec3 vertexPos;

uniform float waterTime;

float PI = 3.1415;
float low = 0.01;

void main() {

	float part1 = sin(2.0 * PI * waterTime + (vertex_position.x * 16.0)) * low;
	float part2 = sin(2.0 * PI * waterTime + (vertex_position.z * vertex_position.x * 8.0)) * low;
	float height = part1 + part2;

	vertexPos = vertex_position;
	gl_Position = vec4(vertex_position.x, vertex_position.y -(height * 3), vertex_position.z, 1.0f);
	
}
