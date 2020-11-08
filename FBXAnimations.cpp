#include "FBXAnimations.h"

FBXAnimatios::FBXAnimatios(char * path, glm::vec3 worldPos)
{
	loadFBX(path);
	this->worldPos = worldPos;
	
}

FBXAnimatios::~FBXAnimatios()
{
}

void FBXAnimatios::loadFBX(std::string path)
{
	Assimp::Importer imp;
	const aiScene *scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fprintf(stderr, "assimp Error");
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void FBXAnimatios::processNode(aiNode * node, const aiScene * scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//prossesMesh här
		meshes.push_back(processMesh(mesh, scene));
		meshes[0].updateJointAnimation(scene->mAnimations[0]);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh FBXAnimatios::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<vertex> tempVertices;

	int boneArraysSize = mesh->mNumVertices * 4;
	boneIDs.resize(boneArraysSize);
	boneWeights.resize(boneArraysSize);

	for (int i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* aiBone = mesh->mBones[i];
		for (int j = 0; j < aiBone->mNumWeights; j++)
		{
			aiVertexWeight weight = aiBone->mWeights[j];
			unsigned int vertexStart = weight.mVertexId * 4;

			for (int y = 0; y < 4; y++)
			{
				if (boneWeights.at(vertexStart + y) == 0)
				{
					boneWeights.at(vertexStart + y) = weight.mWeight;
					boneIDs.at(vertexStart + y) = i;
					//vertices.at(weight.mVertexId).bones[y] = i;
					//vertices.at(weight.mVertexId).weigth[y] = weight.mWeight;
					break;
				}
			}

		}
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;

		vertex.nx = mesh->mNormals[i].x;
		vertex.ny = mesh->mNormals[i].y;
		vertex.nz = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vertex.u = mesh->mTextureCoords[0][i].x;
			vertex.v = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.u = 0;
			vertex.v = 0;
		}
		for (int j = 0; j < 4; j++)
		{
			vertex.bones[j] = boneIDs[i*4 + j];
			vertex.weigth[j] = boneWeights[i * 4 + j];
		}

		tempVertices.push_back(vertex);
	}
	// process indices

	//behövs kanske inte
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	for (int i = 0; i < indices.size(); i++)
	{
		vertices.push_back(tempVertices[indices[i]]);
	}

	//test
	for (int i = 0; i < mesh->mNumBones; i++)
		aiString temp = mesh->mBones[i]->mName;


	//undrar om den kan vara använbar om flera meshes
	//scene->mRootNode->mMeshes;

	//scene->mRootNode->mName;
	//Joint tempRootJoint(0, scene->mRootNode->mName, scene->mRootNode->mChildren, scene->mRootNode->mTransformation);

	Joint rootJoint = createJointConection(scene->mRootNode, mesh)[0];

	rootJoint.rootGlobalTransform(mesh, scene->mRootNode);
	// vvvvv är lika med invers bind pos? 
	mesh->mBones[0]->mOffsetMatrix;
	//temp//


		//// process material
		//if (mesh->mMaterialIndex >= 0)
		//{
		//	...
		//}

	return Mesh(worldPos, vertices, &rootJoint);
}
std::vector<Joint> FBXAnimatios::createJointConection(aiNode *joint, aiMesh * mesh)
{
	std::vector<Joint> allJoints, oldJoints;
	aiMatrix4x4 transform;
	if (mesh->HasBones())
	{
		//for (int i = 0; i < mesh->mNumBones; i++)
			//aiString temp = mesh->mBones[i]->mName;
		for (int i = 0; i < joint->mNumChildren; i++)
		{
			oldJoints = createJointConection(joint->mChildren[i], mesh);
			for (int j = 0; j < mesh->mNumBones; j++)
			{
				if (joint->mChildren[i]->mName == mesh->mBones[j]->mName)
				{
					aiNode * parantJoint = joint;
					for (int k = 0; k < 4; k++)
					{
						//transform *= parantJoint->mTransformation;
						//eller
						transform = parantJoint->mTransformation * transform;
						parantJoint = parantJoint->mParent;
					}
					Joint temp(j, joint->mChildren[i]->mName, oldJoints, transform);
					allJoints.push_back(temp);
					oldJoints.clear();
					j = mesh->mNumBones;
				}
			}
			for (int j = 0; j < oldJoints.size(); j++)
			{
				allJoints.push_back(oldJoints[j]);
			}
		}
	}
	else
	{
		allJoints.push_back(Joint(0, mesh->mName, oldJoints, transform));
	}
	return allJoints;
}

GLuint *FBXAnimatios::getShaderProgramFbx()
{
	return meshes[0].getShaderProgramFbx();
}

void FBXAnimatios::setUpModel()
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].setUpModel();
}

void FBXAnimatios::FbxRender(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 camraPos, glm::vec3 lightPos)
{
	for (int i = 0; i < meshes.size(); i++)
		Render(viewMatrix, projectionMatrix, camraPos, lightPos, i);
}

void FBXAnimatios::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 camraPos, glm::vec3 lightPos, int index)
{
	glUseProgram(*meshes[index].getShaderProgramFbx());
	//glUniform1i(glGetUniformLocation(gShaderProgram, "myTexture"), 0);
	//glUniform1i(glGetUniformLocation(gShaderProgram, "shadowMap"), 10);
	// tell opengl we want to use the gShaderProgram
	//glUseProgram(gShaderProgram);

	/*if (texturName != "")
	{
		//bindes texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, shadowMap);*/

	/*if (haveNormalMap)
	{
		glUniform1i(glGetUniformLocation(gShaderProgram, "normalTexture"), 1);

		float haveNormals = 1;
		unsigned int haveNormalsLoc = glGetUniformLocation(gShaderProgram, "haveNormals");
		glUniform1f(haveNormalsLoc, haveNormals);

		//bindes texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *normalTexture);
	}*/


	// tell opengl we are going to use the VAO we described earlier
	glBindVertexArray(*meshes[index].getVertexAttributeFbx());

	//dot have transparansy yet
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//sending values that change
	std::vector<glm::mat4> jointMatrixes;
	meshes[index].getJointMatixes(&jointMatrixes);
	glm::mat4 world(1.0f);
	world = glm::translate(world, worldPos);
	world = glm::translate(world, glm::vec3(0, 0, 0));
	world = glm::rotate(world, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	unsigned int worldLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "world");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
	unsigned int viewLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	unsigned int projectionLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	unsigned int jointMatrixLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "jointMatrixes");
	glUniformMatrix4fv(jointMatrixLoc, jointMatrixes.size(), GL_FALSE, glm::value_ptr(jointMatrixes[0]));
	//unsigned int lightSpaceLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "lightSpace");
	//glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	unsigned int lightLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "lightPos");
	glUniform3fv(lightLoc, 1, glm::value_ptr(lightPos));
	unsigned int camLoc = glGetUniformLocation(*meshes[index].getShaderProgramFbx(), "camPos");
	glUniform3fv(camLoc, 1, glm::value_ptr(camraPos));
	//unsigned int colorLoc = glGetUniformLocation(gShaderProgram, "color");
	//glUniform3fv(colorLoc, 1, glm::value_ptr(color));
	//unsigned int ambientColorLoc = glGetUniformLocation(gShaderProgram, "ambientColor");
	//glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));
	//texture
	//unsigned int specColorLoc = glGetUniformLocation(gShaderProgram, "specColor_Textur");
	//glm::vec4 in = glm::vec4(specularColor, haveTexture);
	//glUniform4fv(specColorLoc, 1, glm::value_ptr(in));
	//unsigned int transLoc = glGetUniformLocation(gShaderProgram, "transparency");
	//glUniform1f(transLoc, transparancy);
	glDrawArrays(GL_TRIANGLES, 0, meshes[index].vertexCount());

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

