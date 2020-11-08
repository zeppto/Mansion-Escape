#version 440
layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPos[];
in vec2 vertexUV[];
in vec3 vertexNormal[];
in vec3 vertexTangent[];
in vec3 vertexBitangent[];

//in vec4 weights[];
//in ivec4 bonesID[];

out vec3 normal;
out vec3 worldPos;
out vec2 texCoord;
out mat3 TBN;
out vec4 posLigtSpace;
//out vec4 myWeights;
//out ivec4 myBonesID;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 jointMatrixes[64];

uniform mat4 lightSpace;
uniform vec3 camPos;

void main() 
{
	for(int i = 0; i < 3; i++)
	{
		normal = vertexNormal[i];
		texCoord = vertexUV[i];
		
		normal = mat3(transpose(inverse(world))) * normal;

		//Shortcuts for vertices
		vec3 v0 = vertexPos[0];
        vec3 v1 = vertexPos[1];
        vec3 v2 = vertexPos[2];

		// Shortcuts for UVs
        vec2 uv0 = vertexUV[0];
        vec2 uv1 = vertexUV[1];
        vec2 uv2 = vertexUV[2];

		// Edges of the triangle : position delta
        vec3 deltaPos1 = vertexPos[1]-vertexPos[0];
        vec3 deltaPos2 = vertexPos[2]-vertexPos[0];

		// UV delta
        vec2 deltaUV1 = vertexUV[1]-vertexUV[0];
        vec2 deltaUV2 = vertexUV[2]-vertexUV[0];

		/*// We can now use our formula to compute the tangent and the bitangent
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;*/


		 //Creating the TBN Matrix
		vec3 T = normalize(vec3(world * vec4(vertexTangent[i],0.0)));
		vec3 B = normalize(vec3(world * vec4(vertexBitangent[i],0.0)));
		vec3 N = normalize(vec3(world * vec4(normal,0.0)));
		TBN = mat3(T, B, N);

		
		worldPos = vec3(world * vec4(vertexPos[i], 1.0f));
		posLigtSpace = lightSpace * vec4(worldPos, 1.0f);
		vec3 camDir = normalize(worldPos - camPos);
		gl_Position = projection * view * world * gl_in[i].gl_Position;

		EmitVertex();	
		
	}
	EndPrimitive();

}
