#include "Mesh.h"

Mesh::Mesh(glm::vec3 worldPos, std::vector<vertex> vertices, Joint * rootJoint)
{
	this->vertices = vertices;
	this->worldPos = worldPos;
	this->rootJoint = new Joint(*rootJoint);
}

Mesh::~Mesh()
{
}

void Mesh::setUpModel()
{
	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1, &gVertexAttribute);
	// bind is like "enabling" the object to use it
	glBindVertexArray(gVertexAttribute);
	// this activates the first and second attributes of this VAO
	// think of "attributes" as inputs to the Vertex Shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);


	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, &gVertexBuffer);

	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);

	// This "could" imply copying to the GPU, depending on what the driver wants to do, and
	// the last argument (read the docs!)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);


	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	// if this returns -1, it means there is a problem, and the program will likely crash.
	// examples: the name is different or missing in the shader


	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		0,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(vertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	//for normals
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 6)	// note, the first color starts after the first vertex.
	);

	glVertexAttribPointer(
		3,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 8)	// note, the first color starts after the first vertex.
	);

	glVertexAttribIPointer(
		4,
		4,
		GL_INT,
		sizeof(vertex),
		BUFFER_OFFSET(sizeof(float) * 12)
	);
	glBindVertexArray(0);

	/*if (texturName != "")
	{
		createTexture();
		haveTexture = 1;
	}*/
}



GLuint * Mesh::getShaderProgramFbx()
{
	return &gShaderProgram;
}

GLuint * Mesh::getVertexAttributeFbx()
{
	return &gVertexAttribute;
}

GLuint * Mesh::getVertexBufferFbx()
{
	return &gVertexBuffer;
}

int Mesh::vertexCount()
{
	return vertices.size();
}

void Mesh::createTexture()
{
	/*if (texturName != "")
	{
		std::string findTextureFile = path + texturName;
		int image_width = 0;
		int image_height = 0;
		unsigned char* image = SOIL_load_image(findTextureFile.c_str(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		SOIL_free_image_data(image);
	}*/
}

Joint * Mesh::getRootJoint()
{
	return rootJoint;
}

void Mesh::updateJointAnimation(aiAnimation * animation)
{
	rootJoint->updateAnimation(animation);
}

void Mesh::getJointMatixes(std::vector<glm::mat4> * returnMatrixs)
{
	rootJoint->getMatrixes(returnMatrixs);
}

