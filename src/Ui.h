#ifndef WIREBOUNDDEV_SRC_UI_H_
#define WIREBOUNDDEV_SRC_UI_H_

#include <stb_truetype.h>

#include "Texture.h"
#include "GlWrappers.h"

#include "../assets/text/PhrasesHashText.h"

#include "../assets/text/PhrasesTexCoords.h"
#include "Details.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: convert textures to .astc

class IButton;

// TODO: namespace "global"?
enum class Localization {
  kUkr,
  kEng,
};

/// so when we change global locale, we should Callback to replace tex coords
/// at all prerendered phrases


Localization gLocalization = Localization::kEng;

/// TODO: global variables (literals only)
std::array<IButton*, 100> gButtons; // to update tex_coords
std::array<GLuint, 100> gFramebufferIds; // to update sizes
std::array<GLuint, 100> gTextureIdx; // to update tex_coords

class IButton {
 public:
  IButton() : id_(0/*GetGlobalId()*/) {
    gButtons[id_] = this;
  }
  virtual void Invoke() = 0;

  void SetLocalization() {
    switch (gLocalization) {
      case Localization::kUkr:
        tex_coords_ = TexCoords(
            bitmaps::kTexCoords_uk_prerendered_text[text_id_]);
        break;
      case Localization::kEng:
        tex_coords_ = TexCoords(
            bitmaps::kTexCoords_en_prerendered_text[text_id_]);
        break;
    }
  }

  virtual ~IButton() = default;

 private:
  TexCoords tex_coords_;
  int id_; // id_ in gButtons
  int text_id_; // TODO: rename to phrase_id_?
};

void SetLocalization(Localization loc) {
  gLocalization = loc;
  for (auto btn : gButtons) {
    btn->SetLocalization();
  }
}

namespace {

void RunLoop(GLFWwindow* window) {
  Shader shader_static_sprite("../shaders/StaticSprite.vert",
                              "../shaders/StaticSprite.frag");

  stbi_set_flip_vertically_on_load(true); // TODO:
  Texture tex_phrases("../assets/text/tex_en_prerendered_text.png");

  shader_static_sprite.Bind();

  glActiveTexture(GL_TEXTURE0);
  tex_phrases.Bind();
  shader_static_sprite.SetUniform("tex", 0);
  shader_static_sprite.SetUniform("transparency", 1.0f);
  shader_static_sprite.SetUniform("brightness", 1.0f);
  glm::vec3 color_white = {1.0f, 1.0f, 1.0f};
  glm::vec3 color_red = {0.6f, 0.0f, 0.0f};
  glm::vec3 color_green = {0.0f, 0.6f, 0.0f};

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        reinterpret_cast<void*>(8 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(vao);
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    shader_static_sprite.SetUniformVec3(
        "color", 1, glm::value_ptr(color_white));
    glBufferSubData(GL_ARRAY_BUFFER, 0, details::kTexPositionSize,
                    details::kTexPositionDefault.data());
    glBufferSubData(GL_ARRAY_BUFFER, details::kTexPositionSize,
                    details::kTexCoordsSize,
                    details::kTexCoordsDefault.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    shader_static_sprite.SetUniformVec3(
        "color", 1, glm::value_ptr(color_green));
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    tex_coords.Bind();
    tex_coords.BindAsPosition();
    //  glBufferSubData(GL_ARRAY_BUFFER, 0, details::kTexPositionSize,
    //                  details::kTexPositionDefault.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

}


#endif  // WIREBOUNDDEV_SRC_UI_H_
