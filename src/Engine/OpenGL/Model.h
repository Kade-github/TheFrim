#ifndef _MODEL_H
#define _MODEL_H

#include "Mesh.h"
#include "../../Include/assimp/Importer.hpp"
#include "../../Include/assimp/scene.h"
#include "../../Include/assimp/mesh.h"
#include "../../Include/assimp/postprocess.h"

/// <summary>
/// A bunch of this code is from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
/// Thanks!
/// </summary>

class Model
{
public:

    // model data 
    std::vector<Texture*> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    std::string path;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const& path)
    {
        this->path = path;
        loadModel(path);
    }
private:

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif