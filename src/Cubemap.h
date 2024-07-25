#ifndef WIREBOUNDDEV_SRC_CUBEMAP_H_
#define WIREBOUNDDEV_SRC_CUBEMAP_H_

#include <iostream>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>
#include <stb_image.h>

#include "Shader.h"
#include "Texture.h"

class Cubemap {
 public:
  /// order: right, left, top, bottom, front, back
  Cubemap(const std::vector<std::string_view>& tex_paths,
          std::string_view path_shader_vert,
          std::string_view path_shader_frag)
      : shader_(path_shader_vert, path_shader_frag) {
    if (tex_paths.size() != 6) {
      std::cerr << "wrong cubemap texture data" << std::endl;
      return;
    }
    shader_.Bind();
    shader_.SetUniform("tex", 0);
    glUseProgram(0);

    InitTextures(tex_paths);
    InitBuffers();
  }

  void Render(const glm::mat4& mat_view, const glm::mat4 mat_proj) {
    glDepthFunc(GL_LEQUAL);
    shader_.Bind();
    auto mat_view_upd = glm::mat4(glm::mat3(mat_view));
    shader_.SetUniformMat4v("view", 1, false, glm::value_ptr(mat_view_upd));
    shader_.SetUniformMat4v("proj", 1, false, glm::value_ptr(mat_proj));
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_.GetId());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    //TODO: alternative
    /*glDepthMask(GL_FALSE);
    shader_.Bind();
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_.GetId());

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);*/
  }

 private:
  void InitTextures(const std::vector<std::string_view>& tex_paths) {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

    int width, height, channels;
    for (int i = 0; i < tex_paths.size(); ++i) {
      unsigned char* data = stbi_load(
          tex_paths[i].data(), &width, &height, &channels, 0);
      if (data) { // TODO: channels?
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
      } else {
        std::cout << "Cubemap texture failed to load at path: "
                  << tex_paths[i] << std::endl;
        stbi_image_free(data);
      }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    tex_ = Texture(tex_id, width, height, GL_RGB);
  }

  void InitBuffers() {
    float skybox_vertices[] = {
        // coords
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
                 &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          reinterpret_cast<void*>(0));
  }

  Texture tex_;
  Shader shader_;
  GLuint vao_{0};
  GLuint vbo_{0};
};

#endif  // WIREBOUNDDEV_SRC_CUBEMAP_H_
