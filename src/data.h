#ifndef SIMO_DATA_HPP
#define SIMO_DATA_HPP

#include <string>
#include <cml/cml.h>
#include <vector>

typedef cml::vector3f vec3;

class Face {
 public:
  std::vector<int> indices;
};

class Mesh {
 public:
  std::string name;
  std::vector<vec3> vertices;
  std::vector<vec3> normals;
  std::vector<Face> faces;
};

class Data {
 public:
  std::vector<Mesh> meshes;
  void import(const std::string& path);
  void runimport(const char* const path);
  void render();
};

#endif
