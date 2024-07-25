#ifndef FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "ModelData.h"
#include "../Shader.h"

// TODO: should Update cur_pose_

class AnimatableModel : public ModelData {
 public:
  AnimatableModel(ModelData model_data) : ModelData(model_data) {};

  void Render(GLuint ubo, const Shader& shader);

  void UpdateAnimation(float time, float framerate);

  void RunAnimation(int animation_id);
  void StopAnimation();

  void SetCurrentAnimation(int animation_id);
  void UnsetCurrentAnimation();

  void SetNextAnimation(int animation_id);
  void UnsetNextAnimation();
  void ResetTimeAnimation();

 private:
  void RenderMesh(tinygltf::Mesh &mesh);
  void RenderModelNodes(tinygltf::Node &node);

  std::vector<glm::mat4> cur_pose_{0};

  glm::mat4 transform_;

  int cur_animation_id_ = -1;
  int next_animation_id_ = -1;

  double time_ = 0;
  bool repeat_ = false;
};

#endif  // FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_
