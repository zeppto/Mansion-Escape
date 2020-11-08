#version 440
layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPos[];
in vec2 vertexUV[];

out vec2 texCoord;

uniform mat4 world;

void main() 
{
	for(int i = 0; i < 3; i++)
	{	
		texCoord = vertexUV[i];
		gl_Position =  world * gl_in[i].gl_Position;
		//gl_Position =  gl_in[i].gl_Position;

		EmitVertex();	
		
	}
	EndPrimitive();

}
