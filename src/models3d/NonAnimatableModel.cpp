#include "NonAnimatableModel.h"

#include <glm/gtc/type_ptr.hpp>

void NonAnimatableModel::RenderMeshes(
    const Shader& shader, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 vbo_and_ebos.at(indexAccessor.bufferView));
//    glDrawElements(primitive.mode, indexAccessor.count,
//                   indexAccessor.componentType,
//                   reinterpret_cast<void*>(indexAccessor.byteOffset));
    glDrawElementsInstanced(
        primitive.mode, indexAccessor.count, indexAccessor.componentType,
        reinterpret_cast<void*>(indexAccessor.byteOffset), amount_);
    //TODO: need amount_ separately for each tile

  }
}

void NonAnimatableModel::RenderModelNodes(
    const Shader& shader, tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    RenderMeshes(shader, model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    RenderModelNodes(shader, model.nodes[node.children[i]]);
  }
}

void NonAnimatableModel::Render(const Shader& shader) {
  glBindVertexArray(vao);
  shader.Bind();

  //TODO: we can set it once for all models
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

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    RenderModelNodes(shader, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
}

void NonAnimatableModel::UpdatePositions(
    const std::vector<glm::mat4>& transforms) {
//  amount_ = 1;
  amount_ = transforms.size();

  transforms_buffer_; //TODO: save
  glGenBuffers(1, &transforms_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, transforms_buffer_);
  struct t {
    float x, y, z, w;
    float x1, y1, z1, w1;
    float x2, y2, z2, w2;
    float x3, y3, z3, w3;
  };
  std::vector<t> data;
  for (const auto& m : transforms) {
    const t* inst = reinterpret_cast<const t*>(glm::value_ptr(m));
    t i = *inst;
    data.push_back(i);
  }

  //TODO: static, but regenerate for each new tile
  glBufferData(GL_ARRAY_BUFFER, amount_ * sizeof(glm::mat4), data.data(), GL_STATIC_DRAW);
//  glBufferData(GL_ARRAY_BUFFER, amount_ * sizeof(glm::mat4), glm::value_ptr(transforms[0]), GL_STATIC_DRAW);

  glBindVertexArray(vao);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(9);
  glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);
  glVertexAttribDivisor(8, 1);
  glVertexAttribDivisor(9, 1);
  glBindVertexArray(0);
}
