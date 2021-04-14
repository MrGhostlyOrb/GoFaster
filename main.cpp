#include <iostream>

using namespace std;

#include "GL/glew.h"

#pragma comment(lib, "glew32.lib")

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shaders/Shader.h"
#include "GL/freeglut.h"
#include "FreeImage.h"
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Car.h"
#include "Square.h"
#include "FreeType.h"
#include "Wall.h"

#include <iostream>
using namespace std;

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 TextProjectionMatrix;
int screenWidth = 480, screenHeight = 480;

Shader shader;
Car car;
Square trackUp;
Square trackRight;
Wall wall;

freetype::Font font;

//Initialising global variables
float currentAngle = 0;
float targetAngle = 0;
bool isReversing = false;

//Speed and other modifiers
const float TURNING_SPEED = 0.8;
float velocity = 0;
const float REVERSE_SPEED = 0.5;
const float ORTHO = 25;
const float ZOOM = 1;

//Ortho assignments
double orthoYMax = ORTHO * ZOOM;
double orthoYMin = -ORTHO * ZOOM;
double orthoXMin = -ORTHO * ZOOM;
double orthoXMax = ORTHO * ZOOM;

//booleans to handle when the arrow keys are pressed or released.
bool keyLeft = false;
bool keyRight = false;
bool keyUp = false;
bool keyDown = false;

//OPENGL FUNCTION PROTOTYPES
void display();                //used as callback in glut for display.
void reshape(int width, int height); //used as callback for reshape function in glut
void init();                //called in main when the program starts.

/*************    START OF OPENGL FUNCTIONS   ****************/

void reshape(int width, int height)        // Resize the OpenGL window
{
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);                        // set Viewport dimensions

    //Set default coordinate system
    ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
}

void display() {
    //Clear the colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);

    //Enable blend before rendering
    glEnable(GL_BLEND);

    //Create a ViewMatrix with the identity matrix
    ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

    glm::mat4 WallModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wall.GetXPos(), wall.GetYPos(), 0.0));

    wall.Render(shader, WallModelViewMatrix, ProjectionMatrix);


    if(car.IsInCollision(wall.GetOBB())){
        cout << "Collided" << endl;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }



    //Create a CarModelViewMatrix to transform the car to the correct x,y
    glm::mat4 CarModelViewMatrix = ViewMatrix;
    CarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.GetXPos(), car.GetYPos(), 0.0));

    //Set default track position to 0
    float trackPosition = 0;

    //Render a sequence of track going up
    for (int i = 0; i < 20; i++) {

        //Create a TrackModelViewMatrix to position the track on an increasing x,y
        glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0, trackPosition, 0.0));

        //Render the track pieces
        trackUp.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

        //Increase track position
        trackPosition += 10;
    }

    trackPosition = 0;
    for (int i = 0; i < 20; i++) {

        //Create a TrackModelViewMatrix to position the track on an increasing x,y
        glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(200.0, trackPosition, 0.0));

        //Render the track pieces
        trackUp.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

        //Increase track position
        trackPosition += 10;
    }

    trackPosition = 0;
    for (int i = 0; i < 20; i++) {
        glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 0.0, 0.0));

        float x = (M_PI / 2);

        TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));

        trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

        trackPosition += 10;
    }

    trackPosition = 0;
    for (int i = 0; i < 20; i++) {
        glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 200.0, 0.0));

        float x = (M_PI / 2);

        TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));

        trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

        trackPosition += 10;
    }

    CarModelViewMatrix = glm::rotate(CarModelViewMatrix, currentAngle, glm::vec3(0.0, 0.0, 1.0));

    std::cout << "Current Angle : " << currentAngle << std::endl;
    std::cout << "Target Angle : " << targetAngle << std::endl;

    car.Render(shader, CarModelViewMatrix, ProjectionMatrix);

    std::string currentAngleStr = std::to_string(currentAngle);

    glm::mat4 StoreProjectionMatrix = ProjectionMatrix;

    ProjectionMatrix = glm::ortho(-25, 25, -25, 25);

    ProjectionMatrix = glm::translate(ProjectionMatrix, glm::vec3(car.GetXPos(), car.GetYPos(), 0.0));

    //glm::lookAt(glm::vec3(car.GetXPos(), car.GetYPos(), 0.0), glm::vec3)

    print(ProjectionMatrix, font, 0, 0, currentAngleStr.c_str());

    ProjectionMatrix = StoreProjectionMatrix;

    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void init() {

    FreeImage_Initialise();

    glClearColor(1.0, 0.0, 0.0, 0.0);                        //sets the clear colour to black

    //Load the GLSL program
    if (!shader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag")) {
        std::cout << "failed to load shader" << std::endl;
    }



    ///This part commented is to scale the width of the sprite to match the dimensions of the car.png image.
    car.SetWidth(5.0f);
    car.SetHeight(10.0f);
    wall.SetWidth(500.0f);
    wall.SetHeight(5.0f);

    wall.SetXpos(-5);
    wall.SetYpos(-15);

    trackUp.SetWidth(10.0f * (500 / 264.0f));
    trackUp.SetHeight(10.0f);
    trackRight.SetWidth(10.0f * (500 / 264.0f));
    trackRight.SetHeight(10.0f);
    float red[3] = {1, 0, 0};
    float blue[3] = {0, 0, 1};

    wall.Init(shader, red, "roadTexture_26.png");

    car.Init(shader, red, "car.png");
    trackUp.Init(shader, blue, "roadTexture_84.png");
    trackRight.Init(shader, blue, "roadTexture_84.png");

    font.init("fonts/arialbd.ttf", 12);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            keyLeft = true;
            break;
        case GLUT_KEY_RIGHT:
            keyRight = true;
            break;
        case GLUT_KEY_UP:
            keyUp = true;
            break;
        case GLUT_KEY_DOWN:
            keyDown = true;
            break;
        case GLUT_KEY_DELETE:
            glutLeaveMainLoop();
        default:
            break;
    }
}

void specialUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            keyLeft = false;
            break;
        case GLUT_KEY_RIGHT:
            keyRight = false;
            break;
        case GLUT_KEY_UP:
            keyUp = false;
            velocity = 0;
            break;
        case GLUT_KEY_DOWN:
            keyDown = false;
            velocity = 0;
            break;
        default:
            break;
    }
}

void processKeys() {

    if (keyLeft) {

        if (!isReversing) {

            if (currentAngle >= targetAngle) {
                std::cout << "Current Angle is greater than than target angle" << currentAngle << " : " << targetAngle
                          << std::endl;
                currentAngle += TURNING_SPEED / 10;
            } else {
                std::cout << "Current Angle is equal to target angle" << std::endl;
                targetAngle += TURNING_SPEED;
            }
        } else {
            if (currentAngle >= targetAngle) {
                std::cout << "Current Angle is greater than than target angle" << currentAngle << " : " << targetAngle
                          << std::endl;
                currentAngle -= TURNING_SPEED / 10;
            } else {
                std::cout << "Current Angle is equal to target angle" << std::endl;
                targetAngle -= TURNING_SPEED;
            }
        }
    }

    if (keyRight) {

        if (!isReversing) {

            if (currentAngle >= targetAngle) {
                std::cout << "Current Angle is greater than than target angle" << currentAngle << " : " << targetAngle
                          << std::endl;
                currentAngle -= TURNING_SPEED / 10;
            } else {
                std::cout << "Current Angle is equal to target angle" << std::endl;
                targetAngle -= TURNING_SPEED;
            }
        } else {
            if (currentAngle >= targetAngle) {
                std::cout << "Current Angle is greater than than target angle" << currentAngle << " : " << targetAngle
                          << std::endl;
                currentAngle += TURNING_SPEED / 10;
            } else {
                std::cout << "Current Angle is equal to target angle" << std::endl;
                targetAngle += TURNING_SPEED;
            }
        }

    }

    if (keyUp) {
        velocity += 0.005;
        isReversing = false;
        float normalAngle;
        if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
            normalAngle = remainder(currentAngle, 2 * M_PI);
        } else {
            normalAngle = currentAngle;
        }

        std::cout << normalAngle << std::endl;

            //  Bottom keyLeft -
            //.
        if (normalAngle < -M_PI) {
            std::cout << "(-cos, -sin)" << std::endl;
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin -= -cos(normalAngle) * velocity;
            orthoYMax -= -cos(normalAngle) * velocity;
            orthoXMin -= sin(normalAngle) * velocity;
            orthoXMax -= sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight - G
            // .
        else if (normalAngle < -(M_PI / 2)) {
            std::cout << "(-sin, cos)" << std::endl;
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin -= -cos(normalAngle) * velocity;
            orthoYMax -= -cos(normalAngle) * velocity;
            orthoXMin += -sin(normalAngle) * velocity;
            orthoXMax += -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            // . Top keyRight - G
            //
        else if (normalAngle < 0) {
            std::cout << "(-sin, cos)" << std::endl;
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin += cos(normalAngle) * velocity;
            orthoYMax += cos(normalAngle) * velocity;
            orthoXMin += -sin(normalAngle) * velocity;
            orthoXMax += -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight +
            // .
        else if (normalAngle > M_PI) {
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin -= -cos(normalAngle) * velocity;
            orthoYMax -= -cos(normalAngle) * velocity;
            orthoXMin += -sin(normalAngle) * velocity;
            orthoXMax += -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyLeft + G
            //.
        else if (normalAngle > (M_PI / 2)) {
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin -= -cos(normalAngle) * velocity;
            orthoYMax -= -cos(normalAngle) * velocity;
            orthoXMin -= sin(normalAngle) * velocity;
            orthoXMax -= sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //.  Top keyLeft +
            //
        else if (normalAngle >= 0) {
            car.IncPos(-sin(normalAngle) * velocity, cos(normalAngle) * velocity);
            orthoYMin += cos(normalAngle) * velocity;
            orthoYMax += cos(normalAngle) * velocity;
            orthoXMin -= sin(normalAngle) * velocity;
            orthoXMax -= sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }

        std::cout << "Normal Angle : " << normalAngle << std::endl;
    }

    if (keyDown) {

        isReversing = true;
        velocity += 0.005;

        float normalAngle;
        if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
            normalAngle = remainder(currentAngle, 2 * M_PI);
        } else {
            normalAngle = currentAngle;
        }

        std::cout << normalAngle << std::endl;

            //  Bottom keyLeft -
            //.
        if (normalAngle < -M_PI) {
            std::cout << "(-cos, -sin)" << std::endl;
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin -= cos(normalAngle) * REVERSE_SPEED;
            orthoYMax -= cos(normalAngle) * REVERSE_SPEED;
            orthoXMin -= -sin(normalAngle) * REVERSE_SPEED;
            orthoXMax -= -sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight - G
            // .
        else if (normalAngle < -(M_PI / 2)) {
            std::cout << "(-sin, cos)" << std::endl;
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin -= cos(normalAngle) * REVERSE_SPEED;
            orthoYMax -= cos(normalAngle) * REVERSE_SPEED;
            orthoXMin += sin(normalAngle) * REVERSE_SPEED;
            orthoXMax += sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            // . Top keyRight - G
            //
        else if (normalAngle < 0) {
            std::cout << "(-sin, cos)" << std::endl;
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin += -cos(normalAngle) * REVERSE_SPEED;
            orthoYMax += -cos(normalAngle) * REVERSE_SPEED;
            orthoXMin += sin(normalAngle) * REVERSE_SPEED;
            orthoXMax += sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight +
            // .
        else if (normalAngle > M_PI) {
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin -= cos(normalAngle) * REVERSE_SPEED;
            orthoYMax -= cos(normalAngle) * REVERSE_SPEED;
            orthoXMin += sin(normalAngle) * REVERSE_SPEED;
            orthoXMax += sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyLeft + G
            //.
        else if (normalAngle > (M_PI / 2)) {
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin -= cos(normalAngle) * REVERSE_SPEED;
            orthoYMax -= cos(normalAngle) * REVERSE_SPEED;
            orthoXMin -= -sin(normalAngle) * REVERSE_SPEED;
            orthoXMax -= -sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //.  Top keyLeft +
            //
        else if (normalAngle >= 0) {
            car.IncPos(sin(normalAngle) * REVERSE_SPEED, -cos(normalAngle) * REVERSE_SPEED);
            orthoYMin += -cos(normalAngle) * REVERSE_SPEED;
            orthoYMax += -cos(normalAngle) * REVERSE_SPEED;
            orthoXMin -= -sin(normalAngle) * REVERSE_SPEED;
            orthoXMax -= -sin(normalAngle) * REVERSE_SPEED;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
    }
}

void keyFunction(unsigned char key, int x, int y) {
    if (key == 27) {
        glutLeaveMainLoop();
    }
}

void idle() {
    processKeys();

    glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

// FREEGLUT WINDOW SET UP
int main(int argc, char **argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Racer");

    glutReshapeFunc(reshape);

    //This initialises glew - it must be called after the window is created.
    GLenum err = glewInit();
    if (GLEW_OK != err) {
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
    glutKeyboardFunc(keyFunction);

    glutIdleFunc(idle);

    //starts the main loop. Program loops and calls callback functions as appropriate.
    glutMainLoop();

    return 0;
}
