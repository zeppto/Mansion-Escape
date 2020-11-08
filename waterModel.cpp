#include "waterModel.h"

Water::Water(GLuint * gWaterShaderProgram, GLuint * gWaterVertexAttribute, GLuint * gWaterVertexBuffer)
{
	this->gWaterShaderProgram = gWaterShaderProgram;
	this->gWaterVertexAttribute = gWaterVertexAttribute;
	this->gWaterVertexBuffer = gWaterVertexBuffer;
}

Water::~Water()
{
}

void Water::setUpWater()
{
	waterVertex triangleVertices[72] =
	{
		//first
		{ -0.5f, 0.5f, 0.0},
		{ 0.5f, 0.5f, 0.0f},
		{ 0.5f, 0.5f, 1.0f},

		{ 0.5f, 0.5f, 1.0},
		{ -0.5f, 0.5f, 1.0},
		{ -0.5f, 0.5f, 0.0},
		//second
		{ 0.5f, 0.5f, 0.0},
		{ 1.5f, 0.5f, 0.0f},
		{ 1.5f, 0.5f, 1.0f},

		{ 1.5f, 0.5f, 1.0},
		{ 0.5f, 0.5f, 1.0},
		{ 0.5f, 0.5f, 0.0},
		//third
		{ -0.5f, 0.5f, 1.0},
		{ 0.5f, 0.5f, 1.0f},
		{ 0.5f, 0.5f, 2.0f},

		{ 0.5f, 0.5f, 2.0},
		{ -0.5f, 0.5f, 2.0},
		{ -0.5f, 0.5f, 1.0},
		//forth
		{ 0.5f, 0.5f, 1.0},
		{ 1.5f, 0.5f, 1.0f},
		{ 1.5f, 0.5f, 2.0f},

		{ 1.5f, 0.5f, 2.0},
		{ 0.5f, 0.5f, 2.0},
		{ 0.5f, 0.5f, 1.0},
		//5
		{ 1.5f, 0.5f, 1.0},
		{ 2.5f, 0.5f, 1.0f},
		{ 2.5f, 0.5f, 2.0f},

		{ 2.5f, 0.5f, 2.0},
		{ 1.5f, 0.5f, 2.0},
		{ 1.5f, 0.5f, 1.0},
		//6
		{ 1.5f, 0.5f, 0.0},
		{ 2.5f, 0.5f, 0.0f},
		{ 2.5f, 0.5f, 1.0f},

		{ 2.5f, 0.5f, 1.0},
		{ 1.5f, 0.5f, 1.0},
		{ 1.5f, 0.5f, 0.0},

		//7
		{ -0.5f, 0.5f, 1.0},
		{ 0.5f, 0.5f, 1.0f},
		{ 0.5f, 0.5f, 2.0f},

		{ 0.5f, 0.5f, 2.0},
		{ -0.5f, 0.5f, 2.0},
		{ -0.5f, 0.5f, 1.0},
		//8
		{ 0.5f, 0.5f, 1.0},
		{ 1.5f, 0.5f, 1.0f},
		{ 1.5f, 0.5f, 2.0f},

		{ 1.5f, 0.5f, 2.0},
		{ 0.5f, 0.5f, 2.0},
		{ 0.5f, 0.5f, 1.0},
		//9
		{ -0.5f, 0.5f, 2.0},
		{ 0.5f, 0.5f, 2.0f},
		{ 0.5f, 0.5f, 3.0f},

		{ 0.5f, 0.5f, 3.0},
		{ -0.5f, 0.5f, 3.0},
		{ -0.5f, 0.5f, 2.0},
		//10
		{ 0.5f, 0.5f, 2.0},
		{ 1.5f, 0.5f, 2.0f},
		{ 1.5f, 0.5f, 3.0f},

		{ 1.5f, 0.5f, 3.0},
		{ 0.5f, 0.5f, 3.0},
		{ 0.5f, 0.5f, 2.0},
		//11
		{ 1.5f, 0.5f, 2.0},
		{ 2.5f, 0.5f, 2.0f},
		{ 2.5f, 0.5f, 3.0f},

		{ 2.5f, 0.5f, 3.0},
		{ 1.5f, 0.5f, 3.0},
		{ 1.5f, 0.5f, 2.0f},
		//12
		{ 1.5f, 0.5f, 1.0},
		{ 2.5f, 0.5f, 1.0f},
		{ 2.5f, 0.5f, 2.0f},

		{ 2.5f, 0.5f, 2.0},
		{ 1.5f, 0.5f, 2.0},
		{ 1.5f, 0.5f, 1.0},

	};
	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1, gWaterVertexAttribute);
	// bind is like "enabling" the object to use it
	glBindVertexArray(*gWaterVertexAttribute);
	// this activates the first and second attributes of this VAO
	// think of "attributes" as inputs to the Vertex Shader
	glEnableVertexAttribArray(0);

	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, gWaterVertexBuffer);

	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, *gWaterVertexBuffer);

	// This "could" imply copying to the GPU, depending on what the driver wants to do, and
	// the last argument (read the docs!)
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLint vertexPos = glGetAttribLocation(*gWaterShaderProgram, "vertex_position");
	// if this returns -1, it means there is a problem, and the program will likely crash.
	// examples: the name is different or missing in the shader

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(waterVertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);
}

void Water::updateWaterTime(float deltaTime)
{
	this->waterTime += deltaTime * this->waterSpeed;
	unsigned int waterLoc = glGetUniformLocation(*gWaterShaderProgram, "waterTime");
	glUniform1f(waterLoc, this->waterTime);
}

void Water::renderWater(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 camPos,
	glm::vec3 lightPos, float positonX, float positonY, float positonZ, float rotation)
{
	glUseProgram(*gWaterShaderProgram);

	glBindVertexArray(*gWaterVertexAttribute);

	glm::mat4 worldMatrix(1.0f);
	worldMatrix = glm::translate(worldMatrix, glm::vec3(positonX, positonY, positonZ));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));

	glUniformMatrix4fv(glGetUniformLocation(*gWaterShaderProgram, "worldMatrix"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(*gWaterShaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(*gWaterShaderProgram, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3fv(42, 1, glm::value_ptr(lightPos));
	glUniform3fv(43, 1, glm::value_ptr(camPos));

	glDrawArrays(GL_TRIANGLES, 0, 72);
}
