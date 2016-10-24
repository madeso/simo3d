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

struct rgba {
  union {
    struct {
      float r;
      float g;
      float b;
      float a;
    };
    float array[4];
  };


  rgba() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
  rgba(float ar, float ag, float ab) : r(ar), g(ag), b(ab), a(1.0f) {}
  rgba(float ar, float ag, float ab, float aa) : r(ar), g(ag), b(ab), a(aa) {}

  float* data() { return array; }
  const float* const data() const { return array; }
};

rgba White();
rgba Black();

struct texture {
  std::string path;
  int texture_coord;
};

struct material {
  std::string name;
  rgba ambient; // specify the ambient RGBA reflectance of the material. -1 to +1
  rgba diffuse; // specify the diffuse RGBA reflectance of the material. -1 to +1
  rgba specular;  // specify the specular RGBA reflectance of the material. -1 to +1
  rgba emission; // specify the RGBA emitted light intensity of the material.-1 to +1
  float shininess; // specifies the RGBA specular exponent of the material. 0 to 128 (clamped)

  std::string shader;
  std::vector<texture> textures;

  material() : ambient(0.2f, 0.2f, 0.2f, 1.0f), diffuse(0.8f, 0.8f, 0.8f, 1.0f), specular(0.0f, 0.0f, 0.0f, 1.0f), emission(0.0f, 0.0f, 0.0f, 1.0f), shininess(0.0f) {}
};

vec3 operator+(const vec3& lhs, const vec3& rhs);
vec3 operator*(const vec3& lhs, float rhs);
vec3 operator*(float lhs, const vec3& rhs);
vec3 operator/(const vec3& lhs, float rhs);

struct Index {
  explicit Index(int i);
  int vertex;
  int normal;
  int uv;
};

class Face {
 public:
  std::vector<Index> indices;
  int material;
};

class Mesh {
 public:
  std::string name;

  std::vector<vec3> vertices;
  std::vector<vec3> normals;
  std::vector< std::vector<vec3> > uvs;

  std::vector<Face> faces;
};

class Data {
 public:
  std::vector<Mesh> meshes;
  std::vector<material> materials;

  void import(const std::string& path);
};

#endif
