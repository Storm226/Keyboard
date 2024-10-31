#ifndef MODEL_H
#define MODEL_H

#include "../Headers/Shader.h"
#include "../Headers/Mesh.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model {
public:
	Model(const char* path);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);

	void Draw(Shader& shader);

	

private:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // !MODEL_H


