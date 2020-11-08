#version 440
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vertexPos[];

out vec3 fvertexPos;
out vec3 normal;
out vec3 fcolor;
vec3 colour = vec3(0.0f, 0.8f, 1.0f);

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec3 calcNormals()
{
	vec3 u = vertexPos[1] - vertexPos[0];
	vec3 v = vertexPos[2] - vertexPos[0];
	vec3 normal = cross(u, v);
	vec3 nNormal = normalize(normal);
	return nNormal;
}

void main() {

for(int i = 0; i < 3; i++)
{
	vec3 normals = calcNormals();
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_in[i].gl_Position;

	normal = mat3(transpose(inverse(worldMatrix))) * normals;

	fvertexPos = vec3(worldMatrix * vec4(vertexPos[i], 1.0f));
	fcolor = colour;
	EmitVertex();
}
EndPrimitive();
}