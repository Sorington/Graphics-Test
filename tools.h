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

using namespace std;

bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals);

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif // TOOLS_H
