#ifndef SIMO_DATA_HPP
#define SIMO_DATA_HPP

#include <string>
#include <cml/cml.h>
#include <vector>

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

  vec3 normalized() const;
  float length() const;
  float length_squared() const;
};

vec3 operator*(const vec3& lhs, float rhs);
vec3 operator*(float lhs, const vec3& rhs);
vec3 operator/(const vec3& lhs, float rhs);

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
  void render();
};

#endif
