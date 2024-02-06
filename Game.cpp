//
//  Game.cpp
//  3Dlabs
//
//  Created by AHMED HAMED on 01/12/2023.
//


//#include <SDL2/SDL.h>
#include <sstream>

#include "Sound.h"


#include "helper.h"

#include "setUp.h"

#include "Environment1.h"
#include "Environment2.h"

#include "GameVariables.h"
#include "Weapon.h"

bool environment1=true;
float lightPosition=-100.0f;
float lightIntensity=0.1;
bool darker=true;
Weapon weapon;


// Lighting Configuration Function
//=======================================================================
void InitSunLightSource()
{
    // Enable Lighting for this OpenGL Program
    glEnable(GL_LIGHTING);

    // Enable Light Source number 0
    // OpengL has 8 light sources
    glEnable(GL_LIGHT0);

    // Define Light source 0 ambient light
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    // Define Light source 0 diffuse light
    GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    // Define Light source 0 Specular light
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Define Light source 0 as a directional light
    GLfloat direction[] = { 0.0f, -1.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, direction);

    // Finally, define light source 0 position in World Space
    float z=worldLength;
    float y =10;
    GLfloat light_position[] = { lightPosition, y, -z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light_Intensity[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_Intensity);
}


//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
    // Enable Material Tracking
//    glEnable(GL_COLOR_MATERIAL);

    // Sich will be assigneet Material Properties whd by glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Set Material's Specular Color
    // Will be applied to all objects
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

    // Set Material's Shine value (0->128)
    GLfloat shininess[] = { 96.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}



void lightTimer(int value){
    if(lightPosition<-worldLength){
        darker=false;
    }
    else if(lightPosition>worldLength){
        darker=true;
    }
    
    float lightPosDiff=5;
    float lightIntensityDiff=0.01;
    
    if(darker){
        lightPosition-=lightPosDiff;
        lightIntensity-=lightIntensityDiff;
    }else{
        lightPosition+=lightPosDiff;
        lightIntensity+=lightIntensityDiff;
    }
    InitSunLightSource();
    glutPostRedisplay();
    glutTimerFunc(16, lightTimer, 0); // 60 frames per second
}

void init(){
    InitSunLightSource();
    InitMaterial();
}



void Display() {
    setupCamera();
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(gameEnded){
        if(gameWon){
            drawWiningScene();
        }
        else{
            drawLosingScene();
        }
    }
    else{
        
        if(environment1){
            drawEnvironment1();
        }
        else{
            drawEnvironment2();
        }
        
        // Use std::ostringstream for string formatting
        std::ostringstream oss1;
        oss1 << "Score: " << gameScore;
        glDisable(GL_LIGHTING);
        drawText(oss1.str().c_str(),  glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)-20, 0);
        std::ostringstream oss2;
        oss2 << "Lives: " << human.lives;
        drawText(oss2.str().c_str(), 22, glutGet(GLUT_WINDOW_HEIGHT)-20, 0);
        glEnable(GL_LIGHTING);
    }
    
    
    
    glutSwapBuffers();
}




void gameTimer(int value){
    if(!gameEnded){
        
        zombieTimer(value); //moves the zombie(s) towards the player
        glutPostRedisplay();
        glutTimerFunc(16, gameTimer, 0); // 60 frames per second
    }

}


void gameKeyboard(unsigned char key, int x, int y){
    if(key=='m' || key=='M'){
        thirdPerson=!thirdPerson;
        return;
    }
    
    if(key=='b' || key=='B'){ 
        human.hasWeapon=!human.hasWeapon;
        return;
    }
    if(human.hasWeapon && (key=='v' ||key=='V')){
        weapon.shoot();
    }
    
    Keyboard(key, x, y); //no need for this this is the old camera functions
    zombieKeyboard(key);
    human.move(key,0,0,0,1,worldLength-7,&environment1,portal,house);
    glutPostRedisplay();
    
}
void gameMouseMotion(int x, int y) {
//    human.rotateWithMouse(x, y, windowWidth, windowHeight);
    human.rotateWithMouse(x);
    cout<<"mouse function triggereded"<<"\n";
    glutPostRedisplay();  // Trigger a redraw to update the human's orientation
}


void Idle() {
    // This function is called when the application is idle
    // Check conditions to play sound (e.g., collision, event, etc.)
    // If conditions are met, play the sound
    
}


int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(nullptr)));
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
        }
    
    
//    camera = Camera(0,12,50);
    glutInit(&argc, argv);
  

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("game");
    glutDisplayFunc(Display);
//    glutKeyboardFunc(Keyboard);
    glutKeyboardFunc(gameKeyboard);
    glutSpecialFunc(Special);
    glutMotionFunc(gameMouseMotion);
    glutTimerFunc(0, gameTimer, 0);
    glutTimerFunc(0, lightTimer, 0);
   

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    init();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

//    glutIdleFunc(Idle);  // Add an idle function to continuously check for sound playback

    glutMainLoop();
}










