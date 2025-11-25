#pragma once
#ifndef _RECOLECTABLE_H
#define _RECOLECTABLE_H

#include "Base/model.h"

// HERENCIA
class Recolectable : public Model {
public:
    bool fueRecogido = false;

    // CONSTRUCTOR
    Recolectable(string path, Camera* camera) : Model(path, camera) {
        name = "Item_Recolectable";
    }

    // ANIMACION
    void animar() {
        if (!fueRecogido) {
            float rot = getRotY() + 1.0f;
            if (rot > 360) rot -= 360;

            setRotY(rot);
            setNextRotY(rot);
        }
    }
};

#endif