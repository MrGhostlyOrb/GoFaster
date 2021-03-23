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

float currentAngle = 0;
float targetAngle = 0;

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

    //Set default coordinate system
    ProjectionMatrix = glm::ortho(-25.0, 25.0, -25.0, 25.0);
}


void display()
{
    //Clear the colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);

    //Enable blend before rendering
    glEnable(GL_BLEND);

    //Create a ViewMatrix with the identity matrix
    ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

    //Create a CarModelViewMatrix to transform the car to the correct x,y
    glm::mat4 CarModelViewMatrix = ViewMatrix;
    CarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.GetXPos(), car.GetYPos(), 0.0));

    //Set default track position to 0
    float trackPosition = 0;
    float roteAngle = 0;


    //Render a sequence of track
    for(int i = 0; i < 20; i++){

        //Create a TrackModelViewMatrix to position the track on an increasing x,y
        glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0, trackPosition, 0.0));

        //Render the track pieces
        trackUp.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

        //Increase track position
        trackPosition += 5;
    }

    //If the player is moving up, scroll the axis
    if(Up){
        orthoYmin += 0.11;
        orthoYmax += 0.11;
        ProjectionMatrix = glm::ortho(-25.0, 25.0, orthoYmin, orthoYmax);
    }

    //If the player is moving down, scroll the axis
    if (Down) {
        orthoYmin -= 0.09;
        orthoYmax -= 0.09;
        ProjectionMatrix = glm::ortho(-25.0, 25.0, orthoYmin, orthoYmax);

        //180 degree angle
        roteAngle = 1.57*2;

        //Create a PlayerModelViewMatrix to rotate the player based on movement direction
    }

    //If the player is moving left, rotate player
    if (Left) {

        //90 degree angle
        if(currentAngle >= targetAngle){
            std::cout << "Current Angle is less than target angle" << std::endl;
            currentAngle += 0.01;
        }
        else {
            std::cout << "Current Angle is equal to target angle" << std::endl;
            targetAngle += 0.1;
        }

        //Create a PlayerModelViewMatrix to rotate the player based on movement direction

    }

    //If the player is moving right, rotate player
    if (Right) {

        //90 degree angle
        if(currentAngle >= targetAngle){
            currentAngle -= 0.01;
            std::cout << "Current Angle is less than target angle" << std::endl;
        }
        else {
            std::cout << "Current Angle is equal to target angle" << std::endl;
            targetAngle -= 0.1;
        }

        //Create a PlayerModelViewMatrix to rotate the player based on movement direction

    }

    CarModelViewMatrix = glm::rotate(CarModelViewMatrix, currentAngle, glm::vec3(0.0, 0.0, 1.0));

    std::cout << "Current Angle : " << currentAngle << std::endl;
    std::cout << "Target Angle : " << targetAngle << std::endl;

    car.Render(shader, CarModelViewMatrix, ProjectionMatrix);
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
        std::cout << currentAngle << std::endl;

    }
    if (Right)
    {
        std::cout << currentAngle << std::endl;

    }
    if (Up)
    {
        car.IncPos(0.0f, 0.2f);
    }
    if (Down)
    {
        car.IncPos(0.0f, -0.2f);
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
    glutCreateWindow("Racer");

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
