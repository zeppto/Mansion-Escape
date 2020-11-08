#version 440
in vec2 texCoord;

uniform ivec3 mapDSN;
uniform vec2 moveUV;
uniform sampler2D diffuseTexture;

// this is the final pixel colour
out vec4 fragment_color;


void main () 
{

	vec4 coloring;
	if(mapDSN[0] == 1)
		coloring = texture(diffuseTexture, vec2(texCoord.x + moveUV.x , (1-texCoord.y) + moveUV.y));
	else
		coloring = vec4(0.5,0.5,0.5,1);

	fragment_color = coloring;
}