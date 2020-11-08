#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_UV;

out vec2 vertexUV;
out vec3 vertexPos;

void main() {

	vertexUV = vertex_UV;
	vertexPos = vertex_position;
	gl_Position = vec4(vertex_position, 1.0f);
	
}
