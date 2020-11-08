#version 440

in vec2 vertexCoord;
in vec3 fvertexPos;
in vec3 normal;

out vec4 fragment_color;

layout (location = 23) uniform vec3 lightPos;
layout (location = 25) uniform vec3 camPos;

uniform sampler2D blendTexture;
uniform sampler2D cobblestoneTexture;
uniform sampler2D grassTexture;
uniform sampler2D dirtTexture;

void main () {


	//diffuse
	float ambientLight = 0.1f;
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = ambientLight * lightColour; 
	vec3 nNormal = normalize(normal);
	vec3 lightDir = normalize(fvertexPos - lightPos);
	
	float diff = max(dot(lightDir, nNormal), 0);
	vec3 diffuse = diff * lightColour;
	
	//uv coords
	vec2 uv = vec2(vertexCoord.x, vertexCoord.y);

	//blendTexture
	vec4 blendMapColour = texture(blendTexture, uv);
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec4 finalGrass = texture(grassTexture, uv) * backTextureAmount;
	vec4 finalStone = texture(cobblestoneTexture, uv) * blendMapColour.r;
	vec4 finalDirt = texture(dirtTexture, uv) * blendMapColour.b;

	vec4 totalColour = finalGrass + finalStone + finalDirt;

	fragment_color = totalColour * vec4((ambient + diffuse), 1.0f);
	
	
	
}