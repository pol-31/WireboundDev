#ifndef FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_

#include <glm/glm.hpp>

#include "ModelData.h"
#include "../Shader.h"

class NonAnimatableModel : public ModelData {
 public:
  NonAnimatableModel(ModelData model_data) : ModelData(model_data) {};

  void Render(const Shader& shader_program);

  void UpdatePositions(const std::vector<glm::mat4>& transforms);

 private:
  void RenderMeshes(const Shader& shader_program,
                    tinygltf::Mesh &mesh);

  void RenderModelNodes(const Shader& shader_program,
                        tinygltf::Node &node);
  int amount_{0};
  GLuint transforms_buffer_{0};
};

#endif  // FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_
