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
#include "Marker.h"

#include <iostream>
using namespace std;

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 TextProjectionMatrix;
int screenWidth = 800, screenHeight = 800;

Shader shader;

//Car object
Car car;
Car npcCar;

//Track objects
Square trackUp;
Square trackRight;

//Wall objects
Wall wallBottom;
Wall wallRight;
Wall wallTop;
Wall wallLeft;

const int markersSize = 5;
Marker markers[markersSize];
Marker marker1;
Marker marker2;
Marker marker3;
Marker marker4;
Marker marker5;

freetype::Font font;

//Initialising global variables
float currentAngle = 0.2;
float currentAngleNpc = 0.1;
float npcTargetAngle = 0;
bool isReversing = false;
float velocity = 0;
float npcVelocity = 0;
bool npcIsReversing = false;

//Speed and other modifiers
const float TURNING_SPEED = 0.2;
const float MAX_VELOCITY = 0.5;
const float ORTHO = 25;
const float ZOOM = 10;

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

//Menu and game mechanics
bool renderMenu = true;

//OPENGL FUNCTION PROTOTYPES
void display();                //used as callback in glut for display.
void reshape(int width, int height); //used as callback for reshape function in glut
void init();                //called in main when the program starts.

void processNpc();

float calculateDistance(float x, float y);

void moveToMarker();

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

    if(!renderMenu) {

        for (int i = 0; i < 5; i++) {
            glm::mat4 MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(markers[i].getXPos(),
                                                                                   markers[i].getYPos(), 0.0));
            markers[i].Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        }

        glm::mat4 WallBottomModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wallBottom.getXPos(),
                                                                                   wallBottom.getYPos(), 0.0));
        glm::mat4 WallRightModelViewMatrix = glm::translate(ViewMatrix,
                                                            glm::vec3(wallRight.getXPos(), wallRight.getYPos(), 0.0));
        glm::mat4 WallTopModelViewMatrix = glm::translate(ViewMatrix,
                                                          glm::vec3(wallTop.getXPos(), wallTop.getYPos(), 0.0));
        glm::mat4 WallLeftModelViewMatrix = glm::translate(ViewMatrix,
                                                           glm::vec3(wallLeft.getXPos(), wallLeft.getYPos(), 0.0));


        wallBottom.Render(shader, WallBottomModelViewMatrix, ProjectionMatrix);
        wallRight.Render(shader, WallRightModelViewMatrix, ProjectionMatrix);
        wallTop.Render(shader, WallTopModelViewMatrix, ProjectionMatrix);
        wallLeft.Render(shader, WallLeftModelViewMatrix, ProjectionMatrix);


        if (car.isInCollision(wallBottom.getOBB()) || car.isInCollision(wallRight.getOBB()) ||
            car.isInCollision(wallTop.getOBB()) || car.isInCollision(wallLeft.getOBB())) {
            cout << "Collided" << endl;
            velocity = 0;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        //Create a CarModelViewMatrix to transform the car to the correct x,y
        glm::mat4 CarModelViewMatrix = ViewMatrix;
        CarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.getXPos(), car.getYPos(), 0.0));

        glm::mat4 NpcCarModelViewMatrix = ViewMatrix;
        NpcCarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(npcCar.getXPos(), npcCar.getYPos(), 0.0));

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

        NpcCarModelViewMatrix = glm::rotate(NpcCarModelViewMatrix, currentAngleNpc, glm::vec3(0.0, 0.0, 1.0));
        npcCar.Render(shader, NpcCarModelViewMatrix, ProjectionMatrix);


        car.Render(shader, CarModelViewMatrix, ProjectionMatrix);

        std::string currentAngleStr = std::to_string(currentAngle);

        glm::mat4 StoreProjectionMatrix = ProjectionMatrix;

        ProjectionMatrix = glm::ortho(-25, 25, -25, 25);

        ProjectionMatrix = glm::translate(ProjectionMatrix, glm::vec3(car.getXPos(), car.getYPos(), 0.0));

        //glm::lookAt(glm::vec3(car.getXPos(), car.getYPos(), 0.0), glm::vec3)

        print(ProjectionMatrix, font, 0, 0, currentAngleStr.c_str());

        ProjectionMatrix = StoreProjectionMatrix;
    }
    else{
        glm::mat4 FontProjectionMatrix = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(20.0, 430.0, 0.0));

        print(ProjectionMove, font, 10, 10, "Welcome to Racer Racer 2021");
    }
    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void init() {

    FreeImage_Initialise();

    glClearColor(0.0, 0.5, 0.0, 0.2);                        //sets the clear colour to black

    //Load the GLSL program
    if (!shader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag")) {
        std::cout << "failed to load shader" << std::endl;
    }



    ///This part commented is to scale the width of the sprite to match the dimensions of the car.png image.
    car.setWidth(5.0f);
    car.setHeight(10.0f);
    npcCar.setWidth(5.0f);
    npcCar.setHeight(10.0f);
    npcCar.SetXpos(-50.0f);
    npcCar.SetYpos(-50.0f);

    wallBottom.setWidth(250.0f);
    wallBottom.setHeight(5.0f);
    wallRight.setWidth(5.0f);
    wallRight.setHeight(250.0f);
    wallTop.setWidth(250.0f);
    wallTop.setHeight(5.0f);
    wallLeft.setWidth(5.0f);
    wallLeft.setHeight(250.0f);

    wallRight.setXpos(225);
    wallRight.setYpos(100);
    wallBottom.setXpos(100);
    wallBottom.setYpos(-25);
    wallTop.setXpos(100);
    wallTop.setYpos(225);
    wallLeft.setXpos(-25);
    wallLeft.setYpos(100);

    trackUp.setWidth(10.0f * (500 / 264.0f));
    trackUp.setHeight(10.0f);
    trackRight.setWidth(10.0f * (500 / 264.0f));
    trackRight.setHeight(10.0f);
    float red[3] = {1, 0, 0};
    float blue[3] = {0, 0, 1};

    wallBottom.init(shader, red, "roadTexture_26.png");
    wallRight.init(shader, red, "roadTexture_26.png");
    wallTop.init(shader, red, "roadTexture_26.png");
    wallLeft.init(shader, red, "roadTexture_26.png");

    car.init(shader, red, "car.png");
    npcCar.init(shader, red, "car.png");
    trackUp.init(shader, blue, "roadTexture_84.png");
    trackRight.init(shader, blue, "roadTexture_84.png");


    marker1.SetXPos(0);
    marker1.SetYPos(0);

    marker2.SetXPos(0);
    marker2.SetYPos(0);
    marker3.SetXPos(0);
    marker3.SetYPos(0);
    marker4.SetXPos(0);
    marker4.SetYPos(0);
    marker5.SetXPos(0);
    marker5.SetYPos(0);


    marker1.init(shader, red, "roadTexture_26.png");
    marker2.init(shader, red, "roadTexture_26.png");
    marker3.init(shader, red, "roadTexture_26.png");
    marker4.init(shader, red, "roadTexture_26.png");
    marker5.init(shader, red, "roadTexture_26.png");

    markers[0] = marker1;
    markers[1] = marker2;
    markers[2] = marker3;
    markers[3] = marker4;
    markers[4] = marker5;


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
            break;
        case GLUT_KEY_DOWN:
            keyDown = false;
            break;
        default:
            break;
    }
}

void processKeys() {

    //Left and right keys control the angle of the car
    if (keyLeft) {

        if (!isReversing) {

                currentAngle += TURNING_SPEED / 10;

        } else {

                currentAngle -= TURNING_SPEED / 10;

        }
    }

    if (keyRight) {

        if (!isReversing) {


                currentAngle -= TURNING_SPEED / 10;

        } else {

                currentAngle += TURNING_SPEED / 10;

        }

    }


    //Up key controls the acceleration of the car
    if (keyUp) {

        //Limit velocity to 2
        if(velocity > MAX_VELOCITY){
            velocity = MAX_VELOCITY;
        }
        else{
            //Increase velocity when holding up key
            velocity += 0.01;
        }

        isReversing = false;

        //Increase the position of the car based on the angle
        float normalAngle;
        if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
            normalAngle = remainder(currentAngle, 2 * M_PI);
        } else {
            normalAngle = currentAngle;
        }


            //  Bottom keyLeft -
            //.
        if (normalAngle < -M_PI) {
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
    }
    else{
        //Case for if the up key is not being held
        if(velocity > 0){
            velocity -= 0.05;
        }
        else{
            velocity = 0;
        }
        isReversing = false;
        float normalAngle;
        if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
            normalAngle = remainder(currentAngle, 2 * M_PI);
        } else {
            normalAngle = currentAngle;
        }

        //  Bottom keyLeft -
        //.
        if (normalAngle < -M_PI) {
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
    }

    if (keyDown) {

        cout << velocity << endl;

        isReversing = true;

        velocity += 0.05;


        float normalAngle;
        if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
            normalAngle = remainder(currentAngle, 2 * M_PI);
        } else {
            normalAngle = currentAngle;
        }

            //  Bottom keyLeft -
            //.
        if (normalAngle < -M_PI) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin -= cos(normalAngle) * velocity;
            orthoYMax -= cos(normalAngle) * velocity;
            orthoXMin -= -sin(normalAngle) * velocity;
            orthoXMax -= -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight - G
            // .
        else if (normalAngle < -(M_PI / 2)) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin -= cos(normalAngle) * velocity;
            orthoYMax -= cos(normalAngle) * velocity;
            orthoXMin += sin(normalAngle) * velocity;
            orthoXMax += sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            // . Top keyRight - G
            //
        else if (normalAngle < 0) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin += -cos(normalAngle) * velocity;
            orthoYMax += -cos(normalAngle) * velocity;
            orthoXMin += sin(normalAngle) * velocity;
            orthoXMax += sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyRight +
            // .
        else if (normalAngle > M_PI) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin -= cos(normalAngle) * velocity;
            orthoYMax -= cos(normalAngle) * velocity;
            orthoXMin += sin(normalAngle) * velocity;
            orthoXMax += sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //   Bottom keyLeft + G
            //.
        else if (normalAngle > (M_PI / 2)) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin -= cos(normalAngle) * velocity;
            orthoYMax -= cos(normalAngle) * velocity;
            orthoXMin -= -sin(normalAngle) * velocity;
            orthoXMax -= -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
            //.  Top keyLeft +
            //
        else if (normalAngle >= 0) {
            car.IncPos(sin(normalAngle) * velocity, -cos(normalAngle) * velocity);
            orthoYMin += -cos(normalAngle) * velocity;
            orthoYMax += -cos(normalAngle) * velocity;
            orthoXMin -= -sin(normalAngle) * velocity;
            orthoXMax -= -sin(normalAngle) * velocity;
            ProjectionMatrix = glm::ortho(orthoXMin, orthoXMax, orthoYMin, orthoYMax);
        }
    }

}

void keyFunction(unsigned char key, int x, int y) {

    switch(key){
        //Esc
        case 27:
            glutLeaveMainLoop();
        //Carriage Return
        case 13:
            renderMenu = false;
        default:
            cout << "Default" << endl;
    }
}

void idle() {

    if(!renderMenu){
        //Process key presses for player
        processKeys();

        //Process to run npc
        processNpc();
    }

    glutPostRedisplay();
}

void processNpc() {

   /* int closestMarker = 0;
    Marker markerToChoose;*/

    /*for(int i = 0; i < markersSize; i++){

        float newMarker = calculateDistance(markers[i].getXPos(), markers[i].getYPos());

        if(newMarker < closestMarker){
            closestMarker = newMarker;
            markerToChoose = markers[i];
        }
    }*/

    moveToMarker();


}

void moveToMarker() {

    //Define x and y for npc vector

    float normalAngle;
    if (currentAngle > 2 * M_PI || currentAngle < 2 * M_PI) {
        normalAngle = remainder(currentAngle, 2 * M_PI);
    } else {
        normalAngle = currentAngle;
    }


    cout << "Normal Angle : " << normalAngle << endl;

    float carVectorX = cos(normalAngle);
    float carVectorY = sin(normalAngle);

    //Define x and y for marker vector
    float npcVectorX = (car.getXPos() - npcCar.getXPos());
    float npcVectorY = (car.getYPos() - npcCar.getYPos());

    cout << "Npc Vector : " << npcVectorX << " " << npcVectorY << endl;
    cout << "Car Vector : " << carVectorX << " " << carVectorY << endl;

    //Dot product of 2 vectors
    float udotv = (npcVectorX*carVectorX)+(npcVectorY*carVectorX);

    //Absolute values of both vectors
    float absu = sqrt((npcVectorX*npcVectorX)+(npcVectorY*npcVectorY));
    float absv = sqrt((carVectorX*carVectorX)+(carVectorY*carVectorY));

    //Find the cosine of the angle between both vectors
    if(absv*absu == 0){
        cout << "Divide by 0" << endl;
    }
    else{
        double cosAngle = udotv/(absu*absv);
        cout << "u.v : " << udotv << endl;
        cout << "mod(u) : " << absu << endl;
        cout << "mod(v) : " << absv << endl;
        npcTargetAngle = acos(cosAngle);
        cout << "Angle to be targeted : " << acos(cosAngle) << endl;
    }


    //Decide whether to add or subtract the angle
    if(npcTargetAngle >= currentAngleNpc){
        cout << "Current Npc angle : " << currentAngleNpc << endl;
        currentAngleNpc += 0.04;
        cout << "New Npc angle : " << currentAngleNpc << endl;
    }
   /* else if(npcTargetAngle < currentAngleNpc){
        currentAngleNpc -= 0.04;
    }
    else{
        cout << "Here" << endl;
    }*/

    //Limit velocity to 2
    if(npcVelocity > MAX_VELOCITY){
        npcVelocity = MAX_VELOCITY;
        cout << "Npc at max velocity" << endl;
    }
    else{
        //Increase velocity when holding up key
        npcVelocity += 0.01;
    }

    npcIsReversing = false;


    cout << currentAngleNpc << endl;

    cout << "Increasing position by : " << cos(currentAngleNpc) * npcVelocity << ", " << sin(currentAngleNpc) * npcVelocity << endl;
    npcCar.IncPos(cos(currentAngleNpc) * npcVelocity, sin(currentAngleNpc) * npcVelocity);

}

float calculateDistance(float x, float y) {
    float carX = npcCar.getXPos();
    float carY = npcCar.getYPos();

    float distance = sqrt(((carX - carY)*(carX - carY)) + ((x - y)*(x - y)));

    return distance;
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
