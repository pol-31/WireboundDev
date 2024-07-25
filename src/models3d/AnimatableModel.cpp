#include "AnimatableModel.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtc/type_ptr.hpp>

void AnimatableModel::RenderMesh(tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 vbo_and_ebos.at(indexAccessor.bufferView));

    glDrawElements(primitive.mode, indexAccessor.count,
                   indexAccessor.componentType,
                   reinterpret_cast<void*>(indexAccessor.byteOffset));
  }
}

void AnimatableModel::RenderModelNodes(tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    RenderMesh(model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    RenderModelNodes(model.nodes[node.children[i]]);
  }
}

void AnimatableModel::Render(GLuint ubo, const Shader& shader) {
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    RenderModelNodes(model.nodes[scene.nodes[i]]);
  }

  shader.Bind();

  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,
                  model.skins.size() * sizeof(glm::mat4),
                  glm::value_ptr(cur_pose_[0]));

  shader.SetUniform("tex_albedo", 0);
  shader.SetUniform("tex_emission", 1);
  shader.SetUniform("tex_metal_rough", 2);
  shader.SetUniform("tex_normal", 3);
  shader.SetUniform("tex_occlusion", 4);
  glActiveTexture(GL_TEXTURE0);
  material.albedo.Bind();
  glActiveTexture(GL_TEXTURE1);
  material.emission.Bind();
  glActiveTexture(GL_TEXTURE2);
  material.metal_rough.Bind();
  glActiveTexture(GL_TEXTURE3);
  material.normal.Bind();
  glActiveTexture(GL_TEXTURE4);
  material.occlusion.Bind();

  shader.SetUniform(
      "bone_num", static_cast<GLint>(model.skins.size()));

  shader.SetUniformMat4v("transform", 1, GL_FALSE,
                                 glm::value_ptr(transform_));

  glBindVertexArray(0);
}

void AnimatableModel::UpdateAnimation(float time, float framerate) {
  /// Case (time == 1) is checked in hierarchy higher
  /// (SkinnedObjectImpl::Update())
  time_ += framerate /* * speed_ / 10*/;

  // time_ * 200 /* TODO: * TicksPerSeconds*/

  if (time_ >= 1) {
    if (repeat_) {
      time_ = 0;
    } else {
      time_ = 1;
      return;
    }
  }
  for (const auto& channel : model.animations[0].channels) {
    const tinygltf::AnimationSampler& sampler = model.animations[0].samplers[channel.sampler];
    const tinygltf::Accessor& timeAccessor = model.accessors[sampler.input];
    const tinygltf::Accessor& keyframeAccessor = model.accessors[sampler.output];
    const tinygltf::BufferView& timeBufferView = model.bufferViews[timeAccessor.bufferView];
    const tinygltf::BufferView& keyframeBufferView = model.bufferViews[keyframeAccessor.bufferView];
    const tinygltf::Buffer& timeBuffer = model.buffers[timeBufferView.buffer];
    const tinygltf::Buffer& keyframeBuffer = model.buffers[keyframeBufferView.buffer];

    // Find the two keyframes surrounding the current time
    int numKeyframes = keyframeAccessor.count;
    int keyframeIndex = -1;
    for (int i = 0; i < numKeyframes - 1; ++i) {
      float startTime = *reinterpret_cast<const float*>(&timeBuffer.data[i * sizeof(float)]);
      float endTime = *reinterpret_cast<const float*>(&timeBuffer.data[(i + 1) * sizeof(float)]);
      if (time >= startTime && time < endTime) {
        keyframeIndex = i;
        break;
      }
    }
    if (keyframeIndex == -1) {
      // Time is outside the range of keyframes, use the last keyframe
      keyframeIndex = numKeyframes - 2;
    }

    // Interpolate between the two keyframes
    glm::mat4 startMatrix, endMatrix;
    int startOffset = keyframeIndex * 16 * sizeof(float);
    int endOffset = (keyframeIndex + 1) * 16 * sizeof(float);
    for (int i = 0; i < 16; ++i) {
      startMatrix[i / 4][i % 4] = *reinterpret_cast<const float*>(&keyframeBuffer.data[startOffset + i * sizeof(float)]);
      endMatrix[i / 4][i % 4] = *reinterpret_cast<const float*>(&keyframeBuffer.data[endOffset + i * sizeof(float)]);
    }
    float startTime = *reinterpret_cast<const float*>(&timeBuffer.data[keyframeIndex * sizeof(float)]);
    float endTime = *reinterpret_cast<const float*>(&timeBuffer.data[(keyframeIndex + 1) * sizeof(float)]);
    float t = (time - startTime) / (endTime - startTime);
    std::terminate(); // NOT IMPLEMENTED
//    glm::mat4 interpolatedMatrix = glm::mix(startMatrix, endMatrix, t);

    cur_pose_.push_back({}/*interpolatedMatrix*/);
  }
}

void AnimatableModel::RunAnimation(int animation_id) {
  UnsetNextAnimation();
  ResetTimeAnimation();
  SetCurrentAnimation(animation_id);
}

void AnimatableModel::StopAnimation() {
  UnsetNextAnimation();
  UnsetCurrentAnimation();
  ResetTimeAnimation();
}

void AnimatableModel::SetCurrentAnimation(int animation_id) {
  ResetTimeAnimation();
  cur_animation_id_ = animation_id;
}
void AnimatableModel::UnsetCurrentAnimation() {
  ResetTimeAnimation();
  cur_animation_id_ = next_animation_id_;
  UnsetNextAnimation();
}

void AnimatableModel::SetNextAnimation(int animation_id) {
  next_animation_id_ = animation_id;
}
void AnimatableModel::UnsetNextAnimation() {
  next_animation_id_ = -1;
}

void AnimatableModel::ResetTimeAnimation() {
  time_ = 0;
  //  time_ = glfwCurrentTime(); // TODO: -___________---_--___--__-__--___-
}
