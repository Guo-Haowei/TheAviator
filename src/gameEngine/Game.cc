// Game.cc
#include "Game.h"
#include "Collision.h"
#include <common.h>
#include <maths/Maths.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Sky.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/ObstacleHolder.h>
#include <entities/gameObjects/BatteryHolder.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <entities/gameObjects/Camera.h>
#include <models/Geometry.h>
#include <renderEngine/DisplayManager.h>
#include <io/MouseManager.h>
#include <io/Parser.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
using std::cout;

float GAME::AIRPLANE_DISTANCE = 0.0f;
float TIMER = 0;

/* Helper function declaration */
void updateFPSCount(double& previousSecond, int& updates);

Game::Game() {
  currentTime = 0;
  lastTime = DisplayManager::getTime();
  previousSecond = lastTime;
  delta = 0;
  updates = 0;
}

Game::~Game() {
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
}

void Game::init() {
  Parser::parse();
  DisplayManager::createDisplay();
  Geometry::initGeometry();
}

Game& Game::theOne() {
  static Game game;
  return game;
}

void Game::run() {
    if (shouldUpdate()) {
      // temporary code for updating game angle
      GAME::AIRPLANE_DISTANCE += GAME::SPEED;

      MouseManager::update();
      Camera::primary().update();
      DisplayManager::prepareDisplay();

      // update light intensity
      AMBIENT_LIGHT_INTENSITY = std::max(1.0f, AMBIENT_LIGHT_INTENSITY - 0.05f);
      // check collision
      Collision::checkCollisionAgainstPlane();
      ParticleHolder::theOne().update();

      renderer.render();

      ObstacleHolder::theOne().update();
      BatteryHolder::theOne().update();
      Airplane::theOne().update();
      Sky::theOne().update();
      DisplayManager::updateDisplay();
      ++updates;
    }

    if (GAME::DISPLAY_FPS)
      updateFPSCount(previousSecond, updates);
}

bool Game::shouldRun() {
  return !DisplayManager::shouldCloseDisplay();
}

bool Game::shouldUpdate() {
  currentTime = DisplayManager::getTime();
  delta += currentTime - lastTime;
  lastTime = currentTime;
  if (delta >= 1.0 / GAME::FPS) {
    delta -= 1.0/ GAME::FPS;
    ++TIMER;
    return true;
  } else {
    return false;
  }
}

/* Helper function implementation */

void updateFPSCount(double& previousSecond, int& updates) {
  if (DisplayManager::getTime() - previousSecond < 1.0) {
    return;
  }

  ++previousSecond;
  cout << "FPS: " << updates << "\n";
  updates = 0;
}