#include <iostream>
using namespace std;

//--- OpenGL ---
#include "GL/glew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GL/freeglut.h"
#include "shaders/Shader.h"
#include "Square.h"
#include "Circle.h"

#include <iostream>
using namespace std;

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
int screenWidth = 480, screenHeight = 480;

Square myRedSquare, myGreenSquare;
Shader myShader;


//Variables for the positions of the squares
float XRedSquare = 0;
float YRedSquare = 0;
float XGreenSquare = 5;
float YGreenSquare = 0;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;

float Angle = 0.0f;

#define PI 3.1415926535897932384626433832795
const double ToRadians = PI / 180.0;

//OPENGL FUNCTION PROTOTYPES
void display();				//used as callback in glut for display.
void reshape(int width, int height); //used as callback for reshape function in glut
void init();				//called in main when the program starts.

/*************    START OF OPENGL FUNCTIONS   ****************/

void reshape(int width, int height)		// Resize the OpenGL window
{
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);						// set Viewport dimensions

    ProjectionMatrix = glm::ortho(-15.0, 15.0, -15.0, 15.0);
}


void display()
{
    //clear the colour buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //set the view matrix
    ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

    //set the modelviewmatrix for the green square
    glm::mat4 ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(XGreenSquare, YGreenSquare, 0.0));

    myGreenSquare.Render(myShader, ModelViewMatrix, ProjectionMatrix);

    //Set the modelviewtransform for the red square.
    glm::mat4 redTransform = glm::translate(ViewMatrix, glm::vec3(XRedSquare, YRedSquare, 0.0));
    myRedSquare.Render(myShader, redTransform, ProjectionMatrix);

    glutSwapBuffers();

    Angle += 0.0005f;
    if (Angle >= 360)
        Angle = 0;
}

void init()
{
    glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to black

    if (!myShader.load("Basic", "./glslfiles/basicTransformations.vert", "./glslfiles/basicTransformations.frag"))
    {
        std::cout << "failed to load shader" << std::endl;
    }

    myRedSquare.SetSideSize(4.0f);
    float red[3] = { 1,0,0 };
    myRedSquare.Init(myShader, red);

    myGreenSquare.SetSideSize(3.0f);
    float green[3] = { 0,1,0 };
    myGreenSquare.Init(myShader, green);

}

void special(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            Left = true;
            break;
        case GLUT_KEY_RIGHT:
            Right = true;
            break;
        case GLUT_KEY_UP:
            Up = true;
            break;
        case GLUT_KEY_DOWN:
            Down = true;
            break;
    }
}

void specialUp(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            Left = false;
            break;
        case GLUT_KEY_RIGHT:
            Right = false;
            break;
        case GLUT_KEY_UP:
            Up = false;
            break;
        case GLUT_KEY_DOWN:
            Down = false;
            break;
    }
}

void processKeys()
{
    if (Left)
    {
        XRedSquare -= 0.1f;
    }
    if (Right)
    {
        XRedSquare += 0.1f;
    }
    if (Up)
    {
        YRedSquare += 0.1f;
    }
    if (Down)
    {
        YRedSquare -= 0.1f;
    }
}

void idle()
{
    processKeys();

    glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

// FREEGLUT WINDOW SET UP
int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL FreeGLUT Example: Square");

    glutReshapeFunc(reshape);

    //This initialises glew - it must be called after the window is created.
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cout << " GLEW ERROR" << endl;
    }

    //Check the OpenGL version being used
    int OpenGLVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
    cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

    //initialise the objects for rendering
    init();

    //specify which function will be called to refresh the screen.
    glutDisplayFunc(display);


    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutIdleFunc(idle);

    //starts the main loop. Program loops and calls callback functions as appropriate.
    glutMainLoop();

    return 0;
}
