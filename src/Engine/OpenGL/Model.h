#ifndef _MODEL_H
#define _MODEL_H

#include "Mesh.h"
#include "../../../Include/assimp/Importer.hpp"
#include "../../../Include/assimp/scene.h"
#include "../../../Include/assimp/mesh.h"
#include "../../../Include/assimp/postprocess.h"

/// <summary>
/// A bunch of this code is from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
/// Thanks!
/// </summary>

class Model
{
public:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotateAxis = glm::vec3(0, 1, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    float angle = 0.0f;
    std::vector<Mesh>    meshes;
    std::string directory;
    std::string path;

    Texture* uv = nullptr;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const& path)
    {
        this->path = path;
        loadModel(path);
    }

    Model()
    {

    }

    Mesh& GetMesh(int index)
    {
		return meshes[index];
	}

    Mesh& GetMesh(std::string name)
    {
        for (int i = 0; i < meshes.size(); i++)
        {
            if (meshes[i].name == name)
            {
				return meshes[i];
            }
        }
    }

    void ClearUVMaps()
    {
        for (int i = 0; i < meshes.size(); i++)
        {
            meshes[i].uv = nullptr;
		}
	}

    void LoadUVMap(std::string path);

    void Draw();
private:

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};

#endif