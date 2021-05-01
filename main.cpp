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
#include <thread>

using namespace std;

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
int screenWidth = 800, screenHeight = 800;

Shader shader;

//Car object
Car car;
Car npcCar;

//Track objects
Square trackUp;
Square trackRight;
Square trackCorner;
Square start_finish;

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

Marker carMarker1;
Marker carMarker2;
Marker carMarker3;
Marker carMarker4;
Marker carMarker5;

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
const float TURNING_SPEED = 0.4;
const float MAX_VELOCITY = 2;
const float ORTHO = 25;
const float ZOOM = 3;

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

bool passed1 = false;
bool passed2 = false;
bool passed3 = false;
bool passed4 = false;
bool passed5 = false;

bool npcPassed1 = false;
bool npcPassed2 = false;
bool npcPassed3 = false;
bool npcPassed4 = false;
bool npcPassed5 = false;

//Menu and game mechanics
bool renderMenu = true;
bool npcCarFinished = false;
bool carFinished = false;
bool renderFinish = false;
bool renderCountdown = false;

int npcCollisionCount = 0;
int carCollisionCount = 0;

int countdownCount = 0;

//OPENGL FUNCTION PROTOTYPES
void display();                //used as callback in glut for display.
void reshape(int width, int height); //used as callback for reshape function in glut
void init();                //called in main when the program starts.

void processNpc();

float calculateDistance(float x, float y);

void moveToMarker();

void checkPlayerCollisions();

void resetGame();

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

    if(renderCountdown){
        renderMenu = false;
        cout << "Here" << endl;
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if(renderMenu){

        cout << "Rendering Menu" << endl;

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.0, 1.0, 0.0));

        print(ProjectionMove, font, 20, 20, "Press <Enter> to start");

    }
    else if(renderFinish){
        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.0, 1.0, 0.0));

        if(carFinished){
            print(ProjectionMove, font, 20, 20, "You Win!");
        }
        else{
            print(ProjectionMove, font, 20, 20, "You Lose!");
        }


    }
    else if(renderCountdown){

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.0, 1.0, 0.0));

        if(countdownCount == 0){
            cout << "Counting Down..." << endl;
        }
        else if(countdownCount == 1){
            cout << "3" << endl;
            print(ProjectionMove, font, 20, 20, "3");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 2){
            cout << "2" << endl;
            print(ProjectionMove, font, 20, 20, "2");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 3){
            cout << "1" << endl;
            print(ProjectionMove, font, 20, 20, "1");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 4){
            cout << "GO!" << endl;
            print(ProjectionMove, font, 20, 20, "GO!");
            this_thread::sleep_for(1000ms);
            renderCountdown = false;
        }

        countdownCount += 1;

    }
    else {

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
            velocity = -0.5;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (npcCar.isInCollision(wallBottom.getOBB()) || npcCar.isInCollision(wallRight.getOBB()) ||
            npcCar.isInCollision(wallTop.getOBB()) || npcCar.isInCollision(wallLeft.getOBB())) {
            cout << "Collided" << endl;
            npcVelocity = -0.5;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        //Create a CarModelViewMatrix to transform the car to the correct cornerAngle,y
        glm::mat4 CarModelViewMatrix = ViewMatrix;
        CarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.getXPos(), car.getYPos(), 0.0));

        glm::mat4 NpcCarModelViewMatrix = ViewMatrix;
        NpcCarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(npcCar.getXPos(), npcCar.getYPos(), 0.0));

        //Set default track position to 0
        float trackPosition = 0;

        //Render a sequence of track going up
        for (int i = 0; i < 20; i++) {

            //Create a TrackModelViewMatrix to position the track on an increasing cornerAngle,y
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10.0, trackPosition, 0.0));

            //Render the track pieces
            trackUp.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

            //Increase track position
            trackPosition += 10;
        }

        glm::mat4 TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10.0, -10.0, 0.0));
        float cornerAngle = (M_PI / 2);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        trackPosition = 0;
        for (int i = 0; i < 20; i++) {

            //Create a TrackModelViewMatrix to position the track on an increasing cornerAngle,y
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(208.0, trackPosition, 0.0));

            //Render the track pieces
            trackUp.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

            //Increase track position
            trackPosition += 10;
        }

        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(208.0, -10.0, 0.0));
        cornerAngle = (M_PI);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        trackPosition = 4;
        for (int i = 0; i < 20; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, -10.0, 0.0));

            float x = (M_PI / 2);

            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));

            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

            trackPosition += 10;
        }

        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(208.0, 204.0, 0.0));
        cornerAngle = (3* M_PI/2 );
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        trackPosition = 4;
        for (int i = 0; i < 20; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 204.0, 0.0));

            float x = (M_PI / 2);

            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));

            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);

            trackPosition += 10;
        }

        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10.0, 204.0, 0.0));
        cornerAngle = 0;
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        glm::mat4 StartFinishModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(20.0, -10.0, 1.0));
        StartFinishModelViewMatrix = glm::rotate(StartFinishModelViewMatrix, (float)M_PI/2, glm::vec3(0.0, 0.0, 1.0));
        start_finish.Render(shader, StartFinishModelViewMatrix, ProjectionMatrix);

        glm::mat4 MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(220, -10, 0.0));
        marker1.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(200, 220, 0.0));
        marker2.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-20, 200, 0.0));
        marker3.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, -20, 0.0));
        marker4.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);

        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(200, -10, 0.0));
        carMarker1.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(200, 200, 0.0));
        carMarker2.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, 200, 0.0));
        carMarker3.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);
        MarkerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, 0, 0.0));
        carMarker4.Render(shader, MarkerModelViewMatrix, ProjectionMatrix);


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

    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void init() {

    FreeImage_Initialise();

    glClearColor(0.4, 0.3, 0.2, 0.2);                        //sets the clear colour to black

    //Load the GLSL program
    if (!shader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag")) {
        std::cout << "failed to load shader" << std::endl;
    }



    ///This part commented is to scale the width of the sprite to match the dimensions of the car.png image.
    car.setWidth(5.0f);
    car.setHeight(10.0f);
    car.SetXpos(10);
    car.SetYpos(-5);
    currentAngle = 3*M_PI/2;

    npcCar.setWidth(5.0f);
    npcCar.setHeight(10.0f);
    npcCar.SetXpos(10.0f);
    npcCar.SetYpos(-15.0f);
    currentAngleNpc = 3*M_PI/2;

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

    trackCorner.setWidth(19.0f);
    trackCorner.setHeight(19.0f);
    trackCorner.init(shader, red, "roadTexture_02.png");

    start_finish.setWidth(19.0f);
    start_finish.setHeight(19.0f/2);
    start_finish.init(shader, red, "tribune_overhang_striped.png");

    wallBottom.init(shader, red, "roadTexture_26.png");
    wallRight.init(shader, red, "roadTexture_26.png");
    wallTop.init(shader, red, "roadTexture_26.png");
    wallLeft.init(shader, red, "roadTexture_26.png");

    car.init(shader, red, "car.png");
    npcCar.init(shader, red, "car.png");
    trackUp.init(shader, blue, "roadTexture_84.png");
    trackRight.init(shader, blue, "roadTexture_84.png");


    marker1.setWidth(20);
    marker1.setHeight(20);

    marker2.setWidth(20);
    marker2.setHeight(20);

    marker3.setWidth(20);
    marker3.setHeight(20);

    marker4.setWidth(20);
    marker4.setHeight(20);

    marker5.setWidth(20);
    marker5.setHeight(20);


    marker2.SetXPos(0);
    marker2.SetYPos(0);
    marker3.SetXPos(0);
    marker3.SetYPos(0);
    marker4.SetXPos(0);
    marker4.SetYPos(0);
    marker5.SetXPos(0);
    marker5.SetYPos(0);

    carMarker1.setWidth(20);
    carMarker1.setHeight(20);

    carMarker2.setWidth(20);
    carMarker2.setHeight(20);

    carMarker3.setWidth(20);
    carMarker3.setHeight(20);

    carMarker4.setWidth(20);
    carMarker4.setHeight(20);

    carMarker5.setWidth(20);
    carMarker5.setHeight(20);


    carMarker2.SetXPos(0);
    carMarker2.SetYPos(0);
    carMarker3.SetXPos(0);
    carMarker3.SetYPos(0);
    carMarker4.SetXPos(0);
    carMarker4.SetYPos(0);
    carMarker5.SetXPos(0);
    carMarker5.SetYPos(0);


    marker1.init(shader, red, "roadTexture_26.png");
    marker2.init(shader, red, "roadTexture_26.png");
    marker3.init(shader, red, "roadTexture_26.png");
    marker4.init(shader, red, "roadTexture_26.png");
    marker5.init(shader, red, "roadTexture_26.png");

    carMarker1.init(shader, red, "tribune_overhang_striped.png");
    carMarker2.init(shader, red, "tribune_overhang_striped.png");
    carMarker3.init(shader, red, "tribune_overhang_striped.png");
    carMarker4.init(shader, red, "tribune_overhang_striped.png");
    carMarker5.init(shader, red, "tribune_overhang_striped.png");

    markers[0] = marker1;
    markers[1] = marker2;
    markers[2] = marker3;
    markers[3] = marker4;
    markers[4] = marker5;


    font.init("fonts/arialbd.ttf", 30);

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
            renderFinish = false;
            resetGame();
            renderCountdown = true;
            countdownCount = 0;
        default:
            cout << "Default" << endl;
    }
}

void resetGame() {
    car.SetXpos(10);
    car.SetYpos(-5);
    currentAngle = 3*M_PI/2;

    npcCar.SetXpos(10.0f);
    npcCar.SetYpos(-15.0f);
    currentAngleNpc = 3*M_PI/2;

    orthoYMax = ORTHO * ZOOM;
    orthoYMin = -ORTHO * ZOOM;
    orthoXMin = -ORTHO * ZOOM;
    orthoXMax = ORTHO * ZOOM;

    carCollisionCount = 0;
    carFinished = false;
    velocity = 0;
    passed1 = false;
    passed2 = false;
    passed3 = false;
    passed4 = false;
    passed5 = false;

    npcCollisionCount = 0;
    npcCarFinished = false;
    npcVelocity = 0;
    npcPassed1 = false;
    npcPassed2 = false;
    npcPassed3 = false;
    npcPassed4 = false;
    npcPassed5 = false;

}

void idle() {

    if(renderMenu == false && renderFinish == false){
        //Process key presses for player
        processKeys();

        checkPlayerCollisions();

        //Process to run npc
        processNpc();
    }

    glutPostRedisplay();
}

void checkPlayerCollisions() {

    if(car.isInCollision(carMarker1.GetOBB())){
        carCollisionCount = 1;
        passed1 = true;
    }
    else if(car.isInCollision((carMarker2.GetOBB()))){
        carCollisionCount = 2;
        passed2 = true;
    }
    else if(car.isInCollision((carMarker3.GetOBB()))){
        carCollisionCount = 3;
        passed3 = true;
    }
    else if(car.isInCollision((carMarker4.GetOBB()))){
        carCollisionCount = 4;
        passed4 = true;
    }
    else if(car.isInCollision((start_finish.GetOBB()))){
        carCollisionCount = 5;
        passed4 = true;
    }

    if(carCollisionCount == 5){
        passed5 = true;
    }

    if(carCollisionCount == 5 && passed1 && passed2 && passed3 && passed4 && passed5){
        carFinished = true;
        cout << "Car finished" << endl;
        renderFinish = true;
    }

}

void processNpc() {

    if(npcVelocity < MAX_VELOCITY){
        npcVelocity += 0.01;
    }
    else{
        npcVelocity = MAX_VELOCITY;
    }

    if(npcCar.isInCollision(marker1.GetOBB())){
        npcCollisionCount = 1;
        npcVelocity = 0.5;
        npcPassed1 = true;
    }
    else if(npcCar.isInCollision((marker2.GetOBB()))){
        npcCollisionCount = 2;
        npcVelocity = 0.5;
        npcPassed2 = true;
    }
    else if(npcCar.isInCollision((marker3.GetOBB()))){
        npcCollisionCount = 3;
        npcVelocity = 0.5;
        npcPassed3 = true;
    }
    else if(npcCar.isInCollision((marker4.GetOBB()))){
        npcCollisionCount = 4;
        npcVelocity = 0.5;
        npcPassed4 = true;
    }
    else if(npcCar.isInCollision((start_finish.GetOBB()))){
        npcCollisionCount = 5;
        npcVelocity = 0.5;
        npcPassed5 = true;
    }


    if(npcCollisionCount == 0){
        npcCar.IncPos(npcVelocity, 0);
        cout << "Section 1" << endl;
    }
    else if(npcCollisionCount == 1){
        currentAngleNpc = 0;
        npcCar.IncPos(0, npcVelocity);
        cout << "Section 2" << endl;
    }
    else if(npcCollisionCount == 2){
        currentAngleNpc = M_PI/2;
        npcCar.IncPos(-npcVelocity, 0);
        cout << "Section 3" << endl;
    }
    else if(npcCollisionCount == 3){
        currentAngleNpc = M_PI;
        npcCar.IncPos(0, -npcVelocity);
        cout << "Section 4" << endl;
    }
    else if(npcCollisionCount == 4){
        currentAngleNpc = 3*M_PI/2;
        npcCar.IncPos(npcVelocity, 0);
        cout << "Section 5" << endl;
    }
    else if(npcCollisionCount == 5){
        currentAngleNpc = 3*M_PI/2;
        npcCar.IncPos(npcVelocity, 0);
        cout << "Section 5" << endl;
    }

    if(npcCollisionCount == 5 && npcPassed1 && npcPassed2 && npcPassed3 && npcPassed4 && npcPassed5){
        currentAngleNpc = 3*M_PI/2;
        npcCar.IncPos(0, 0);
        npcCarFinished = true;
        cout << "Npc Finished" << endl;
        renderFinish = true;
    }



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
