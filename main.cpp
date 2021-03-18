#include <iostream>
using namespace std;

//--- OpenGL ---
#include "GL/glew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shaders/Shader.h"
#include "GL/freeglut.h"
#include "FreeImage.h"

#include "Car.h"
#include "Square.h"

#include <iostream>
using namespace std;

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
int screenWidth = 480, screenHeight = 480;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;

double orthoYmax = 25;
double orthoYmin = -25;
double orthoXmin = -25;
double orthoXmax = 25;

Shader shader;
Car car;
Square trackUp;

float Angle = 0.5f;

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

    ProjectionMatrix = glm::ortho(-25.0, 25.0, -25.0, 25.0);
}


void display()
{
    //clear the colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);

    ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
    glm::mat4 ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.GetXPos(), car.GetYPos(), 0.0));


    glEnable(GL_BLEND);

    float trackPosition = 0;

    for(int i = 0; i < 20; i++){
        glm::mat4 ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0, trackPosition, 0.0));
        trackUp.Render(shader, ModelViewMatrix, ProjectionMatrix);
        trackPosition += 5;
    }

    if(Up){
        orthoYmin += 0.11;
        orthoYmax += 0.11;
        ProjectionMatrix = glm::ortho(-25.0, 25.0, orthoYmin, orthoYmax);
    }
    if (Down) {
        orthoYmin -= 0.09;
        orthoYmax -= 0.09;
        ProjectionMatrix = glm::ortho(-25.0, 25.0, orthoYmin, orthoYmax);
        float roteAngle = 1.57*2;
        ModelViewMatrix = glm::rotate(ModelViewMatrix, roteAngle, glm::vec3(0.0, 0.0, 1.0));


    }
    if (Left) {
        float roteAngle = 1.57;
        ModelViewMatrix = glm::rotate(ModelViewMatrix,roteAngle,glm::vec3(0.0, 0.0, 1.0));
    }


    if (Right) {
        float roteAngle = -1.57;
        ModelViewMatrix = glm::rotate(ModelViewMatrix, roteAngle, glm::vec3(0.0, 0.0, 1.0));
    }






    ModelViewMatrix = glm::rotate(ModelViewMatrix, car.getRot(), glm::vec3(0.0, 0.0, 1.0));

    car.Render(shader, ModelViewMatrix, ProjectionMatrix);
    glDisable(GL_BLEND);

    glutSwapBuffers();

}

void init()
{
    FreeImage_Initialise();

    glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to black

    //Load the GLSL program
    if (!shader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag"))
    {
        std::cout << "failed to load shader" << std::endl;
    }

    ///This part commented is to scale the width of the sprite to match the dimensions of the car.png image.
    car.SetWidth(5.0f);
    car.SetHeight(10.0f);
    trackUp.SetWidth(10.0f * (500 / 264.0f));
    trackUp.SetHeight(10.0f);
    float red[3] = { 1,0,0 };
    float blue[3] = { 0,0,1 };

    car.Init(shader, red, "car.png");
    trackUp.Init(shader, blue, "roadTexture_84.png");


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void special(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            Left = true;
            Angle += 0.5;
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
        if(car.getRot() <= -3){
            car.incRot(0.01);
        }
        else{
            car.decRot(0.01);
        }
        std::cout << car.getRot() << std::endl;

    }
    if (Right)
    {
        if(car.getRot() <= -3){
            car.decRot(0.01);
        }
        else{
            car.incRot(0.01);
        }
        std::cout << car.getRot() << std::endl;

    }
    if (Up)
    {
        car.IncPos(0.0f, 0.1f);
    }
    if (Down)
    {
        car.IncPos(0.0f, -0.1f);
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
    glutCreateWindow("OpenGL FreeGLUT Example: Image loading");

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
