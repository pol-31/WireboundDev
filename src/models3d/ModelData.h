#ifndef WIREBOUNDDEV_SRC_MODELS3D_MODELDATA_H_
#define WIREBOUNDDEV_SRC_MODELS3D_MODELDATA_H_

#include <map>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "../Texture.h"
#include "../Aabb.h"

struct Material {
  Texture albedo;
  Texture emission;
  Texture metal_rough;
  Texture normal;
  Texture occlusion;
};

struct ModelData {
  GLuint vao{0};
  std::map<int, GLuint> vbo_and_ebos;

  // useful for placement on terrain; you need to add it to terrain height
  float descent{0};

  float GetDescent() const {
    return descent;
  }

  /// are these two useless?
  /*float GetHeight() const {
    return aabb.Height();
  }
  float GetWidth() const {
    return aabb.Width();
  }*/

  const Aabb3& GetAabb() const {
    return aabb;
  }
//  Collision* GetCollision() {}

  tinygltf::Model model;
  Material material;

  //TODO: we need it for rendering wrt terrain_height and for collision as well...
  Aabb3 aabb;

//  ~ModelData() {
//    glDeleteVertexArrays(1, &vao);
//  }
};

#endif  // WIREBOUNDDEV_SRC_MODELS3D_MODELDATA_H_
