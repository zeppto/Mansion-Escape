#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_UV;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

out vec2 vertexUV; //texCoords
out vec3 vertexPos;


void main()
{
	vertexPos = vertex_position;
	vertexUV = vertex_UV;

	gl_Position = vec4(vertex_position, 1);
}
