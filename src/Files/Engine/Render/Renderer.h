/* 
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../../../Includes.h"
#include <glm/vec2.hpp>
namespace Engine
{
    namespace Render
    {
        class Renderer
        {
        public:
            glm::vec2 Viewport;

            virtual void Init(GLFWwindow* window) = 0;

            virtual void Render(GLFWwindow* window) = 0;
        };
    }
}


#endif