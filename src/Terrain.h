#ifndef WIREBOUNDDEV_SRC_TERRAIN_H_
#define WIREBOUNDDEV_SRC_TERRAIN_H_

#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"

class Terrain {
 public:
  Terrain(
      std::string_view shader_vert_path,
      std::string_view shader_tessc_path,
      std::string_view shader_tesse_path,
      std::string_view shader_frag_path,
      std::string_view height_map_path,
      std::string_view tex_material_path,
      std::string_view tex_occlusion_path)
      : shader_(shader_vert_path, shader_tessc_path,
                shader_tesse_path, shader_frag_path),
        height_map_(height_map_path),
        tex_color_(tex_material_path),
        tex_occlusion_(tex_occlusion_path) {
    Init();
  }

  void Render() const {
    shader_.Bind();
    shader_.SetUniform("dmap_depth", dmap_depth_);

    glBindVertexArray(vao_);

    glActiveTexture(GL_TEXTURE0);
    height_map_.Bind();

    glActiveTexture(GL_TEXTURE1);
    tex_color_.Bind();

    glActiveTexture(GL_TEXTURE2);
    tex_occlusion_.Bind();

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
    glBindVertexArray(0);
  }

  //TODO: tile_id unused by now, but when we have multiple tiles, we need it
  glm::vec3 GetPosition(int tile_id, int coord_x, int coord_z) {
    /// on surface (no height)
    auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(heights_[coord_x + coord_z * 1024]);
//    where.y *= dmap_depth_;
    where.y /= 64.0f; // TODO: idk why 64.0f
//    std::cout << where.y << std::endl;
    return where;
  }

 private:
  void Init() {
    shader_.Bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    shader_.SetUniform("tex_displacement", 0);
    shader_.SetUniform("tex_color", 1);
    shader_.SetUniform("tex_occlusion", 2);

    // Define the vertices (not actually used, but required for the draw call)
    GLfloat vertices[] = {
        -0.5f, 0.0f, -0.5f, 1.0f,
        0.5f, 0.0f, -0.5f, 1.0f,
        -0.5f, 0.0f,  0.5f, 1.0f,
        0.5f, 0.0f,  0.5f, 1.0f
    };

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    InitHeights();

    //TODO:
    // - glPatchParameteri(GL_PATCH_VERTICES, n);
    // - ubo for view & proj matrices
  }

  void InitHeights() {
    // TODO: get height, width and only then init....
    heights_.resize(1024 * 1024);
    height_map_.Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, heights_.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }


  Shader shader_;
  Texture height_map_;

 public:
  const Texture& GetHeightMap() const {
    return height_map_;
  }
  float GetDmapDepth() const {
    return dmap_depth_;
  }

 private:
  Texture tex_color_;
  Texture tex_occlusion_;

  GLuint vao_{0};
  GLuint vbo_{0};

  /// need to duplicate both on GPU and CPU:
  /// for gpu to render terrain; for cpu to dynamically obtain object y pos
  std::vector<uint8_t> heights_;

  //
  float dmap_depth_ = 4.0f; // height_map_ scale
};

#endif  // WIREBOUNDDEV_SRC_TERRAIN_H_
