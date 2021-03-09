#pragma once

#include "shaders/Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Circle
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

	float m_Radius;
	int m_NumOfVerts;

public:
	Circle();					//default constructor
	void Init(Shader& shader, float colour[3]);
	void SetRadius(float radius);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};