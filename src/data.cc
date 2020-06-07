#include "data.h"
#include "functions.h"

#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags

simo::vec3 operator+(const simo::vec3& lhs, const simo::vec3& rhs) {
  simo::vec3 r;
  r.set_x(lhs.x() + rhs.x());
  r.set_y(lhs.y() + rhs.y());
  r.set_z(lhs.z() + rhs.z());
  return r;
}
simo::vec3 operator*(const simo::vec3& lhs, float rhs) {
  simo::vec3 r;
  r.set_x(lhs.x() * rhs);
  r.set_y(lhs.y() * rhs);
  r.set_z(lhs.z() * rhs);
  return r;
}

simo::vec3 operator/(const simo::vec3& lhs, float rhs) {
  simo::vec3 r;
  r.set_x(lhs.x() / rhs);
  r.set_y(lhs.y() / rhs);
  r.set_z(lhs.z() / rhs);
  return r;
}

simo::rgba Color(float r, float g, float b) { simo::rgba re; re.set_r(r); re.set_g(g); re.set_b(b); return re; }
simo::rgba White() { return Color(1.0f, 1.0f, 1.0f); }
simo::rgba Black() { return Color(0.0f, 0.0f, 0.0f); }

namespace {
void C(simo::rgba* r, aiColor3D c) {
  r->set_r(c.r);
  r->set_g(c.g);
  r->set_b(c.b);
  r->set_a(1.0f);
}
}

simo::material DefaultMaterial() {
  simo::material m;
  *m.mutable_ambient() = Color(0.2f, 0.2f, 0.2f);
  *m.mutable_diffuse() = Color(0.8f, 0.8f, 0.8f);
  *m.mutable_specular() = Color(0.0f, 0.0f, 0.0f);
  *m.mutable_emission() = Color(0.0f, 0.0f, 0.0f);
  m.set_shininess(0.0f);
  return m;
}

simo::index Index(int i) {
  simo::index index;
  index.set_vertex(i);
  index.set_normal(i);
  index.set_uv(i);
  return index;
}

Array<simo::vec3, Mesh> Mesh::vertices() {
  auto* vertices = data->mutable_meshes(index)->mutable_vertices();
  return Array<simo::vec3, Mesh>(this, vertices);
}

void Mesh::open() {
  if( old.get() != nullptr ) {
    throw "Unable to open, mesh is already opened!";
  }
  old.reset( new simo::mesh(data->meshes(index)) );
}

void Mesh::close() {
  if( old.get() == nullptr ) {
    throw "Unable to close, mesh wasn't opened!";
  }
  old.reset();
}

void Mesh::assertOpen() {
  if( old.get() == nullptr ) {
    throw "Mesh isn't open!";
  }
}

namespace {
void vecopy(google::protobuf::RepeatedPtrField<simo::vec3>* t, aiVector3D* s, unsigned int c) {
  t->Clear();
  for (unsigned i = 0; i < c; ++i) {
    simo::vec3* v = t->Add();
    v->set_x(s[i].x);
    v->set_y(s[i].y);
    v->set_z(s[i].z);
  }
}
}

Mesh::Mesh(simo::filedata* d, int ind) : data(d), index(ind) {
}

Mesh Data::meshes() {
  return Mesh(&data, 0);
}

std::string Mesh::toString() const {
  return data->meshes(index).name();
}

void Data::import(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path, aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);

  if (!scene) {
    AddLog(importer.GetErrorString());
    throw std::runtime_error(importer.GetErrorString());
  }

  data.clear_materials();
  data.clear_meshes();

  for (unsigned int imat = 0; imat  < scene->mNumMaterials; ++imat ) {
    simo::material* m = data.add_materials();
    aiMaterial* mat = scene->mMaterials[imat];

    aiString name;
    aiColor3D color;

    mat->Get(AI_MATKEY_NAME, name);
    m->set_name(name.C_Str());

    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
      C(m->mutable_diffuse(), color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
      C(m->mutable_specular(), color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
      C(m->mutable_ambient(), color);
    }
    if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, color)) {
      C(m->mutable_emission(), color);
    }

    float f = 0.0f;
    mat->Get(AI_MATKEY_SHININESS, f);
    m->set_shininess(f);

    // get textures
  }

  for (unsigned int imesh = 0; imesh < scene->mNumMeshes; ++imesh) {
    aiMesh* amesh = scene->mMeshes[imesh];
    simo::mesh* mesh = data.add_meshes();
    mesh->set_name(amesh->mName.data);
    vecopy(mesh->mutable_vertices(), amesh->mVertices, amesh->mNumVertices);
    if (amesh->mNormals) {
      vecopy(mesh->mutable_normals(), amesh->mNormals, amesh->mNumVertices);
    }

    for(unsigned int ti=0; ti<8; ++ti) {
      if( amesh->mTextureCoords[ti] ) {
        simo::texcoord* uv = mesh->add_uvs();
        vecopy(uv->mutable_coords(), amesh->mTextureCoords[ti], amesh->mNumUVComponents[ti]);
      }
    }

    for (unsigned int iface = 0; iface < amesh->mNumFaces; ++iface) {
      const aiFace& aface = amesh->mFaces[iface];
      simo::face* face = mesh->add_faces();
      face->set_material(amesh->mMaterialIndex);
      for (unsigned int i = 0; i < aface.mNumIndices; ++i) {
        simo::index* index = face->add_indices();
        *index = Index(aface.mIndices[i]);
      }
    }
  }
}
