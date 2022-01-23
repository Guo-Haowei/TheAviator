// Parser.cc
#include "Parser.h"
#include <common.h>
#include <fstream>
#include <iostream>
#include <string>
using std::cout;
using std::ifstream;
using std::string;

float AMBIENT_LIGHT_INTENSITY = 1.0f;

float GAME::SPEED;
float GAME::FPS;
int GAME::DISPLAY_FPS;

float SEA::RADIUS;
float SEA::HEIGHT;
int SEA::RADIAL_SEGMENTS;
int SEA::HEIGHT_SEGMENTS;
float SEA::MIN_AMPLITUDE;
float SEA::MAX_AMPLITUDE;
float SEA::MIN_SPEED;
float SEA::MAX_SPEED;

float CAMERA::X;
float CAMERA::Y;
float CAMERA::Z;
float CAMERA::FOV;
float CAMERA::ZOOM_SENSITIVITY;

float LIGHT::X;
float LIGHT::Y;
float LIGHT::Z;

float SHADOW::NEAR_PLANE;
float SHADOW::FAR_PLANE;
int SHADOW::WIDTH;
int SHADOW::HEIGHT;

float AIRPLANE::X;
float AIRPLANE::Y;
float AIRPLANE::Z;
float AIRPLANE::MOVE_SENSITIVITY;
float AIRPLANE::ROTATE_SENSITITY;
float AIRPLANE::AMPWIDTH;
float AIRPLANE::AMPHEIGHT;

// helper
void getNextFloat(ifstream& file, float* value)
{
    string word;
    while(file >> word)
    {
        if((word[0] >= '0' && word[0] <= '9') || word[0] == '-' || word[0] == '.')
        {
            *value = std::stof(word);
            return;
        }
    }
}

void getNextInt(ifstream& file, int* value)
{
    string word;
    while(file >> word)
    {
        if((word[0] >= '0' && word[0] <= '9') || word[0] == '-')
        {
            *value = std::stoi(word);
            return;
        }
    }
}

void Parser::parse()
{
    ifstream configFile;
    configFile.open("../config.txt");
    if(!configFile.is_open())
    {
        std::cout << "==================================================\n";
        std::cout << "ERROR::PARSER: Failed to open file ../config.txt\n";
        std::cout << "\n==================================================\n";
        return;
    }
    getNextFloat(configFile, &GAME::SPEED);
    getNextFloat(configFile, &GAME::FPS);
    getNextInt(configFile, &GAME::DISPLAY_FPS);

    getNextFloat(configFile, &SEA::RADIUS);
    getNextFloat(configFile, &SEA::HEIGHT);
    getNextInt(configFile, &SEA::RADIAL_SEGMENTS);
    getNextInt(configFile, &SEA::HEIGHT_SEGMENTS);
    getNextFloat(configFile, &SEA::MIN_AMPLITUDE);
    getNextFloat(configFile, &SEA::MAX_AMPLITUDE);
    getNextFloat(configFile, &SEA::MIN_SPEED);
    getNextFloat(configFile, &SEA::MAX_SPEED);

    getNextFloat(configFile, &CAMERA::X);
    getNextFloat(configFile, &CAMERA::Y);
    getNextFloat(configFile, &CAMERA::Z);
    getNextFloat(configFile, &CAMERA::FOV);
    getNextFloat(configFile, &CAMERA::ZOOM_SENSITIVITY);

    getNextFloat(configFile, &LIGHT::X);
    getNextFloat(configFile, &LIGHT::Y);
    getNextFloat(configFile, &LIGHT::Z);

    getNextFloat(configFile, &SHADOW::NEAR_PLANE);
    getNextFloat(configFile, &SHADOW::FAR_PLANE);
    getNextInt(configFile, &SHADOW::WIDTH);
    getNextInt(configFile, &SHADOW::HEIGHT);

    getNextFloat(configFile, &AIRPLANE::X);
    getNextFloat(configFile, &AIRPLANE::Y);
    getNextFloat(configFile, &AIRPLANE::Z);
    getNextFloat(configFile, &AIRPLANE::MOVE_SENSITIVITY);
    getNextFloat(configFile, &AIRPLANE::ROTATE_SENSITITY);
    getNextFloat(configFile, &AIRPLANE::AMPWIDTH);
    getNextFloat(configFile, &AIRPLANE::AMPHEIGHT);
}
