#ifndef FAITHFUL_SRC_LOADER_MODELPOOL_H_
#define FAITHFUL_SRC_LOADER_MODELPOOL_H_

#include <filesystem>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "ModelData.h"
#include "AnimatableModel.h"
#include "NonAnimatableModel.h"

#include "../Aabb.h"

#include "../Texture.h"

#define TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR // TODO:

/// we're using only "url" property, but
/// load inside the LoadTextures() with texture_pool_
bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err,
                   std::string *warn, int req_width, int req_height,
                   const unsigned char *bytes, int size, void *user_data);

/// Wrapper for all model types, serves as:
///  - a connection point between user interface & implementation
///  - a storage for data (other classes take models refs / copies from there)
/// private inheritance from IAssetPool - we don't want Load(std::string),
/// but other mechanic still useful
class ModelLoader {
 public:
  ModelLoader() {
    Init();
  }

  ModelData Load(std::string_view path);

 private:
  void Init();

  Aabb3 CalculateAabb();

  /// collecting vertex data for computing bounding shapes
  std::vector<glm::vec3> CollectModelVertices(tinygltf::Model model);

  /// generating of vao / vbo, should be called from opengl_context
  void BindMesh(ModelData& data, tinygltf::Mesh &mesh);

  void BindModelNodes(ModelData& data,
                      tinygltf::Node &node);

  void BindModel(ModelData& data);

  void LoadTextures(const std::filesystem::path& dir_path,
                    tinygltf::Model& model, Material& material);

  tinygltf::TinyGLTF loader_;

  std::vector<AnimatableModel> animatable_models_;
  std::vector<NonAnimatableModel> non_animatable_models_;
};

#endif  // FAITHFUL_SRC_LOADER_MODELPOOL_H_
