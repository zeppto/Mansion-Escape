#version 440

in vec3 fvertexPos;
in vec3 normal;
in vec3 fcolor;

out vec4 fragment_color;

layout (location = 42) uniform vec3 lightPos;
layout (location = 43) uniform vec3 camPos;

void main () {

	//diffuse
	float ambientStr = 0.1f;
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = ambientStr * lightColour;  
	vec3 nNormal = normalize(normal);
	vec3 lightDir = normalize(fvertexPos - lightPos);
	
	float diff = max(dot(lightDir, nNormal), 0);
	vec3 diffuse = diff * lightColour;

	//specular
	float specularStr = 1.0f;
	vec3 viewDir = normalize(camPos - fvertexPos);
	vec3 reflectDir = reflect(lightDir, normalize(nNormal));
	float specular = pow(max(dot(viewDir, reflectDir), 0), 32);
	vec3 specularFinal = specularStr * specular * lightColour * fcolor;

	fragment_color = vec4((ambient + diffuse + specularFinal) * fcolor, 0.4f);
	
}