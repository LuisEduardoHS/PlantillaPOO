#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>
#define zeroVec3(v) v.x == 0.0f && v.y == 0.0f && v.z == 0.0f

    struct Material {
        bool hasDiffuse;
        glm::vec3 Diffuse;
        bool hasSpecular;
        glm::vec3 Specular;
        bool hasAmbient;
        glm::vec3 Ambient;
        bool hasShininess;
        float Shininess;
    };

    struct Light {
        glm::vec3 position;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

//    uniform Material material;
//    uniform Light light;
#endif