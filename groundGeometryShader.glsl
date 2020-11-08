#version 440
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in vec3 vertexPos[];
in vec2 vertexUV[];

out vec2 vertexCoord;
out vec3 fvertexPos;
out vec3 normal;

layout (location = 25) uniform vec3 camPos;


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
	vec3 worldPos = vec3(worldMatrix * vec4(vertexPos[i], 1.0f));
	vec3 camDir = normalize(worldPos - camPos);
	vec3 normals = calcNormals();

	normal = mat3(transpose(inverse(worldMatrix))) * normals;
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_in[i].gl_Position;
	fvertexPos = vec3(worldMatrix * vec4(vertexPos[i], 1.0f));
	vertexCoord = vertexUV[i];

	if(dot(camDir, normal) >= 0)
	{
		EmitVertex();
	}

}
	EndPrimitive();
}