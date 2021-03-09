#include "Circle.h"
#include "shaders/Shader.h"

#include <iostream>

Circle::Circle()
{
	m_vaoID = 0;
	m_vboID[0] = 0;
	m_vboID[1] = 0;
	m_Radius = 0.0f;
}

void Circle::SetRadius(float radius)
{
	m_Radius = radius;
}


void Circle::Init(Shader& shader, float colour[3])
{

	//Create the geometry
	double step = 10;
	int size = (int)((360.0 / step) + 1) * 3;
	auto* vert = new float[size];
	auto* col = new float[size];

	m_NumOfVerts = 0;
	for (double i = 0; i <= 360; i += step)
	{
		float xcoord = m_Radius * cos(glm::radians(i));
		float ycoord = m_Radius * sin(glm::radians(i));
		vert[m_NumOfVerts * 3] = xcoord;
		vert[m_NumOfVerts * 3 + 1] = ycoord;
		vert[m_NumOfVerts * 3 + 2] = 0.0;

		col[m_NumOfVerts * 3] = 0.0;
		col[m_NumOfVerts * 3 + 1] = 0.0;
		col[m_NumOfVerts * 3 + 2] = 1.0;


		m_NumOfVerts++;
	}
	
	//VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(2, m_vboID); // we need two VBOs - one for the vertices and one for the colours
							 
	//Lets set up the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);

	//initialises data storage of vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, m_NumOfVerts * 3 * sizeof(GLfloat), vert, GL_STATIC_DRAW);

	//set the position - linked to the position shader input
	GLint vertexLocation = glGetAttribLocation(shader.handle(), "in_Position");
	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Now set up the colours
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, m_NumOfVerts * 3 * sizeof(GLfloat), col, GL_STATIC_DRAW);

	//set the colour - linked to the colour shader input.
	GLint colorLocation = glGetAttribLocation(shader.handle(), "in_Color");
	glEnableVertexAttribArray(colorLocation);
	//location in shader, number of componentns,  type, normalised, stride, pointer to first attribute
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//good practice to bind to 0.
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Circle::Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix)
{
	glUseProgram(shader.handle());  // use the shader

	//set the uniform for the projectionmatrix
	glUniformMatrix4fv(glGetUniformLocation(shader.handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//pass the uniform for the ModelView matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader.handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//Draw the object
	glPointSize(5.0);
	glBindVertexArray(m_vaoID);		// select first VAO
	glDrawArrays(GL_POINTS, 0, m_NumOfVerts);	// draw first object

	glBindVertexArray(0); //unbind the vertex array object
	glUseProgram(0); //turn off the current shader
}
