#pragma once
#include "Geometry.h"
#include "Node.h"

#define MESH_KEY_AIRPLANE_BODY      "$mesh.airplane.body"
#define MESH_KEY_AIRPLANE_HAIR      "$mesh.airplane.hair"
#define MESH_KEY_AIRPLANE_PROPELLER "$mesh.airplane.propeller"
#define MESH_KEY_OCEAN              "$mesh.ocean"

#define SCENE_KEY_ROOT               "$scene.root"
#define SCENE_KEY_AIRPLANE           "$scene.airplane"
#define SCENE_KEY_AIRPLANE_BODY      "$scene.airplane.body"
#define SCENE_KEY_AIRPLANE_HAIR      "$scene.airplane.hair"
#define SCENE_KEY_AIRPLANE_PROPELLER "$scene.airplane.propeller"
#define SCENE_KEY_OCEAN              "$scene.ocean"

void InitMeshes();

void SetupScene();

MeshGroup* FindMesh( const char* key );

extern Node g_scene;
