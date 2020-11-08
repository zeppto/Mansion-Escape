#version 440
in vec3 normal;
in vec2 texCoord;
in vec3 worldPos;
in vec4 myWeights;
in vec4 myBonesID;


uniform vec3 lightPos;
uniform vec3 camPos;
//uniform vec3 color;
//uniform vec3 ambientColor;
//uniform float transparency;
//uniform sampler2D myTexture;
//uniform vec4 specColor_Textur;



// this is the final pixel colour
out vec4 fragment_color;


void main () {

	vec3 nNormal;

	//diffuse
	float ambientLight = 0.1f;
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = 0.5f * lightColour; 
	//vec3 ambient = ambientColor; 
	nNormal = normalize(normal);

	vec3 lightDir = normalize(lightPos - worldPos);
	
	float diff = max(dot(lightDir, nNormal), 0);
	vec3 diffuse = diff * lightColour;

	//specular
	vec3 specularFinal = vec3(0,0,0);
	/*if(specColor_Textur.x != -1)
	{
		float specularStr = 3.0f;
		vec3 viewDir = normalize(camPos - worldPos);
		vec3 reflectDir = reflect(-lightDir, normalize(nNormal));
		float specular = pow(max(dot(viewDir, reflectDir), 0), 32);
		specularFinal = specularStr * specular * lightColour * specColor_Textur.xyz;
	}*/
	

	//total colour
	vec2 uv = vec2(texCoord.x, 1-texCoord.y);
	//vec3 temp = color + vec3(1,1,1);


	/*if(specColor_Textur.w == 1)
	{
		fragment_color = texture(myTexture, vec2(texCoord.x, 1-texCoord.y)) * vec4(temp * (ambient + diffuse + specularFinal), transparency);
	}
	else
	{
		fragment_color = vec4(color * (ambient + (1.0 - shadow) *(diffuse + specularFinal)), transparency);
	}*/
	vec4 weightsColor = vec4(myWeights.xyz,1.0);
	fragment_color = weightsColor;//vec4(vec3(0,0,0.5f) * (ambient + diffuse + specularFinal), 1.0f);
	//if(finalValue <= 1.001f && finalValue >= 0.9999f)
	//{
	//fragment_color = vec4(1.0, 0.0, 0.0, 1.0);
	//}
}