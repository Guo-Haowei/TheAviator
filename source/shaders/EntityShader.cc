// EntityShader.cc
#include "EntityShader.h"
#include "glPrerequisites.h"
#include <common.h>
#include <entities/DynamicEntity.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Camera.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

using std::cout;
using std::vector;

EntityShader::EntityShader()
{
    const char* VERTEX_FILE = "../shaders/entity.vert";
    const char* FRAGMENT_FILE = "../shaders/entity.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void EntityShader::bindAttributes()
{
    bindAttribute(0, "position");
    bindAttribute(1, "normal");
}

void EntityShader::getAllUniformLocations()
{
    ShaderProgram::getAllUniformLocations();
    location_M = getUniformLocation("M");
    location_V = getUniformLocation("V");
    location_P = getUniformLocation("P");
    location_color = getUniformLocation("color");
    location_light = getUniformLocation("lightPos");
}

void EntityShader::render()
{
    start();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glm::vec3 lightPos(Light::theOne().getPosition());
    loadVector3f(location_light, lightPos);

    // constexpr glm::vec3 eye(0, 200, 100);
    constexpr glm::vec3 eye(0, 0, 100);
    const glm::mat4 V = glm::lookAt(eye, eye + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    const float fovy = glm::radians(60.0f);
    const float aspect = (float)WIDTH / HEIGHT;
    const glm::mat4 P = glm::perspective(fovy, aspect, 1.0f, 10000.0f);

    loadMatrix4f(location_V, V);
    loadMatrix4f(location_P, P);

    for(auto& entry : staticEntities)
    {
        vector<Entity*>& entities = entry.second;
        entry.first->bind();
        for(int i = 0; i < entities.size(); ++i)
        {
            Entity* entity = entities.at(i);
            RawModel* model = entity->getModel();
            loadVector3f(location_color, entity->getColor());

            const auto M = entity->getTransformationMatrix();

            // printf("%f %f %f %f\n", M[0].x, M[1].x, M[2].x, M[3].x);
            // printf("%f %f %f %f\n", M[0].y, M[1].y, M[2].y, M[3].y);
            // printf("%f %f %f %f\n", M[0].z, M[1].z, M[2].z, M[3].z);
            // printf("%f %f %f %f\n", M[0].w, M[1].w, M[2].w, M[3].w);

            loadMatrix4f(location_M, entity->getTransformationMatrix());

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
        RawModel::unbind();
    }
    // for(auto& entry : dynamicEntities)
    // {
    //     vector<DynamicEntity*>& entities = entry.second;
    //     entry.first->bind();
    //     for(int i = 0; i < entities.size(); ++i)
    //     {
    //         DynamicEntity* entity = entities.at(i);
    //         RawModel* model = entity->getModel();
    //         loadVector3f(location_color, entity->getColor());
    //         loadMatrix4f(location_M, entity->getTransformationMatrix());

    //         glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
    //     }
    //     RawModel::unbind();
    // }

    // vector<DynamicEntity*>& particles = ParticleHolder::getParticles();
    // if(particles.size())
    //     particles[0]->getModel()->bind();
    // for(int i = 0; i < particles.size(); ++i)
    // {
    //     DynamicEntity* particle = particles.at(i);
    //     RawModel* model = particle->getModel();
    //     loadBool(location_receiveShadow, particle->getReceiveShadow());
    //     loadFloat(location_opacity, particle->getOpacity());
    //     loadVector3f(location_color, particle->getColor());
    //     loadMatrix4f(location_transformationMatrix, particle->getTransformationMatrix());
    //     loadMatrix4f(location_prevPVM, PV * particle->getPrevTransformationMatrix());

    //     glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
    // }
    RawModel::unbind();
    stop();
}
