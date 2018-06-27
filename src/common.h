// common.h
#pragma once
// the color palette
const float RED[] = { 242.0f/255.0f, 83.0f/255.0f, 70.0f/255.0f };
const float WHITE[] = { 216.0f/255.0f, 208.0f/255.0f, 209.0f/255.0f };
const float BROWN[] = { 89.0f/255.0f, 51.0f/255.0f, 46.0f/255.0f };
const float PINK[] = { 245.0f/255.0f, 152.0f/255.0f, 110.0f/255.0f };
const float BROWNDARK[] = { 35.0f/255.0f, 25.0f/255.0f, 15.0f/255.0f };
const float BLUE[] = { 104.0f/255.0f, 195.0f/255.0f, 192.0f/255.0f };

const float BACKGROUND_COLOR1[] = { 228.0f/255.0f, 224.0f/255.0f, 186.0f/255.0f };
const float BACKGROUND_COLOR2[] = { 247.0f/255.0f, 217.0f/255.0f, 170.0f/255.0f };

extern int WIDTH, HEIGHT, ACTUAL_WIDTH, ACTUAL_HEIGHT;
extern float TIMER;

extern float AMBIENT_LIGHT_INTENSITY;
const float NEAR_PLANE = 1.0f, FAR_PLANE = 10000.0f;

namespace GAME {
  extern float SPEED;
  extern float FPS;
  extern int DISPLAY_FPS;
  extern float AIRPLANE_DISTANCE;
  extern float MILES;
  extern float HEALTH;
};

namespace SEA {
  extern float RADIUS;
  extern float HEIGHT;
  extern int RADIAL_SEGMENTS;
  extern int HEIGHT_SEGMENTS;
  extern float MIN_AMPLITUDE;
  extern float MAX_AMPLITUDE;
  extern float MIN_SPEED;
  extern float MAX_SPEED;
};

namespace CAMERA {
  extern float X;
  extern float Y;
  extern float Z;
  extern float FOV;
  extern float ZOOM_SENSITIVITY;
};

namespace LIGHT {
  extern float X;
  extern float Y;
  extern float Z;
};

namespace SHADOW {
  extern float NEAR_PLANE;
  extern float FAR_PLANE;
  extern int WIDTH;
  extern int HEIGHT;
};

namespace AIRPLANE {
  extern float X;
  extern float Y;
  extern float Z;
  extern float MOVE_SENSITIVITY;
  extern float ROTATE_SENSITITY;
  extern float AMPWIDTH;
  extern float AMPHEIGHT;
};

extern float COLLISION_SPEED_X;
extern float COLLISION_SPEED_Y;
extern float COLLISION_DISPLACEMENT_X;
extern float COLLISION_DISPLACEMENT_Y;

class Entity;
extern Entity* SEA_MODEL;