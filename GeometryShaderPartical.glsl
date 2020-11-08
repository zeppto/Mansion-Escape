#version 440
layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

out vec4 color;
out vec2 texCoord;

uniform vec4 particalColor;
uniform mat4 particalWorld;
uniform mat4 particelRotation;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
vec3 camraRight;
vec3 camraUp;

void createVertex(vec3 offset, vec2 texurCoord)
{
	//sending to fragmantShader
	texCoord = texurCoord;
	color = particalColor;

	//creat a plain to aline the particel to fase the camra
	camraRight = vec3(view[0][0],view[1][0],view[2][0]);
	camraUp = -vec3(view[0][1],view[1][1],view[2][1]);

	//rotates the particel then aline it to the camra
	vec4 vertexPos = vec4(offset, 1.0f); 
	vertexPos = particelRotation * vertexPos;
	vertexPos = vec4(vertexPos.x*camraRight + vertexPos.y*camraUp, 1.0f);

	gl_Position = projection * view * world * particalWorld * vertexPos;

	EmitVertex();
}

void main() {
	//creat all cornos of a partical
	createVertex(vec3(1.0f, 1.0f, 0f), vec2( 1f, 1f));
	createVertex(vec3(1.0f, -1.0f, 0f), vec2( 1f, 0f));
	createVertex(vec3(-1.0f, 1.0f, 0f), vec2( 0f, 1f));
	createVertex(vec3(-1.0f, -1.0f, 0f), vec2( 0f, 0f));

    EndPrimitive();

}