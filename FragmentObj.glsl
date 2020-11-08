#version 440
// these values are interpolated at the rasteriser
//in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec3 worldPos;
in mat3 TBN;
in vec4 posLigtSpace;

uniform vec3 lightPos;
uniform vec3 camPos;
uniform vec3 color;
uniform vec3 ambientColor;
uniform float transparency;
uniform sampler2D myTexture;
uniform sampler2D normalTexture;
uniform sampler2D shadowMap;
uniform sampler2D glowTexture;
uniform vec4 specColor_Textur;

uniform float haveNormals;
uniform float haveGlow;

//-----Glow-----//
uniform float glowIntensity;
vec4 final_color;
float Weight[6] = float[] (0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925);
float PixOffset[20] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0);



// this is the final pixel colour
out vec4 fragment_color;

float shadowClaculation ( vec4 fPosLightSpace)
{
	//needs for perspektiv prjojektion
	vec3 clipCoord = fPosLightSpace.xyz / fPosLightSpace.w;
	//from 1, -1 to 1, 0
	clipCoord = clipCoord * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, clipCoord.xy).r; 
	vec3 lightDir = normalize(lightPos - worldPos);
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

void main () {

	vec3 nNormal;

	//diffuse
	//float ambientLight = 0.1f;
	vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
	//vec3 ambient = 0.1f * ambientColor * lightColour; 
	vec3 ambient = ambientColor; 
	if(haveNormals == 1)
	{
		// obtain normal from normal map in range [0,1]
		nNormal = texture(normalTexture, texCoord).rgb;
		// transform normal vector to range [-1,1]
		nNormal = normalize(nNormal * 2.0 - 1.0); // this normal is in tangent space
		nNormal = normalize(TBN * nNormal);
	}
	else 
		nNormal = normalize(normal);

	vec3 lightDir = normalize(lightPos - worldPos);
	//vec3 lightDir = TBN * normalize(lightPos - worldPos);
	
	float diff = max(dot(lightDir, nNormal), 0);
	vec3 diffuse = diff * lightColour;

	//specular
	vec3 specularFinal = vec3(0,0,0);
	if(specColor_Textur.x != -1)
	{
		float specularStr = 3.0f;
		vec3 viewDir = normalize(camPos - worldPos);
		//vec3 viewDir = TBN * normalize(camPos - worldPos);
		vec3 reflectDir = reflect(-lightDir, normalize(nNormal));
		float specular = pow(max(dot(viewDir, reflectDir), 0), 32);
		specularFinal = specularStr * specular * lightColour * specColor_Textur.xyz;
	}
	
	float shadow = shadowClaculation(posLigtSpace);

	//total colour
	vec2 uv = vec2(texCoord.x, 1-texCoord.y);
	vec3 temp = color + vec3(1,1,1);


	if(haveGlow == 1)
	{
		//-----Save both textures as vec4-----//
		vec4 BrightFragment_color = texture(glowTexture, uv);
		vec4 textureFragment_color = texture(myTexture, uv);

		//-----Calculate blur-----//
		vec2 tex_offset = 1.0 / textureSize(glowTexture, 0); // gets size of single texel
		vec3 result = (texture(glowTexture, uv).rgb * 0.1f) * Weight[0]; // current fragment's contribution
	
		for ( int i = 1; i < 6; i++ )
		{		
			result += texture(glowTexture, uv + vec2(tex_offset.x * i, 0.0)).rgb * Weight[i];
			result += texture(glowTexture, uv - vec2(tex_offset.x * i, 0.0)).rgb * Weight[i];
		}
		for ( int i = 1; i < 6; i++ )
		{
			result += texture(glowTexture, uv + vec2(0.0, tex_offset.y * i)).rgb * Weight[i];
			result += texture(glowTexture, uv - vec2(0.0, tex_offset.y * i)).rgb * Weight[i];
		}
		

		//-----apply the result multiplied with glow intensity-----//
		BrightFragment_color = vec4(result,1.0) * glowIntensity;
		
		//-----Combine both textures and add light calculations-----//
		vec4 lighting = vec4((ambient + (1.0 - shadow) *(diffuse + specularFinal)), 1.0f);
		final_color = BrightFragment_color += textureFragment_color * lighting;
	
	}

	if(specColor_Textur.w == 1)
	{
		if(haveGlow == 1)
			fragment_color = final_color;
		else
			fragment_color = texture(myTexture, vec2(texCoord.x, 1-texCoord.y)) * vec4(temp * (ambient + (1.0 - shadow) *(diffuse + specularFinal)), transparency);
	}
	else
	{
		fragment_color = vec4(color * (ambient + (1.0 - shadow) *(diffuse + specularFinal)), transparency);
	}
	//fragment_color = vec4(nNormal, 1.0);

}