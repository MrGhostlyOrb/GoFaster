#pragma once

#include "shaders/Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Square
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

	float m_SideSize;

public:
	Square();					//default constructor
	void Init(Shader& shader, float colour[3]);
	void SetSideSize(float size);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};