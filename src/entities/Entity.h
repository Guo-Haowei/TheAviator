// Entity.h
#pragma once
#include <models/RawModel.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

struct Object3D;

class Entity {
protected:
  unsigned int id;
  RawModel* model;
  float opacity;
  bool receiveShadow;
  bool castShadow;
  glm::vec3 position, color, scale;
  glm::mat4 transformation, prevTransformation;
  Object3D* rigidBody;

  void updateTransformation(glm::mat4 transformationMatrix);
public:
  Entity();
  Entity(const Entity& other);
  Entity(RawModel* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f), glm::vec3 scale = glm::vec3(1.0f), float opacity = 1.0f, bool receiveShadow = true, bool castShadow = true);
  virtual ~Entity();

  glm::vec3 getPosition() const;
  void setPosition(float dx, float dy, float dz);
  void changePosition(glm::mat4 translationMatrix);
  void changePosition(float dx, float dy, float dz);

  void changeRotation(float x, float y, float z);
  void changeRotation(glm::mat4 rotationMatrix);
  void changeRotation(glm::vec3 axis, float angle);
  void changeRotation(glm::vec3 axis, float angle, glm::vec3 center);

  glm::vec3 getScale() const;
  void setScale(float dx, float dy, float dz);

  glm::mat4 getTransformationMatrix() const;
  glm::mat4 getPrevTransformationMatrix() const;
  glm::vec4 getWorldPos() const;
  void updatePrevTransformation();

  float getOpacity() const;
  RawModel* getModel() const;
  glm::vec3 getColor() const;
  void setColor(glm::vec3 color);
  bool getReceiveShadow() const;
  bool getCastShadow() const;
  unsigned int getId() const;
  Object3D* getBody();
  void setBody(Object3D* body);

  static void addEntity(Entity* entity);
};

typedef std::map<RawModel*, std::vector<Entity*>> StaticEntities;
extern StaticEntities staticEntities;
