#pragma once

#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"

#include "shaders/Shader.h"
#include "OBB.h"

class Car
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[3];	// ids for Vertex Buffer Objects
	unsigned int m_NumberOfVerts;//number of vertices in the object

	//Dimensions of the sprite.
	float m_Width;
	float m_Height;

	//position of the sprite
	float m_xpos;
	float m_ypos;
	float m_rot;

	GLuint m_TexName; //identifier for the texture

	OBB obb; //obb object for oriented bounding box collision

public:
	Car();					//default constructor
	void init(Shader& shader, float *colour, std::string filename);
	void setWidth(float size);
	void setHeight(float size);
	void SetXpos(float x);
	void SetYpos(float y);
	float getXPos();
	float getYPos();
	void IncPos(float x, float y);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
	OBB& GetOBB();
	bool isInCollision(OBB &anotherOBB);

    void incRot(float rot);

    void decRot(float rot);

    float getRot();
};