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
#include <irrklang/irrKlang.h>
#include "conio.h"

using namespace irrklang;

//ISoundEngine* SoundEngine = createIrrKlangDevice();
//SoundEngine->play2D("firstSound.mp3", true);

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

Square innerSquare;

Square arrowKeys;

//Wall objects
Wall wallBottom;
Wall wallRight;
Wall wallTop;
Wall wallLeft;

Wall wallBottom_2;
Wall wallRight_2;
Wall wallTop_2;
Wall wallLeft_2;

Wall innerWall;
Wall innerWall_2;
Wall innerWall_3;

const int markersSize = 5;
Marker markers[markersSize];
Marker marker1;
Marker marker2;
Marker marker3;
Marker marker4;
Marker marker5;
Marker marker6;
Marker marker7;
Marker marker8;

Marker carMarker1;
Marker carMarker2;
Marker carMarker3;
Marker carMarker4;
Marker carMarker5;
Marker carMarker6;
Marker carMarker7;
Marker carMarker8;

Marker npcMarker1;
Marker npcMarker2;
Marker npcMarker3;
Marker npcMarker4;
Marker npcMarker5;
Marker npcMarker6;
Marker npcMarker7;
Marker npcMarker8;

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
const float TURNING_SPEED = 0.6;
float MAX_VELOCITY = 2;
const float ORTHO = 25;
const float ZOOM = 3;
float ACCELERATION = 0.01;
float NPC_ADVANTAGE = 1.5;
float COLLISION_SPEED = 0.5;

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
bool passed6 = false;
bool passed7 = false;
bool passed8 = false;

bool npcPassed1 = false;
bool npcPassed2 = false;
bool npcPassed3 = false;
bool npcPassed4 = false;
bool npcPassed5 = false;
bool npcPassed6 = false;
bool npcPassed7 = false;
bool npcPassed8 = false;

bool passedStart = false;
bool npcPassedStart = false;

//Menu and game mechanics
bool renderMenu = true;
bool renderControlMenu = false;
bool npcCarFinished = false;
bool carFinished = false;
bool renderFinish = false;
bool renderCountdown = false;
bool clockRunning = false;
float bestScores[3];
std::array<float,3> bestScores2;

int currentLevel = 0;
bool renderLevel1 = false;
bool renderLevel2 = false;

int npcCollisionCount = 0;
int carCollisionCount = 0;

int countdownCount = 0;
int timer = 0;
int storeTime = 0;

int bannerX = 0;

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

    cout << "Current Level : " << currentLevel << endl;

    if(currentLevel == 1){
        renderLevel1 = true;
    }
    else if(currentLevel == 2){
        renderLevel2 = true;
    }

    if(clockRunning){
        timer += 1;
    }


    //Clear the colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);


    //Enable blend before rendering
    glEnable(GL_BLEND);

    //Create a ViewMatrix with the identity matrix
    ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));

    if(renderCountdown || renderControlMenu){
        renderMenu = false;
    }

    cout << to_string(renderLevel2) << endl;

    if(renderMenu){

        cout << "Rendering Menu" << endl;

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 20, 20, "Press <Enter> to start");

        ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 20, 70, "Press <C> to view controls");

        ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 220, 250, "Race to the finish.");
        ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 100, 200, "Beat the PURPLE car to Go Faster.");

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.5, 1.5, 0.0));

        print(ProjectionMove, font, 175, 400, "Go Faster");

        ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));
        print(ProjectionMove, font, 70, 475, "Select a track...");

        if(currentLevel == 1){
            ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));
            print(ProjectionMove, font, 70, 425, "<1> Track 1 :EASY: <--");

            ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));
            print(ProjectionMove, font, 70, 375, "<2> Track 2 :HARD:");
        }
        else if(currentLevel == 2){

            ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));
            print(ProjectionMove, font, 70, 425, "<1> Track 1 :EASY:");

            ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));
            print(ProjectionMove, font, 70, 375, "<2> Track 2 :HARD: <--");
        }


        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.5, 1.5, 0.0));

        if(bannerX > -200){
            bannerX = -800;
        }
        else{
            bannerX += 2;
        }


        print(ProjectionMove, font, bannerX, 500, "Go Faster Go Faster Go Faster Go Faster Go Faster Go Faster Go Faster Go Faster ");




    }
    else if(renderControlMenu){
        cout << "Rendering Control Menu" << endl;

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 100, 650, "Controls:");

        ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        print(ProjectionMove, font, 100, 550, "Press <UP> to Accelerate");
        print(ProjectionMove, font, 100, 450, "Press <LEFT> or <RIGHT> to Steer");
        print(ProjectionMove, font, 100, 350, "Press <DOWN> to Brake or Reverse");

        glm::mat4 ProjectionMatrixKeys = glm::translate(ViewMatrix, glm::vec3(0, -45, 0.0));
        arrowKeys.Render(shader, ProjectionMatrixKeys, ProjectionMatrix);
    }
    else if(renderFinish){
        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.0, 1.0, 0.0));

        if(carFinished){
            clockRunning = false;
            storeTime = timer;
            print(ProjectionMove, font, 320, 700, "You Win!");

            char* char_arr;
            string str_obj("Your time : " + to_string((float)storeTime/60) + " seconds");
            char_arr = &str_obj[0];

            print(ProjectionMove, font, 125, 600, char_arr);

            char* char_arr2;
            string str_obj2("TOP SPEED LEVEL : " + to_string((int)MAX_VELOCITY - 2));
            char_arr2 = &str_obj2[0];

            print(ProjectionMove, font, 125, 500, char_arr2);

            print(ProjectionMove, font, 125, 400, "Best Times:");
            int startingY = 350;
            for(float & bestScore : bestScores2){
                char* char_arr;
                string str_obj(to_string((float)bestScore) + " seconds");
                char_arr = &str_obj[0];
                print(ProjectionMove, font, 125, startingY, char_arr);
                startingY -= 80;
            }
            print(ProjectionMove, font, 20, 60, "Press <Enter> to Go Faster");
        }
        else{

            char* char_arr2;
            string str_obj2("TOP SPEED LEVEL : " + to_string((int)MAX_VELOCITY - 2));
            char_arr2 = &str_obj2[0];

            print(ProjectionMove, font, 125, 500, char_arr2);
            print(ProjectionMove, font, 320, 700, "You Lose!");
            print(ProjectionMove, font, 125, 400, "Best Times:");
            int startingY = 350;
            for(float & bestScore : bestScores2){
                char* char_arr;
                string str_obj(to_string((float)bestScore) + " seconds");
                char_arr = &str_obj[0];
                print(ProjectionMove, font, 125, startingY, char_arr);
                startingY -= 80;
            }
            print(ProjectionMove, font, 20, 60, "Press <Enter> to Restart");
        }
        print(ProjectionMove, font, 20, 100, "Press <M> to return to Menu");
        print(ProjectionMove, font, 20, 20, "Press <Esc> to Exit");


    }
    else if(renderCountdown){

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        ProjectionMove = glm::scale(ProjectionMove, glm::vec3(1.5, 1.5, 0.0));

        if(countdownCount == 0){
            cout << "Counting Down..." << endl;
        }
        else if(countdownCount == 1){
            cout << "3" << endl;

            print(ProjectionMove, font, 250, 300, "3");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 2){
            cout << "2" << endl;
            print(ProjectionMove, font, 250, 300, "2");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 3){
            cout << "1" << endl;
            print(ProjectionMove, font, 250, 300, "1");
            this_thread::sleep_for(1000ms);
        }
        else if(countdownCount == 4){
            cout << "GO!" << endl;
            print(ProjectionMove, font, 175, 300, "GO Faster!");
            this_thread::sleep_for(1000ms);
        }
        else if (countdownCount == 5){
            this_thread::sleep_for(1000ms);
            renderCountdown = false;
        }

        countdownCount += 1;
        cout << "Loading Level : " << currentLevel << endl;

    }
    else if(renderLevel1){

        if(car.isInCollision(npcCar.GetOBB())){
            if(velocity > MAX_VELOCITY/4){
                velocity -= 0.1;
            }
            if(npcVelocity > MAX_VELOCITY/4){
                npcVelocity -= 0.1;
            }
        }

        if(car.isInCollision(innerWall.getOBB())){
            velocity = COLLISION_SPEED;
        }
        if(npcCar.isInCollision(innerWall.getOBB())){
            npcVelocity = COLLISION_SPEED;
        }

        clockRunning = true;
        char* char_arr;
        string str_obj(to_string((float)timer/60));
        char_arr = &str_obj[0];

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

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
            velocity = COLLISION_SPEED;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (npcCar.isInCollision(wallBottom.getOBB()) || npcCar.isInCollision(wallRight.getOBB()) ||
            npcCar.isInCollision(wallTop.getOBB()) || npcCar.isInCollision(wallLeft.getOBB())) {
            cout << "Collided" << endl;
            npcVelocity = COLLISION_SPEED;
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

        glm::mat4 WallModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100, 100, 0.0));
        innerWall.Render(shader, WallModelViewMatrix, ProjectionMatrix);

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

        print(ProjectionMove, font, 20, 20, char_arr);
    }
    else if(renderLevel2){

        cout << "Hereeeeeeeeeeeeeeeeee" << endl;

        if(car.isInCollision(npcCar.GetOBB())){
            if(velocity > MAX_VELOCITY/4){
                velocity -= 0.1;
            }
            if(npcVelocity > MAX_VELOCITY/4){
                npcVelocity -= 0.1;
            }
        }

        /*if(car.isInCollision(innerWall.getOBB())){
            velocity = 0.5;
        }
        if(npcCar.isInCollision(innerWall.getOBB())){
            npcVelocity = 0.5;
        }*/

        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        clockRunning = true;
        char* char_arr;
        string str_obj(to_string((float)timer/60));
        char_arr = &str_obj[0];

        glm::mat4 FontProjectionMatrix = glm::ortho(1.0f, (float)screenWidth, 1.0f, (float)screenHeight);

        glm::mat4 ProjectionMove = glm::translate(FontProjectionMatrix, glm::vec3(0.0, 0.0, 0.0));

        glm::mat4 WallBottomModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wallBottom_2.getXPos(),
                                                                                   wallBottom_2.getYPos(), 0.0));
        glm::mat4 WallRightModelViewMatrix = glm::translate(ViewMatrix,
                                                            glm::vec3(wallRight_2.getXPos(), wallRight_2.getYPos(), 0.0));
        glm::mat4 WallTopModelViewMatrix = glm::translate(ViewMatrix,
                                                          glm::vec3(wallTop_2.getXPos(), wallTop_2.getYPos(), 0.0));
        glm::mat4 WallLeftModelViewMatrix = glm::translate(ViewMatrix,
                                                           glm::vec3(wallLeft_2.getXPos(), wallLeft_2.getYPos(), 0.0));


        wallBottom_2.Render(shader, WallBottomModelViewMatrix, ProjectionMatrix);
        wallRight_2.Render(shader, WallRightModelViewMatrix, ProjectionMatrix);
        wallTop_2.Render(shader, WallTopModelViewMatrix, ProjectionMatrix);
        wallLeft_2.Render(shader, WallLeftModelViewMatrix, ProjectionMatrix);

        glm::mat4 WallModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(50, 165, 0.0));
        innerWall_2.Render(shader, WallModelViewMatrix, ProjectionMatrix);

        if(car.isInCollision(innerWall_2.getOBB())){
            velocity = COLLISION_SPEED;
        }
        if(npcCar.isInCollision(innerWall_2.getOBB())){
            npcVelocity = COLLISION_SPEED;
        }

        WallModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(133, 167, 0.0));
        innerWall_3.Render(shader, WallModelViewMatrix, ProjectionMatrix);

        if(car.isInCollision(innerWall_3.getOBB())){
            velocity = COLLISION_SPEED;
        }
        if(npcCar.isInCollision(innerWall_3.getOBB())){
            npcVelocity = COLLISION_SPEED;
        }

        if (car.isInCollision(wallBottom_2.getOBB()) || car.isInCollision(wallRight_2.getOBB()) ||
            car.isInCollision(wallTop_2.getOBB()) || car.isInCollision(wallLeft_2.getOBB())) {
            cout << "Collided" << endl;
            velocity = COLLISION_SPEED;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (npcCar.isInCollision(wallBottom_2.getOBB()) || npcCar.isInCollision(wallRight_2.getOBB()) ||
            npcCar.isInCollision(wallTop_2.getOBB()) || npcCar.isInCollision(wallLeft_2.getOBB())) {
            cout << "Collided" << endl;
            npcVelocity = COLLISION_SPEED;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        //Create a CarModelViewMatrix to transform the car to the correct cornerAngle,y
        glm::mat4 CarModelViewMatrix = ViewMatrix;
        CarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(car.getXPos(), car.getYPos(), 0.0));

        glm::mat4 NpcCarModelViewMatrix = ViewMatrix;
        NpcCarModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(npcCar.getXPos(), npcCar.getYPos(), 0.0));

        //Set default track position to 0
        float trackPosition = 0;
        float cornerAngle = 0;
        glm::mat4 TrackCornerModelViewMatrix = ViewMatrix;

        //Piece 1
        trackPosition = 4;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, -10.0, 0.0));
            float x = (M_PI / 2);
            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 1
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(108.0, -10.0, 0.0));
        cornerAngle = (M_PI);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 2
        trackPosition = 4;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(108, trackPosition, 0.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 2
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(108.0, 108.0, 0.0));
        cornerAngle = (0);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 3
        trackPosition = 122;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 108.0, 0.0));
            float x = (M_PI / 2);
            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 3
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(226.0, 108.0, 0.0));
        cornerAngle = (M_PI);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 4
        trackPosition = 122;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(226, trackPosition, 0.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 4
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(226.0, 226.0, 0.0));
        cornerAngle = (3*M_PI/2);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 5
        trackPosition = 122;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 226.0, 0.0));
            float x = (M_PI / 2);
            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 5
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(126.0, 226.0, 0.0));
        cornerAngle = (M_PI/2);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 6
        trackPosition = 240;
        for (int i = 0; i < 10; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(126, trackPosition, 0.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 6
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(126.0, 344.0, 0.0));
        cornerAngle = (3*M_PI/2);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 7
        trackPosition = 2;
        for (int i = 0; i < 12; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(trackPosition, 344.0, 0.0));
            float x = (M_PI / 2);
            TrackModelViewMatrix = glm::rotate(TrackModelViewMatrix, x, glm::vec3(0.0, 0.0, 1.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 7
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10.0, 344.0, 0.0));
        cornerAngle = (0);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);

        //Piece 8
        trackPosition = 0;
        for (int i = 0; i < 34; i++) {
            glm::mat4 TrackModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10, trackPosition, 0.0));
            trackRight.Render(shader, TrackModelViewMatrix, ProjectionMatrix);
            trackPosition += 10;
        }

        //Corner 9
        TrackCornerModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10.0, -10.0, 0.0));
        cornerAngle = (M_PI / 2);
        TrackCornerModelViewMatrix = glm::rotate(TrackCornerModelViewMatrix, cornerAngle, glm::vec3(0.0, 0.0, 1.0));
        trackCorner.Render(shader, TrackCornerModelViewMatrix, ProjectionMatrix);


        glm::mat4 StartFinishModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(20.0, -10.0, 1.0));
        StartFinishModelViewMatrix = glm::rotate(StartFinishModelViewMatrix, (float)M_PI/2, glm::vec3(0.0, 0.0, 1.0));
        start_finish.Render(shader, StartFinishModelViewMatrix, ProjectionMatrix);

        glm::mat4 Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(120, -10, 0.0));//
        marker1.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(105, 125, 0.0));//
        marker2.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(240, 110, 0.0));//
        marker3.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(230, 240, 0.0));//
        marker4.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(113, 230, 0.0));//
        marker5.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(120, 358, 0.0));//
        marker6.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-20, 345, 0.0));
        marker7.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10, -20, 0.0));
        marker8.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);

        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100, -10, 0.0));//
        carMarker1.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(125, 110, 0.0));//
        carMarker2.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(210, 110, 0.0));//
        carMarker3.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(210, 230, 0.0));//
        carMarker4.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(140, 230, 0.0));//
        carMarker5.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(120, 330, 0.0));//
        carMarker6.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0, 345, 0.0));
        carMarker7.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);
        Marker2ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-10, -10, 0.0));
        carMarker8.Render(shader, Marker2ModelViewMatrix, ProjectionMatrix);

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

        print(ProjectionMove, font, 20, 20, char_arr);
    }

    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void init() {

    currentLevel = 1;
    cout << "Current Level : " << currentLevel << endl;

    FreeImage_Initialise();

    glClearColor(0.4, 0.3, 0.2, 0.2);                        //sets the clear colour to black

    //Load the GLSL program
    if (!shader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag")) {
        std::cout << "failed to load shader" << std::endl;
    }

    float red[3] = {1, 0, 0};
    float blue[3] = {0, 0, 1};

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

    arrowKeys.setWidth(50);
    arrowKeys.setHeight(50);
    arrowKeys.init(shader, red, "keys.png");

    wallBottom.setWidth(250.0f);
    wallBottom.setHeight(5.0f);
    wallRight.setWidth(5.0f);
    wallRight.setHeight(250.0f);
    wallTop.setWidth(250.0f);
    wallTop.setHeight(5.0f);
    wallLeft.setWidth(5.0f);
    wallLeft.setHeight(250.0f);

    innerWall_3.setWidth(150);
    innerWall_3.setHeight(90);

    innerWall_3.init(shader, red, "roadTexture_26.png");

    innerWall_2.setWidth(85);
    innerWall_2.setHeight(325);


    innerWall_2.init(shader, red, "roadTexture_26.png");

    innerWall.setWidth(175);
    innerWall.setHeight(175);

    innerWall.init(shader, red, "roadTexture_26.png");

    wallRight.setXpos(225);
    wallRight.setYpos(100);
    wallBottom.setXpos(100);
    wallBottom.setYpos(-25);
    wallTop.setXpos(100);
    wallTop.setYpos(225);
    wallLeft.setXpos(-25);
    wallLeft.setYpos(100);

    wallBottom_2.setWidth(275.0f);
    wallBottom_2.setHeight(5.0f);
    wallRight_2.setWidth(5.0f);
    wallRight_2.setHeight(400.0f);
    wallTop_2.setWidth(275.0f);
    wallTop_2.setHeight(5.0f);
    wallLeft_2.setWidth(5.0f);
    wallLeft_2.setHeight(400.0f);

    wallRight_2.setXpos(250);
    wallRight_2.setYpos(175);
    wallBottom_2.setXpos(112);
    wallBottom_2.setYpos(-25);
    wallTop_2.setXpos(112);
    wallTop_2.setYpos(375);
    wallLeft_2.setXpos(-25);
    wallLeft_2.setYpos(175);

    trackUp.setWidth(10.0f * (500 / 264.0f));
    trackUp.setHeight(10.0f);
    trackRight.setWidth(10.0f * (500 / 264.0f));
    trackRight.setHeight(10.0f);

    trackCorner.setWidth(19.0f);
    trackCorner.setHeight(19.0f);
    trackCorner.init(shader, red, "roadTexture_02.png");

    start_finish.setWidth(19.0f);
    start_finish.setHeight(19.0f/2);
    start_finish.init(shader, red, "start_finish.png");

    wallBottom.init(shader, red, "roadTexture_26.png");
    wallRight.init(shader, red, "roadTexture_26.png");
    wallTop.init(shader, red, "roadTexture_26.png");
    wallLeft.init(shader, red, "roadTexture_26.png");

    wallBottom_2.init(shader, red, "roadTexture_26.png");
    wallRight_2.init(shader, red, "roadTexture_26.png");
    wallTop_2.init(shader, red, "roadTexture_26.png");
    wallLeft_2.init(shader, red, "roadTexture_26.png");

    car.init(shader, red, "car.png");
    npcCar.init(shader, red, "npc.png");
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

    marker6.setWidth(20);
    marker6.setHeight(20);

    marker7.setWidth(20);
    marker7.setHeight(20);

    marker8.setWidth(20);
    marker8.setHeight(20);

    marker1.SetXPos(1000);
    marker1.SetYPos(1000);
    marker2.SetXPos(0);
    marker2.SetYPos(0);
    marker3.SetXPos(0);
    marker3.SetYPos(0);
    marker4.SetXPos(0);
    marker4.SetYPos(0);
    marker5.SetXPos(0);
    marker5.SetYPos(0);
    marker6.SetXPos(0);
    marker6.SetYPos(0);
    marker7.SetXPos(0);
    marker7.SetYPos(0);
    marker8.SetXPos(0);
    marker8.SetYPos(0);

    carMarker1.setWidth(40);
    carMarker1.setHeight(40);

    carMarker2.setWidth(40);
    carMarker2.setHeight(40);

    carMarker3.setWidth(40);
    carMarker3.setHeight(40);

    carMarker4.setWidth(40);
    carMarker4.setHeight(40);

    carMarker5.setWidth(40);
    carMarker5.setHeight(40);

    carMarker6.setWidth(40);
    carMarker6.setHeight(40);
    carMarker7.setWidth(40);
    carMarker7.setHeight(40);
    carMarker8.setWidth(40);
    carMarker8.setHeight(40);


    carMarker1.SetXPos(0);
    carMarker1.SetYPos(0);
    carMarker2.SetXPos(0);
    carMarker2.SetYPos(0);
    carMarker3.SetXPos(0);
    carMarker3.SetYPos(0);
    carMarker4.SetXPos(0);
    carMarker4.SetYPos(0);
    carMarker5.SetXPos(0);
    carMarker5.SetYPos(0);
    carMarker6.SetXPos(0);
    carMarker6.SetYPos(0);
    carMarker7.SetXPos(0);
    carMarker7.SetYPos(0);
    carMarker8.SetXPos(0);
    carMarker8.SetYPos(0);

    npcMarker1.setWidth(20);
    npcMarker1.setHeight(20);
    npcMarker2.setWidth(20);
    npcMarker2.setHeight(20);
    npcMarker3.setWidth(20);
    npcMarker3.setHeight(20);
    npcMarker4.setWidth(20);
    npcMarker4.setHeight(20);
    npcMarker5.setWidth(20);
    npcMarker5.setHeight(20);
    npcMarker6.setWidth(20);
    npcMarker6.setHeight(20);
    npcMarker7.setWidth(20);
    npcMarker7.setHeight(20);
    npcMarker8.setWidth(20);
    npcMarker8.setHeight(20);


    marker1.init(shader, red, "transparent.png");
    marker2.init(shader, red, "transparent.png");
    marker3.init(shader, red, "transparent.png");
    marker4.init(shader, red, "transparent.png");
    marker5.init(shader, red, "transparent.png");
    marker6.init(shader, red, "transparent.png");
    marker7.init(shader, red, "transparent.png");
    marker8.init(shader, red, "transparent.png");

    carMarker1.init(shader, red, "transparent.png");
    carMarker2.init(shader, red, "transparent.png");
    carMarker3.init(shader, red, "transparent.png");
    carMarker4.init(shader, red, "transparent.png");
    carMarker5.init(shader, red, "transparent.png");
    carMarker6.init(shader, red, "transparent.png");
    carMarker7.init(shader, red, "transparent.png");
    carMarker8.init(shader, red, "transparent.png");

    npcMarker1.init(shader, red, "roadTexture_26.png");
    npcMarker2.init(shader, red, "roadTexture_26.png");
    npcMarker3.init(shader, red, "roadTexture_26.png");
    npcMarker4.init(shader, red, "roadTexture_26.png");
    npcMarker5.init(shader, red, "roadTexture_26.png");
    npcMarker6.init(shader, red, "roadTexture_26.png");
    npcMarker7.init(shader, red, "roadTexture_26.png");
    npcMarker8.init(shader, red, "roadTexture_26.png");

    markers[0] = marker1;
    markers[1] = marker2;
    markers[2] = marker3;
    markers[3] = marker4;
    markers[4] = marker5;


    font.init("fonts/VT323-Regular.ttf", 35);

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
            cout << "At Max Velocity" << endl;
        }
        else{
            //Increase velocity when holding up key
            velocity += ACCELERATION;
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

    if(key == 27){
        glutLeaveMainLoop();
    }
    else if(key == 13){
        renderMenu = false;
        renderFinish = false;
        resetGame();
        renderCountdown = true;
        countdownCount = 0;
    }
    else if(key == 49){
        currentLevel = 1;
        renderLevel2 = false;
        cout << "Level 1 Selected" << endl;
    }
    else if(key == 50){
        currentLevel = 2;
        renderLevel1 = false;
        cout << "Level 2 Selected" << endl;
    }
    else if(key == 109){
        renderLevel1 = false;
        renderLevel2 = false;
        resetGame();
        renderControlMenu = false;
        renderMenu = true;
    }
    else if(key == 99){
        cout << "Pressed Control Button" << endl;
        if(renderControlMenu){
            renderControlMenu = false;
            renderMenu = true;
        }
        else{
            renderControlMenu = true;
        }

    }
    else{
        cout << "Default" << endl;
    }

    /*switch(key){
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
        case 49:
            currentLevel = 1;
            cout << "Level 1 Selected" << endl;
        case 50:
            currentLevel = 2;
            cout << "Level 2 Selected" << endl;
        default:
            cout << "Default" << endl;
    }*/

    cout << "Current Level : " << currentLevel << endl;
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
    passed6 = false;
    passed7 = false;
    passed8 = false;

    npcCollisionCount = 0;
    npcCarFinished = false;
    npcVelocity = 0;
    npcPassed1 = false;
    npcPassed2 = false;
    npcPassed3 = false;
    npcPassed4 = false;
    npcPassed5 = false;
    npcPassed6 = false;
    npcPassed7 = false;
    npcPassed8 = false;

    timer = 0;

    cout << "Current Level : " << currentLevel << endl;
    cout << "Current Speed Level : " << MAX_VELOCITY << endl;
    cout << "Current Acceleration : " << ACCELERATION << endl;

}

void idle() {

    cout << "Current Level : " << currentLevel << endl;

    cout << "Time : " << timer << endl;

    if(renderMenu == false && renderFinish == false && renderCountdown == false && timer > 0){
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
        cout << "Car Marker 1";
    }
    else if(car.isInCollision((carMarker2.GetOBB()))){
        carCollisionCount = 2;
        passed2 = true;
        cout << "Car Marker 2";
    }
    else if(car.isInCollision((carMarker3.GetOBB()))){
        carCollisionCount = 3;
        passed3 = true;
        cout << "Car Marker 3";
    }
    else if(car.isInCollision((carMarker4.GetOBB()))){
        carCollisionCount = 4;
        passed4 = true;
        cout << "Car Marker 4";
    }
    else if(car.isInCollision((carMarker5.GetOBB()))){
        carCollisionCount = 5;
        passed5 = true;
    }
    else if(car.isInCollision((carMarker6.GetOBB()))){
        carCollisionCount = 6;
        passed6 = true;
    }
    else if(car.isInCollision((carMarker7.GetOBB()))){
        carCollisionCount = 7;
        passed7 = true;
    }
    else if(car.isInCollision((carMarker8.GetOBB()))){
        carCollisionCount = 8;
        passed8 = true;
    }
    else if(car.isInCollision((start_finish.GetOBB()))){
        carCollisionCount = 9;
        passedStart = true;
    }

    if(currentLevel == 1){
        if(carCollisionCount == 9 && passed1 && passed2 && passed3 && passed4 && passedStart){
            carFinished = true;
            cout << "Car finished" << endl;
            renderFinish = true;
            MAX_VELOCITY = MAX_VELOCITY + 1;
            ACCELERATION += 0.005;
            NPC_ADVANTAGE += 0.1;

            bool swapped = false;

            float storeTime = timer + 1;

            float newScore = (float)storeTime/60;

            if(newScore < bestScores2[0] || bestScores2[0] == 0){
                std::array<float,3> newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                newScores[0] = newScore;
                newScores[1] = bestScores2[0];
                newScores[2] = bestScores2[1];
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
            if(newScore < bestScores2[1] && swapped == false || bestScores2[1] == 0 && swapped == false){
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                std::array<float,3> newScores;
                newScores[0] = bestScores2[0];
                newScores[1] = newScore;
                newScores[2] = bestScores2[1];
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
            if(newScore < bestScores2[2] && swapped == false || bestScores2[2] == 0 && swapped == false){
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                std::array<float,3> newScores;
                newScores[0] = bestScores2[0];
                newScores[1] = bestScores2[1];
                newScores[2] = newScore;
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
        }
    }
    else if(currentLevel == 2){
        if(carCollisionCount == 9 && passed1 && passed2 && passed3 && passed4 && passed5 && passed6 && passed7 && passedStart){
            carFinished = true;
            cout << "Car finished" << endl;
            renderFinish = true;
            MAX_VELOCITY = MAX_VELOCITY + 1;
            ACCELERATION += 0.005;
            NPC_ADVANTAGE += 0.1;

            bool swapped = false;

            float storeTime = timer + 1;

            float newScore = (float)storeTime/60;

            if(newScore < bestScores2[0] || bestScores2[0] == 0){
                std::array<float,3> newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                newScores[0] = newScore;
                newScores[1] = bestScores2[0];
                newScores[2] = bestScores2[1];
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
            if(newScore < bestScores2[1] && swapped == false || bestScores2[1] == 0 && swapped == false){
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                std::array<float,3> newScores;
                newScores[0] = bestScores2[0];
                newScores[1] = newScore;
                newScores[2] = bestScores2[1];
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
            if(newScore < bestScores2[2] && swapped == false || bestScores2[2] == 0 && swapped == false){
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                std::array<float,3> newScores;
                newScores[0] = bestScores2[0];
                newScores[1] = bestScores2[1];
                newScores[2] = newScore;
                bestScores2 = newScores;
                cout << bestScores2[0] << bestScores2[1] << bestScores2[2] << endl;
                swapped = true;
            }
        }
    }


}

void processNpc() {

    if(npcVelocity < MAX_VELOCITY * NPC_ADVANTAGE){
        npcVelocity += ACCELERATION;
    }
    else{
        npcVelocity = MAX_VELOCITY * NPC_ADVANTAGE;
        cout << "Npc at Max Velocity" << endl;
    }

    if(npcCar.isInCollision(marker1.GetOBB())){
        npcCollisionCount = 1;
        npcVelocity = COLLISION_SPEED;
        npcPassed1 = true;
        cout << "Marker 1" << endl;
    }
    else if(npcCar.isInCollision((marker2.GetOBB()))){
        npcCollisionCount = 2;
        npcVelocity = COLLISION_SPEED;
        npcPassed2 = true;
        cout << "Marker 2" << endl;
    }
    else if(npcCar.isInCollision((marker3.GetOBB()))){
        npcCollisionCount = 3;
        npcVelocity = COLLISION_SPEED;
        npcPassed3 = true;
        cout << "Marker 3" << endl;
    }
    else if(npcCar.isInCollision((marker4.GetOBB()))){
        npcCollisionCount = 4;
        npcVelocity = COLLISION_SPEED;
        npcPassed4 = true;
        cout << "Marker 4" << endl;
    }
    else if(npcCar.isInCollision((marker5.GetOBB()))){
        npcCollisionCount = 5;
        npcVelocity = COLLISION_SPEED;
        npcPassed5 = true;
        cout << "Marker 5" << endl;
    }
    else if(npcCar.isInCollision((marker6.GetOBB()))){
        npcCollisionCount = 6;
        npcVelocity = COLLISION_SPEED;
        npcPassed6 = true;
        cout << "Marker 6" << endl;
    }
    else if(npcCar.isInCollision((marker7.GetOBB()))){
        npcCollisionCount = 7;
        npcVelocity = COLLISION_SPEED;
        npcPassed7 = true;
        cout << "Marker 7" << endl;
    }
    else if(npcCar.isInCollision((marker8.GetOBB()))){
        npcCollisionCount = 8;
        npcVelocity = COLLISION_SPEED;
        npcPassed8 = true;
        cout << "Marker 8" << endl;
    }
    else if(npcCar.isInCollision((start_finish.GetOBB()))){
        npcCollisionCount = 9;
        npcVelocity = COLLISION_SPEED;
        npcPassedStart = true;
        cout << "Marker 9" << endl;
    }
    else{
        cout << "No collisions" << endl;
    }

    //M_PI = Down
    //M_PI/2 = Left
    //0 = Up
    //3*M_PI/2 = Right

    cout << "Current Level : " << currentLevel << endl;

    if(currentLevel == 1){
        if(npcCollisionCount == 0){
            currentAngleNpc = 3*M_PI/2;
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
        else if(npcCollisionCount == 9){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(npcVelocity, 0);
            cout << "Start/Finish" << endl;
        }
        else{
            cout << "NPC collision count : " << npcCollisionCount << endl;
        }

        //Win condition for npc
        if(npcCollisionCount == 9 && npcPassed1 && npcPassed2 && npcPassed3 && npcPassed4){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(0, 0);
            npcCarFinished = true;
            cout << "Npc Finished" << endl;
            renderFinish = true;
        }
    }
    else if(currentLevel == 2){

        cout << "NPC collision count : " << npcCollisionCount << endl;
        if(npcCollisionCount == 0){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(npcVelocity, 0);
            cout << "Section 1" << endl;
        }
        else if(npcCollisionCount == 1){
            currentAngleNpc = 0;
            npcCar.IncPos(0, npcVelocity);
            cout << "Section 2" << endl;
        }
        else if(npcCollisionCount == 2){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(npcVelocity, 0);
            cout << "Section 3" << endl;
        }
        else if(npcCollisionCount == 3){
            currentAngleNpc = 0;
            npcCar.IncPos(0, npcVelocity);
            cout << "Section 4" << endl;
        }
        else if(npcCollisionCount == 4){
            currentAngleNpc = M_PI/2;
            npcCar.IncPos(-npcVelocity, 0);
            cout << "Section 5" << endl;
        }
        else if(npcCollisionCount == 5){
            currentAngleNpc = 0;
            npcCar.IncPos(0, npcVelocity);
            cout << "Section 5" << endl;
        }
        else if(npcCollisionCount == 6){
            currentAngleNpc = M_PI/2;
            npcCar.IncPos(-npcVelocity, 0);
            cout << "Section 6" << endl;
        }
        else if(npcCollisionCount == 7){
            currentAngleNpc = M_PI;
            npcCar.IncPos(0, -npcVelocity);
            cout << "Section 7" << endl;
        }
        else if(npcCollisionCount == 8){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(npcVelocity, 0);
            cout << "Section 8" << endl;
        }
        else if(npcCollisionCount == 9){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(npcVelocity, 0);
        }
        else{
            cout << "NPC collision count : " << npcCollisionCount << endl;
        }

        //Win condition for npc
        if(npcCollisionCount == 9 && npcPassed1 && npcPassed2 && npcPassed3 && npcPassed4 && npcPassed5 && npcPassed6 && npcPassed7 && npcPassed8 && npcPassedStart){
            currentAngleNpc = 3*M_PI/2;
            npcCar.IncPos(0, 0);
            npcCarFinished = true;
            cout << "Npc Finished" << endl;
            renderFinish = true;
        }
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
    if(npcVelocity > MAX_VELOCITY * NPC_ADVANTAGE){
        npcVelocity = MAX_VELOCITY * NPC_ADVANTAGE;
        cout << "Npc at max velocity" << endl;
    }
    else{
        //Increase velocity when holding up key
        npcVelocity += ACCELERATION;
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
