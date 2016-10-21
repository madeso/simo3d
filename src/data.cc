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

namespace {
void vecopy(std::vector<vec3>& t, aiVector3D* s, unsigned int c) {
  t.reserve(c);
  for (unsigned i = 0; i < c; ++i) {
    t.push_back(vec3(s[i].x, s[i].y, s[i].z));
  }
}
}

void Data::import(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path, aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
                aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);

  if (!scene) {
    AddLog(importer.GetErrorString());
    throw std::runtime_error(importer.GetErrorString());
  }

  if (scene->HasMeshes() == false) {
    AddLog("non meshes not currently supported");
    throw std::runtime_error("non meshes not currently supported");
  }

  meshes.clear();
  meshes.reserve(scene->mNumMeshes);
  for (unsigned int imesh = 0; imesh < scene->mNumMeshes; ++imesh) {
    aiMesh* amesh = scene->mMeshes[imesh];
    Mesh mesh;
    mesh.name = amesh->mName.data;
    vecopy(mesh.vertices, amesh->mVertices, amesh->mNumVertices);
    if (amesh->mNormals) {
      vecopy(mesh.normals, amesh->mNormals, amesh->mNumVertices);
    }

    for(vec3& n: mesh.normals) {
      n = n.normalized();
    }

    for (unsigned int iface = 0; iface < amesh->mNumFaces; ++iface) {
      const aiFace& aface = amesh->mFaces[iface];
      Face face;
      for (unsigned int i = 0; i < aface.mNumIndices; ++i) {
        face.indices.push_back(aface.mIndices[i]);

      }
      mesh.faces.push_back(face);
    }

    meshes.push_back(mesh);
  }
}

void Data::render() {
  // glColor3f(1, 1, 1);
  for (const Mesh& m : meshes) {
    for (const Face& f : m.faces) {
      int type = GL_POINTS;

      switch (f.indices.size()) {
        case 1:
          type = GL_POINTS;
          break;
        case 2:
          type = GL_LINES;
          break;
        case 3:
          type = GL_TRIANGLES;
          break;
        default:
          type = GL_POLYGON;
          break;
      }

      glBegin(type);
      for (int i : f.indices) {
        if (m.normals.empty() == false) {
          glNormal3fv(m.normals[i].data());
        }
        glVertex3fv(m.vertices[i].data());
      }
      glEnd();
    }
  }
}
