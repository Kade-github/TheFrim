/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _MESH_H_
#define _MESH_H_

#include "../../../../Includes.h"
#include "../../Data/MeshData.h"
#include "../../Logging.h"

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
            class Mesh
            {
            private:
                unsigned int vao, vbo, ebo;

                Data::MeshData meshData;

            public:
                Mesh(Data::MeshData m);
                ~Mesh();

                /// <summary>
                /// Create the mesh.
                /// </summary>
                void Create();


                /// <summary>
                /// Render the mesh.
                /// </summary>
                void Render();
            };
        }
    }
}


#endif