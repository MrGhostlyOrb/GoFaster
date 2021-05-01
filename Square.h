#pragma once

#include "shaders/Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBB.h"
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
    float m_XPos;
    float m_Ypos;

    OBB obb; //obb object for oriented bounding box collision

public:
	Square();					//default constructor
	void init(Shader& shader, float *colour, std::string filename);
	void SetSideSize(float size);
	float GetSideSize();
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
    void setWidth(float size);
    void setHeight(float size);
    void SetXPos(float XPos);
    void SetYPos(float YPos);
    float GetXPos();
    float GetYPos();
    bool isInCollision(OBB &anotherOBB);
    OBB &GetOBB();
};