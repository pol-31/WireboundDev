#ifndef WIREBOUNDDEV_SRC_SPRITE_H_
#define WIREBOUNDDEV_SRC_SPRITE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "GlWrappers.h"
#include "Colors.h"

/// store position data as 2D transform matrix;
/// in shaders set as uniform variable
class Sprite {
 public:
  /// all texture data are known at compile-time, so there's no need in that
  /// (except position, but we can change it later)
  Sprite() = delete;

  Sprite(const Texture& tex, TexCoords tex_coords)
  : tex_(tex), tex_coords_(tex_coords) {}

  virtual ~Sprite() = default;

  // TODO: macros cpp_const & gl_const

  /// IMPORTANT: shader, vao, vbo should be already bind/set
  ///  (otherwise each Text instance should've been kept them as class members)
  void Render(const Shader& shader) {
    tex_.Bind();
    shader.SetUniformMat4v("transform", 1, GL_FALSE,
                           glm::value_ptr(transform_));
    shader.SetUniform("brightness", brightness_);
    shader.SetUniform("transparency", transparency_);
    shader.SetUniformVec3("color", 1, glm::value_ptr(color_));
    // TODO: buffer should be bind
    tex_coords_.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

 protected:
  TexCoords tex_coords_;

 private:
  Texture tex_;
  glm::mat4 transform_; // TODO: can we use mat3?
  float brightness_{1.0f};
  float transparency_{1.0f};
  glm::vec3 color_{colors::kWhite};
};

/// store position data as GL_TRIANGLE_STRIP vector data;
/// in shaders set as layout variable, so need to push data to buffer.
/// Because we separate floating-rendering_targets and static at rendering,
/// we can save on matrix multiplication by providing already known data.
/// Usage example: hud, static inventory tabs, main menu buttons, settings
class StaticSprite {
 public:
  /// all texture data are known at compile-time, so there's no need in that
  /// (except position, but we can change it later)
  StaticSprite() = delete;

  StaticSprite(const Texture& tex, TexCoords tex_coords,
               const TexPosition& position)
  : tex_(tex), tex_coords_(tex_coords), position_(position) {}

  virtual ~StaticSprite() = default;

  /// IMPORTANT: shader, vao, vbo should be already bind/set
  ///  (otherwise each Text instance should've been kept them as class members)
  void Render(const Shader& shader) {
    tex_.Bind();
    shader.SetUniform("brightness", brightness_);
    shader.SetUniform("transparency", transparency_);
    shader.SetUniformVec3("color", 1, glm::value_ptr(color_));
    // TODO: buffer should be bind
    tex_coords_.Bind();
    position_.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

 private:
  Texture tex_;
  TexCoords tex_coords_;
  TexPosition position_;
  float brightness_{1.0f};
  float transparency_{1.0f};
  glm::vec3 color_{colors::kWhite};
};

#endif //WIREBOUNDDEV_SRC_SPRITE_H_
