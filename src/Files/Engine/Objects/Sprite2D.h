/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "GameObject.h"

namespace Engine
{
    namespace Objects
    {
        class Sprite2D : public GameObject
        {
        private:
        public:
            Sprite2D(std::string path);

            void OnCreate() override;
            void OnDestroy() override;

            void Render() override;
        };
    }
}

#endif