#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string>
#include <vector>
#include "Shader.h"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Surface_Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh {
public:
	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices; 
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void Draw(Shader& shader);
private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();

};


#endif