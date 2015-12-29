#ifndef SIMO_DATA_HPP
#define SIMO_DATA_HPP

#include <string>
#include <cml/cml.h>
#include <vector>

typedef std::vector<int> IntList;

struct vec3 {
  vec3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

  union {
    struct {
      float x;
      float y;
      float z;
    };
    float array[3];
  };

  float* data() { return array; }
  const float* const data() const { return array; }
};
typedef std::vector<vec3> Vec3List;

class Face {
 public:
  IntList indices;
};
typedef std::vector<Face> FaceList;

class Mesh {
 public:
  std::string name;
  Vec3List vertices;
  Vec3List normals;
  FaceList faces;
};
typedef std::vector<Mesh> MeshList;

class Data {
 public:
  MeshList meshes;
  void import(const std::string& path);
  void render();
};

#endif
