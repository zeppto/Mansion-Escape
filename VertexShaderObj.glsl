#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_UV;

out vec2 vertexUV; //texCoords
out vec3 vertexPos;
out vec3 vertexNormal;

out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

uniform mat4 world; //model
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 camPos; //viewPos

void main()
{
	vertexPos = vertex_position;
	vertexUV = vertex_UV;
	vertexNormal = vertex_normal;

//	vec3 v0;
//
//
//	vertexPos = vec3(world * vec4(vertex_position, 1.0)); // fragPos
//	vertexUV = vertex_UV;
//
//
//
//	mat3 normalMatrix = transpose(inverse(mat3(world)));
//	vec3 T = normalize(normalMatrix * tangent);
//	vec3 N = normalize(normalMatrix * vertex_normal);
//	T = normalize(T - dot(T,N) * N);
//	vec3 B = cross(N, T);
//
//	mat3 TBN = transpose(mat3(T,B,N));
//	tangentLightPos = TBN * lightPos;
//	tangentViewPos = TBN * viewPos;
//	tangentFragPos = TBN * vertexPos;
//
//	gl_Position = projection * view * world * vec4(vertex_position, 1.0);


	gl_Position = vec4(vertex_position, 1);
}

