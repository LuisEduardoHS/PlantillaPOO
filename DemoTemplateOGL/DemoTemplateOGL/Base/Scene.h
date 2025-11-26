#pragma once
#include "Base/model.h"
#include "SkyDome.h"
#include "Terreno.h"
#include "Billboard.h"
#include "BillboardAnimation.h"
#include "Billboard2D.h"
#include "Texto.h"
#include "Water.h"
#include "Animator.h"
#include "Animation.h"
#include "../Recolectable.h"
#include "../InputDevices/KeyboardInput.h"
#include "../Villano.h"
#include "../Principal.h"
#include <string>

class Scene {
	public:
        int estadoJuego = 0; // 0 = JUGANDO, 1 = GAME OVER, 2 = VICTORIA
        bool mensajeMostrado = false;
        float tiempoEnAgua = 0.0f;

		virtual float getAngulo() = 0;
		virtual void setAngulo(float angulo) = 0;
		virtual Model* getMainModel() = 0;
		virtual void setMainModel(Model* mainModel) = 0;
		virtual Scene* Render() = 0;
		virtual SkyDome* getSky() = 0;
		virtual Terreno* getTerreno() = 0;
		virtual std::vector<Model*> *getLoadedModels() = 0;
		virtual std::vector<Billboard*> *getLoadedBillboards() = 0;
		virtual std::vector<BillboardAnimation*> *getLoadedBillboardsAnimation() = 0;
		virtual std::vector<Billboard2D*> *getLoadedBillboards2D() = 0;
		virtual std::vector<Texto*> *getLoadedText() = 0;
		virtual ~Scene(){
		};

        void calcularPuntaje(int& recogidos, int& totales) {
            recogidos = 0;
            totales = 0;
            for (Model* m : *getLoadedModels()) {
                if (Recolectable* r = dynamic_cast<Recolectable*>(m)) {
                    totales++;
                    if (r->fueRecogido) recogidos++;
                }
            }
        }

        virtual int update() {

            if (estadoJuego != 0) {
                // --- DE FIN DE JUEGO ---
                if (!mensajeMostrado) {
                    if (estadoJuego == 1) {
                        INFO("HAS MUERTO.\n\nPresiona ACEPTAR y luego ENTER para reiniciar.", "GAME OVER");
                    }
                    else if (estadoJuego == 2) {
                        INFO("MISION CUMPLIDA: Todas las pruebas recolectadas.\n\nPresiona ACEPTAR y luego ENTER para reiniciar.", "VICTORIA");
                    }
                    mensajeMostrado = true;
                }

                // --- REINICIO CON TECLA ENTER ---
                if (KEYS[input.Enter]) {
                    reiniciarJuego();
                }

                return -1; // Detenemos
            }

            float angulo = getAngulo() + 0.1 * gameTime.deltaTime / 100; // SkyDome rotation
            angulo = angulo >= 360 ? angulo - 360.0 : angulo;
            setAngulo(angulo);
            getSky()->setRotY(angulo);

            Model* camara = getMainModel();

            int itemsRecogidos = 0;
            int itemsTotales = 0;

            for (int i = 0; i < getLoadedModels()->size(); i++) {

                auto it = getLoadedModels()->begin() + i;
                Model* collider = NULL, * model = *it;

                // Recolectables Animation
                if (Recolectable* item = dynamic_cast<Recolectable*>(model)) {
                    item->animar();
                }

                // Villano AI Update
                if (Villano* enemigo = dynamic_cast<Villano*>(model)) {
                    float terrenoY = getTerreno()->Superficie(enemigo->getTranslate()->x, enemigo->getTranslate()->z);

                    enemigo->update(gameTime.deltaTime, *camara->getTranslate(), terrenoY);

                    // Ataque al jugador si esta cerca
                    float distAtaque = glm::distance(*camara->getTranslate(), *enemigo->getTranslate());

                    if (distAtaque < 3.0f) {
                        if (Principal* jugador = dynamic_cast<Principal*>(camara)) {
                            jugador->recibirDano(get_nanos() / 1000000.0);
                        }
                    }
                }

                // Actualizacion y colisiones
                for (int j = 0; j < model->getModelAttributes()->size(); j++) {
                    int idxCollider = -1;
                    bool objInMovement = (*model->getNextTranslate(j)) != (*model->getTranslate(j));
                    glm::vec3& posM = objInMovement ? *model->getNextTranslate(j) : *model->getTranslate(j);
                    glm::vec3 ejeColision = glm::vec3(0);
                    bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad

                    float terrainY = getTerreno()->Superficie(posM.x, posM.z);

                    ModelCollider mcollider = model->update(terrainY, *getLoadedModels(), ejeColision, isPrincipal, j);

                    if (mcollider.model != NULL) {
                        collider = (Model*)mcollider.model;
                        idxCollider = mcollider.attrIdx;
                    }

                    if (collider != NULL && model == camara) {
                        // Recoleccion
                        if (Recolectable* item = dynamic_cast<Recolectable*>(collider)) {
                            if (KEYS[input.E] && !item->fueRecogido) {
                                item->fueRecogido = true;
                                item->setActive(false);
                                KEYS[input.E] = false;

                            }
                        }
                        // Aplastamiento
                        if (ejeColision.y == 1) {
                            //INFO("APLASTADO!!!! " + collider->name, "COLLISION");
                            if (removeCollideModel(collider, idxCollider)) i--;
                        }
                    }

                    if (j < 0) j = 0;
                }
                if (i < 0) i = 0;
            }

            int rec = 0, tot = 0;
            calcularPuntaje(rec, tot);

            // Actualizar UI solo cuando cambie el valor
            static int prevRec = -1, prevTot = -1;
            if (rec != prevRec || tot != prevTot) {
                actualizarTextoUI(rec, tot);
                prevRec = rec;
                prevTot = tot;
            }

            if (tot > 0 && rec >= tot) estadoJuego = 2;

            // Agua

            if (Principal* jugador = dynamic_cast<Principal*>(camara)) {
                glm::vec3 pos = *jugador->getTranslate();

                // Limites
                bool enX = (pos.x > -500.0f && pos.x < 500.0f);
                bool enZ = (pos.z > -35.0f && pos.z < 35.0f);
                bool enY = (pos.y < 12.0f);

                // Cronometro
                if (enX && enZ && enY) {
                    tiempoEnAgua += gameTime.deltaTime / 1000.0f;

                    // Imprime el tiempo 
                    std::cout << "Tiempo en Radiacion: " << tiempoEnAgua << std::endl;

                    if (tiempoEnAgua >= 15.0f) { // Tiempo limite

                        // HACER DAÑO
                        jugador->recibirDano(get_nanos() / 1000000.0);

                        // Reiniciar cuenta
                        tiempoEnAgua = 0.0f;
                    }
                }
                else {
                    tiempoEnAgua = 0.0f;
                }
            }

            // Validamos que itemsTotales > 0 para no ganar al inicio si no han cargado
            if (itemsTotales > 0 && itemsRecogidos >= itemsTotales) {
                estadoJuego = 2; // WIN
            }

            // B) DERROTA (Sin vidas)
            if (Principal* jugador = dynamic_cast<Principal*>(camara)) {
                if (jugador->vidas.empty()) {
                    estadoJuego = 1; // LOSE
                }
            }

            // C) DAÑO AMBIENTAL (Rio)
            if (Principal* jugador = dynamic_cast<Principal*>(camara)) {
                glm::vec3 pos = *jugador->getTranslate();
                // Limites del Rio (Ajusta a tus valores)
                if (pos.x > -500 && pos.x < 500 && pos.z > -35 && pos.z < 35 && pos.y < 12) {
                    tiempoEnAgua += gameTime.deltaTime / 1000.0f;
                    if (tiempoEnAgua >= 5.0f) {
                        jugador->recibirDano(get_nanos() / 1000000.0);
                        tiempoEnAgua = 0.0f;
                    }
                }
                else tiempoEnAgua = 0.0f;
            }

            // Actualizamos la camara
            camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());

            return -1;
        }

        void reiniciarJuego() {
            estadoJuego = 0;
            mensajeMostrado = false;
            tiempoEnAgua = 0;

            // 1. Revivir Jugador
            if (Principal* jugador = dynamic_cast<Principal*>(getMainModel())) {
                jugador->reiniciar();


                const float SPAWN_X = 50.0f;
                const float SPAWN_Z = -230.0f;
                float yRespawn = getTerreno()->Superficie(SPAWN_X, SPAWN_Z);
                glm::vec3 respawn = glm::vec3(SPAWN_X, yRespawn, SPAWN_Z);

                jugador->setTranslate(&respawn);
                jugador->setNextTranslate(&respawn);
            }

            // 2. Resetear Items
            int total = 0;
            for (Model* m : *getLoadedModels()) {
                // Reset Items
                if (Recolectable* item = dynamic_cast<Recolectable*>(m)) {
                    item->fueRecogido = false;
                    item->setActive(true);
                    total++;
                }
                if (Villano* v = dynamic_cast<Villano*>(m)) {
                    // Posicion inicial del villano
                    float terrenoY = getTerreno()->Superficie(65.0f, 340.0f);
                    glm::vec3 inicioVillano = glm::vec3(65.0f, terrenoY, 340.0f);
                    v->setTranslate(&inicioVillano);
                    v->setNextTranslate(&inicioVillano);
                    v->setAnimation(0); // Idle
                }
            }

            // 3. Resetear Texto UI
            actualizarTextoUI(0, total);
        }

        // --- ACTUALIZAR UI ---
        void actualizarTextoUI(int recogidos, int totales) {
            std::vector<Texto*>* listaTextos = getLoadedText();
            for (int k = 0; k < listaTextos->size(); k++) {
                if (listaTextos->at(k)->name == "ContadorPruebas") {
                    // Metodo Destruir y Recrear (Infalible)
                    delete listaTextos->at(k);
                    listaTextos->erase(listaTextos->begin() + k);

                    wchar_t buffer[100];
                    swprintf(buffer, 100, L"Pruebas: %d/%d", recogidos, totales);

                    Texto* nuevoTexto = new Texto(buffer, 20, 0, 10, 50, 0, getMainModel());
                    nuevoTexto->name = "ContadorPruebas";
                    listaTextos->push_back(nuevoTexto);
                    break;
                }
            }
        }

		virtual bool removeCollideModel(Model* collider, int idxCollider){
			auto it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
			if (idxCollider == 0){
				collider->setActive(false);
				Model* AABB = (Model*)collider->getModelAttributes()->at(0).hitbox;
				if (AABB)
					delete AABB;
				collider->getModelAttributes()->at(0).hitbox = NULL;
			} else {
				ModelAttributes &attr = collider->getModelAttributes()->at(idxCollider);
				Model *AABB = (Model*)attr.hitbox;
				if (AABB)
					delete AABB;
				collider->getModelAttributes()->erase(collider->getModelAttributes()->begin() + idxCollider);
			}
			if (collider->getModelAttributes()->size() == 1 && !collider->getActive()){
				it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
				it = getLoadedModels()->erase(it);
				delete collider;
				return true;
			}
			return false;
		}

		virtual Model* lookForCollition(glm::vec3 &yPos, bool collitionMovement = false) {
			std::vector<Model*> *ourModel = getLoadedModels();
			Model *camara = getMainModel();
			for (int i = 0; i < ourModel->size(); i++) {
				Model *model = (*ourModel)[i];
				if (model != camara) {
					if (camara->colisionaCon(model->getModelAttributes()->at(0), yPos, collitionMovement)) {
						return model;
					}
				}
			}
			return NULL;
		}
};