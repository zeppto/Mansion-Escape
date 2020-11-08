#include "groundModel.h"

Ground::Ground(GLuint * gGroundShader, GLuint * gGroundVertexAttribute, GLuint * gGroundVertexBuffer, GLuint *cobblestoneTexture, GLuint *grassTexture, GLuint *blendTexture, GLuint *dirtTexture)
{
	this->gGroundShader = gGroundShader;
	this->gGroundVertexAttribute = gGroundVertexAttribute;
	this->gGroundVertexBuffer = gGroundVertexBuffer;

	this->blendTexture = blendTexture;
	this->cobblestoneTexture = cobblestoneTexture;
	this->grassTexture = grassTexture;
	this->dirtTexture = dirtTexture;
}

Ground::~Ground()
{
}

void Ground::setUpGround()
{
	struct GroundVertex
	{
		float x, y, z;
		float u, v;

	};

	GroundVertex triangleVerticesGround[6] =
	{
		//first
		{ -0.5f, 0.5f, 0.0, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
		{ 0.5f, 0.5f, 1.0f, 1.0f, 1.0f},

		{ 0.5f, 0.5f, 1.0, 1.0f, 1.0f},
		{ -0.5f, 0.5f, 1.0, 0.0f, 1.0f},
		{ -0.5f, 0.5f, 0.0, 0.0f, 0.0f},
	};
	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1,gGroundVertexAttribute);
	// bind is like "enabling" the object to use it
	glBindVertexArray(*gGroundVertexAttribute);
	// this activates the first and second attributes of this VAO
	// think of "attributes" as inputs to the Vertex Shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, gGroundVertexBuffer);

	//// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, *gGroundVertexBuffer);

	// This "could" imply copying to the GPU, depending on what the driver wants to do, and
	// the last argument (read the docs!)
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerticesGround), triangleVerticesGround, GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLint vertexPos = glGetAttribLocation(*gGroundShader, "vertex_position");
	// if this returns -1, it means there is a problem, and the program will likely crash.
	// examples: the name is different or missing in the shader

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(GroundVertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexUV = glGetAttribLocation(*gGroundShader, "vertex_UV");
	glVertexAttribPointer(
		vertexUV,
		2,
		GL_FLOAT,
		GL_FALSE, sizeof(GroundVertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float) * 3)	// note, the first color starts after the first vertex.
	);
}

void Ground::renderUpdateGround(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 camPos,
	glm::vec3 lightPos, glm::vec3 updateScale, float rotation, float positionX, float positionY, float positionZ)
{
	glUseProgram(*gGroundShader);

	glUniform1i(glGetUniformLocation(*gGroundShader, "cobblestoneTexture"), 0);
	glUniform1i(glGetUniformLocation(*gGroundShader, "grassTexture"), 1);
	glUniform1i(glGetUniformLocation(*gGroundShader, "blendTexture"), 2);
	glUniform1i(glGetUniformLocation(*gGroundShader, "dirtTexture"), 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *cobblestoneTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *grassTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *blendTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, *dirtTexture);

	glBindVertexArray(*gGroundVertexAttribute);

	glm::mat4 worldMatrix(1.0f);
	worldMatrix = glm::translate(worldMatrix, glm::vec3(positionX, positionY, positionZ));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(rotation), glm::vec3(1.f, 0.f, 0.f));
	worldMatrix = glm::scale(worldMatrix, updateScale);

	glUniformMatrix4fv(glGetUniformLocation(*gGroundShader, "worldMatrix"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(glGetUniformLocation(*gGroundShader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(*gGroundShader, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(23, 1, glm::value_ptr(lightPos));
	glUniform3fv(25, 1, glm::value_ptr(camPos));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
