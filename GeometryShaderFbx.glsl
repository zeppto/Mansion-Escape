#version 440
layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPos[];
in vec2 vertexUV[];
in vec3 vertexNormal[];
in vec4 weights[];
in ivec4 bonesID[];

out vec3 normal;
out vec3 worldPos;
out vec2 texCoord;
out vec4 myWeights;
out ivec4 myBonesID;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 jointMatrixes[64];


uniform vec3 lightPos;
uniform vec3 camPos;

void main() 
{


	for(int i = 0; i < 3; i++)
	{
		myBonesID = bonesID[i];
		myWeights = weights[i];
		normal = vertexNormal[i];
		texCoord = vertexUV[i];
		
		float total = myWeights[0] + myWeights[1] + myWeights[2] + myWeights[3]; 

		float first = myWeights[0] / total;
		float second = myWeights[1] / total;
		float third = myWeights[2] / total;
		float fourth = myWeights[3] / total;
		float finalValue = first + second + third + fourth;
		
		myWeights[0] = first;
		myWeights[1] = second;
		myWeights[2] = third;
		myWeights[3] = fourth;

		
		if(myWeights[0] != 0)
		{
			mat4 BoneTransForm = jointMatrixes[myBonesID[0]] * myWeights[0];
			BoneTransForm += jointMatrixes[myBonesID[1]] * myWeights[1];
			BoneTransForm += jointMatrixes[myBonesID[2]] * myWeights[2];
			BoneTransForm += jointMatrixes[myBonesID[3]] * myWeights[3];

			gl_Position = (projection * view * world) * (BoneTransForm * gl_in[i].gl_Position);

			//vec4 normalTemp = BoneTransForm * vec4(normal);
			normal = mat3(transpose(inverse(world))) * normal;
			worldPos = vec3(world * vec4(vertexPos[i], 1.0f));

		}
		else
		{
			normal = mat3(transpose(inverse(world))) * normal;

			worldPos = vec3(world * vec4(vertexPos[i], 1.0f));
			vec3 camDir = normalize(worldPos - camPos);
			gl_Position = projection * view * world * gl_in[i].gl_Position;
		}
		
		EmitVertex();
		
	}
	EndPrimitive();
}