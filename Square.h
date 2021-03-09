#pragma once

#include "shaders/Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>

class Square
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

    float m_Width;
    float m_Height;
	float m_SideSize;
    GLuint m_TexName; //identifier for the texture
    unsigned int m_NumberOfVerts;//number of vertices in the object

public:
	Square();					//default constructor
	void Init(Shader& shader, float colour[3], std::string filename);
	void SetSideSize(float size);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
    void SetWidth(float size);
    void SetHeight(float size);
};