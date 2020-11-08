#version 440
in vec3 normal;
in vec2 texCoord;
in vec3 worldPos;
in mat3 TBN;
in vec4 posLigtSpace;
//in vec4 myWeights;
//in vec4 myBonesID;


uniform vec3 lightPos[2];


uniform vec3 camPos;
uniform ivec3 mapDSN;
uniform sampler2D diffuseTexture;
uniform sampler2D myNormalTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//uniform vec3 color;
//uniform vec3 ambientColor;
//uniform float transparency;
//uniform sampler2D myTexture;
//uniform vec4 specColor_Textur;


// this is the final pixel colour
out vec4 fragment_color;

float shadowClaculation ( vec4 fPosLightSpace, vec3 lightDir)
{
	//needs for perspektiv prjojektion
	vec3 clipCoord = fPosLightSpace.xyz / fPosLightSpace.w;
	//from 1, -1 to 1, 0
	clipCoord = clipCoord * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, clipCoord.xy).r; 
	//funkar inte men kan hjälpa när det gör
	float bias = max(0.005 *(1.0 - dot(normal, lightDir)), 0.005);
	bias = 0.007;
	float currentDepth = clipCoord.z;  

	float shadow = 0.0f;
	vec2 shadowTextureSize = 1.0 / textureSize(shadowMap, 0);
	for ( int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float depthTile = texture(shadowMap, clipCoord.xy + vec2(x,y) * shadowTextureSize).r; 
			if(currentDepth - bias > depthTile)
				shadow += 1.0f;
			else
				shadow += 0.0f;
		}
	}
	//dela med 9 för det är 9 rutor som samplas
	shadow /= 9.0f;
    return shadow;
}


void main () 
{
	vec3 nNormal;

	//diffuse
	//float ambientLight = 0.1f;
	vec3 lightColour[2] = {vec3(1.0f, 0.5f, 0.0f), vec3(1.0f, 0.5f, 0.0f)};
	
	if(mapDSN[2] == 1)
	{
		// obtain normal from normal map in range [0,1]
		nNormal = texture(myNormalTexture, vec2(texCoord.x, 1-texCoord.y)).rgb;
		// transform normal vector to range [-1,1]
		nNormal = normalize(nNormal * 2.0 - 1.0); // this normal is in tangent space
		nNormal = normalize(TBN * nNormal);
	}
	else 
		nNormal = normalize(normal);

	//vec3 lightPos[3] = {lightPos1,lightPos2, lightPos3};
	vec3 lightDir[2];
	vec3 attenuation[2] = {vec3(1.0f, 0.05f, 0.1f), vec3(1.0f, 0.05f, 0.1f)};

	for(int i = 0; i < 2; i++)
	{
		lightDir[i] = (lightPos[i] - worldPos);
	}

	//vec3 lightDir = TBN * normalize(lightPos - worldPos);
	vec3 specularFinal = vec3(0,0,0);
	vec3 ambient, diffuse;

	for(int i = 0; i < 2; i++)
	{
		float distance = length(lightDir[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		float diff = max(dot(lightDir[i], nNormal), 0);
		diffuse = diffuse + (diff * lightColour[i])/attFactor;
		ambient = ambient + 0.01f * lightColour[i]; 
		//specular
		
		if(mapDSN[1] == 1)
		{
			float specularStr = 2.0f;
			vec3 viewDir = normalize(camPos - worldPos);
			//vec3 viewDir = TBN * normalize(camPos - worldPos);
			vec3 reflectDir = reflect(normalize(lightDir[i]), normalize(nNormal));
			float specular = pow(max(dot(viewDir, reflectDir), 0), 16);
			//inte riktigt rätt
			specularFinal = specularFinal + ((specularStr * specular * lightColour[i]) * texture(specularTexture, vec2(texCoord.x, 1-texCoord.y)).rgb);
		}
	}

	//float shadow = shadowClaculation(posLigtSpace, lightDir[0]);
	float shadow = 0;

	vec4 coloring;
	if(mapDSN[0] == 1)
		coloring = texture(diffuseTexture, vec2(texCoord.x, 1-texCoord.y));
	else
		coloring = vec4(0.5,0.5,0.5,1);

	 if(coloring.a < 0.1)
	 {
		discard;
	 }

	if(mapDSN[1] == 1)
		coloring *= vec4((ambient + 1 *(diffuse + specularFinal)),1); //+ (1.0 - shadow)
	else
		coloring *= vec4((ambient + 1 * diffuse),1); //+ (1.0 - shadow);

	fragment_color = coloring;
}