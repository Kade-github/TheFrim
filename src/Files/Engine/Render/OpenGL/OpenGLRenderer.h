/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _OPENGLRENDERER_H_
#define _OPENGLRENDERER_H_

#include "../../../../Includes.h"
#include "../../Logging.h"

#include "../Renderer.h"

namespace Engine
{
    namespace Render
    {
        class OpenGLRenderer : public Renderer
        {
            void Init(GLFWwindow* window) override;

            void Render(GLFWwindow* window) override;
        };
    }
}

#endif