#ifndef WIREBOUNDTEXTBAKER__TEXTURE_H_
#define WIREBOUNDTEXTBAKER__TEXTURE_H_

#include <memory>
#include <string_view>


#include <glad/glad.h>

#include "Shader.h"

class Texture {
 public:
  Texture() = default;
  explicit Texture(std::string_view path);
  explicit Texture(GLuint opengl_id, GLsizei width,
                   GLsizei height, GLint format);

  Texture(const Texture& other) = default;
  Texture& operator=(const Texture& other);

  Texture(Texture&& other) noexcept = default;
  Texture& operator=(Texture&& other) noexcept;

  ~Texture();

  // technically it's not const, because internal data
  // can be affected by other OpenGL functions
  void Bind() const;

  // technically it's not const, because opengl_id_ is internal OpenGL id
  // which can be affected by OpenGL functions
  [[nodiscard]] GLuint GetId() const {
    return opengl_id_;
  }

  [[nodiscard]] GLsizei GetWidth() const {
    return width_;
  }
  [[nodiscard]] GLsizei GetHeight() const {
    return height_;
  }
  [[nodiscard]] GLsizei GetFormat() const {
    return format_;
  }
  [[nodiscard]] std::pair<GLsizei, GLsizei> GetSize() const {
    return {width_, height_};
  }

  explicit operator bool() const {
    return opengl_id_ != 0;
  }

 private:
  void Init(std::string_view path);

  void DeleteTexture();

  /// we need only counter
  std::shared_ptr<void> copies_counter_{}; // TODO: is it deprecated

  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

class TextureRef {
 public:
  constexpr TextureRef() = default;

  [[nodiscard]] constexpr GLuint GetId() const {
    return id_;
  }

  constexpr void SetId(GLuint id) {
    id_ = id;
  }

  void Bind() const {
    glBindTexture(GL_TEXTURE_2D, id_);
  }

 private:
  GLuint id_{0};
};

#endif  // WIREBOUNDTEXTBAKER__TEXTURE_H_
