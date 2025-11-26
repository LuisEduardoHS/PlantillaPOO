#pragma once
#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include "Base/model.h"
#include "Billboard2D.h"
#include <vector>

class Principal : public Model {
private:
    double tiempoUltimoGolpe = 0;

public:
    std::vector<Billboard2D*> vidas;
    bool invulnerable = false;

    Principal(string const& path, Camera* camera) : Model(path, camera) {
        asignarvidas();
    }

    void asignarvidas() {
        glm::vec3 scale = glm::vec3(50.0f, 50.0f, 0.0f);

        // Creamos 3 corazones
        for (int i = 0; i < 3; i++) {
            float xPos = 50.0f + (i * 60.0f);
            Billboard2D* corazon = new Billboard2D((WCHAR*)L"textures/corazon.webp", 6, 6, xPos, 100, 0, this->cameraDetails);
            corazon->setScale(&scale);
            vidas.push_back(corazon);
        }
    }

    void recibirDano(double tiempoActual) {
        if (tiempoActual - tiempoUltimoGolpe > 2000.0) {
            if (!vidas.empty()) {
                delete vidas.back();
                vidas.pop_back();

                tiempoUltimoGolpe = tiempoActual;

                std::cout << "AUCH! Vidas restantes: " << vidas.size() << std::endl;
            }
        }
    }

    void reiniciar() {
        for (Billboard2D* vida : vidas) {
            delete vida;
        }
        vidas.clear();

        asignarvidas();

        tiempoUltimoGolpe = 0;

        std::cout << "JUGADOR REINICIADO: Vidas completas." << std::endl;
    }

    void DrawUI() {
        for (Billboard2D* vida : vidas) {
            vida->Draw();
        }
    }

    void Draw() override {
        Model::Draw();
    }

    // Destructor
    ~Principal() {
        for (Billboard2D* vida : vidas) delete vida;
        vidas.clear();
    }
};

#endif