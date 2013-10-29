#ifndef TOOLS_H
#define TOOLS_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <fstream>
#include <vector>
#include <iostream>

#define DEFAULT_SPECULAR "resources/textures/defaultSpecular.png"
#define DEFAULT_DIFFUSE "resources/textures/defaultDiffuse.png"

using namespace std;

extern GLuint normalbuffer;
extern GLuint uvbuffer;
extern GLuint vertexbuffer;
extern GLuint tangentbuffer;
extern GLuint bitangentbuffer;

extern glm::vec3 dirLight;
extern glm::vec3 ptLight;

struct Model
{
    std::vector<glm::vec3> vertices, normals, tangents, bitangents;
    std::vector<glm::vec2> texCoords;
    sf::Texture texture;
    sf::Texture normalMap;
    sf::Texture specularMap;
};

bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals);

GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);

bool drawModel(glm::mat4& modelMat, glm::mat4& viewMat, glm::mat4& projMat, glm::mat4& depthBiasMVP, GLuint& shader, GLuint& shadowMap, glm::vec3& eyePos, Model& m, bool useNormalMap);

void computeTangentBasis(vector<glm::vec3>& vertices, vector<glm::vec2>& uvs, vector<glm::vec3>& tangents, vector<glm::vec3>& bitangents);

void setBuffers(Model& m);

Model loadModel(string path, string texPath, string specularPath, string normalMapPath);

#endif // TOOLS_H
