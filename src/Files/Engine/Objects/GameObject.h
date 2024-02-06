/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "../../Includes.h"

#include "../../../include/glm/vec2.hpp"
#include "../../../include/glm/vec3.hpp"

namespace Engine
{
    namespace Objects
    {
        class GameObject
        {
        private:

            // inner vec2,vec3
            glm::vec2 position2D;
            float rotation2D;
            glm::vec3 position3D;
            glm::vec3 rotation3D;

            int id;

        public:

            bool operator==(const GameObject& rhs) const
            {
                const GameObject* ptr = dynamic_cast<const GameObject*>(&rhs);
                if (ptr != 0) 
                    return (id == ptr->id);
                else 
                    return false;
            }

            GameObject()
            {
                static int idCounter = 0;

                id = idCounter;
                idCounter++;

                position2D = glm::vec2(0.0f, 0.0f);
                rotation2D = 0.0f;
                position3D = glm::vec3(0.0f, 0.0f, 0.0f);
                rotation3D = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            ~GameObject()
            {
                OnDestroy();
            }

            virtual void OnCreate() = 0;
            virtual void OnDestroy() = 0;

            virtual void Render() = 0;
        };
    }
}

#endif