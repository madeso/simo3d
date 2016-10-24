#include "data.h"

#include "functions.h"

#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags

#include "wx.h"
#include "wxgl.h"

vec3 vec3::normalized() const {
  return *this / length();
}
float vec3::length() const {
  return sqrt(length_squared());
}
float vec3::length_squared() const {
  return x*x + y*y + z*z;
}

rgba White() { return rgba(1.0f, 1.0f, 1.0f); }
rgba Black() { return rgba(0.0f, 0.0f, 0.0f); }

vec3 operator+(const vec3& lhs, const vec3& rhs) {
  return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

vec3 operator*(const vec3& lhs, float rhs) {
  return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}
vec3 operator*(float lhs, const vec3& rhs) {
  return rhs * lhs;
}
vec3 operator/(const vec3& lhs, float rhs) {
  return lhs * (1.0f / rhs);
}

Index::Index(int i) : vertex(i), normal(i), uv(i) {}

namespace {
void vecopy(std::vector<vec3>& t, aiVector3D* s, unsigned int c) {
  t.reserve(c);
  for (unsigned i = 0; i < c; ++i) {
    t.push_back(vec3(s[i].x, s[i].y, s[i].z));
  }
}
}

namespace {
rgba C(aiColor3D c) {
  return rgba(c.r, c.g, c.b);
}
}

void Data::import(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path, aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);

  if (!scene) {
    AddLog(importer.GetErrorString());
    throw std::runtime_error(importer.GetErrorString());
  }

  meshes.clear();
  meshes.reserve(scene->mNumMeshes);

  materials.clear();
  materials.reserve(scene->mNumMaterials);

  for (unsigned int imat = 0; imat  < scene->mNumMaterials; ++imat ) {
    material m;
    aiMaterial* mat = scene->mMaterials[imat];

    aiString name;
    aiColor3D color;

    mat->Get(AI_MATKEY_NAME, name);
    m.name = name.C_Str();

    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
      m.diffuse = C(color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
      m.specular = C(color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
      m.ambient = C(color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, color)) {
      m.emission = C(color);
    }

    mat->Get(AI_MATKEY_SHININESS, m.shininess);

    // get textures
    materials.push_back(m);
  }

  for (unsigned int imesh = 0; imesh < scene->mNumMeshes; ++imesh) {
    aiMesh* amesh = scene->mMeshes[imesh];
    Mesh mesh;
    mesh.name = amesh->mName.data;
    vecopy(mesh.vertices, amesh->mVertices, amesh->mNumVertices);
    if (amesh->mNormals) {
      vecopy(mesh.normals, amesh->mNormals, amesh->mNumVertices);
    }

    for(unsigned int ti=0; ti<8; ++ti) {
      if( amesh->mTextureCoords[ti] ) {
        std::vector<vec3> uv;
        vecopy(uv, amesh->mTextureCoords[ti], amesh->mNumUVComponents[ti]);
        mesh.uvs.push_back(uv);
      }
    }

    for(vec3& n: mesh.normals) {
      n = n.normalized();
    }

    for (unsigned int iface = 0; iface < amesh->mNumFaces; ++iface) {
      const aiFace& aface = amesh->mFaces[iface];
      Face face;
      face.material = amesh->mMaterialIndex;
      for (unsigned int i = 0; i < aface.mNumIndices; ++i) {
        Index index { aface.mIndices[i] };
        face.indices.push_back(index);
      }
      mesh.faces.push_back(face);
    }

    meshes.push_back(mesh);
  }
}
