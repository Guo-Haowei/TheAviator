// common.h
#pragma once
// the color palette
const float RED[] = { 242/255, 83/255, 70/255 };
const float WHITE[] = { 216/255, 208/255, 209/255 };
const float BROWN[] = { 89/255, 51/255, 46/255 };
const float PINK[] = { 245/255, 152/255, 110/255 };
const float BROWNDARK[] = { 35/255, 25/255, 15/255 };
const float BLUE[] = { 104/255, 195/255, 192/255 };

const float BACKGROUND_COLOR1[] = { 228/255, 224/255, 186/255 };
const float BACKGROUND_COLOR2[] = { 247/255, 217/255, 170/255 };

extern const int WIDTH, HEIGHT;
extern int ACTUAL_WIDTH, ACTUAL_HEIGHT;

const float NEAR_PLANE = 0.1f, FAR_PLANE = 1000.0f, ZOOM = 45.0f;
