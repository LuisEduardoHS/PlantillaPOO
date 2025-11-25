#pragma once

#ifndef VILLANO_H
#define VILLANO_H
#include "Base/model.h"
#include "Base/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

class Villano : public Model {
private:
    float speed = 0.15f;

public:
    Villano(string const& path, Camera* camera) : Model(path, camera) {
        name = "Villano";
    }

    void update(double deltaTime, glm::vec3 playerPos, float terrenoY) {
        glm::vec3* pos = getTranslate();
        glm::vec3* vel = getVelocity();

        float distancia = glm::distance(*pos, playerPos);
        glm::vec3 direccion = playerPos - *pos;

        // Rango de Persecucion: Si esta a menos de 80 metros y a mas de 2 (para no fusionarse)
        if (distancia < 80.0f && distancia > 2.0f) {

            direccion = glm::normalize(direccion);

            vel->x = direccion.x * speed;
            vel->z = direccion.z * speed;

            float rotY = atan2(direccion.x, direccion.z);
            float grados = glm::degrees(rotY);

            setRotY(grados);
            setNextRotY(grados);

            setAnimation(1); // Walk
        }
        else {
            // Si el jugador esta lejos, se detiene
            vel->x = 0;
            vel->z = 0;
            
            setAnimation(0); // Idle
        }

        pos->x += vel->x * (deltaTime / 10.0f); // Ajuste de velocidad
        pos->z += vel->z * (deltaTime / 10.0f);

        pos->y = terrenoY;

        // Actualizamos los punteros del motor
        setTranslate(pos);
        setNextTranslate(pos);
    }
};

#endif