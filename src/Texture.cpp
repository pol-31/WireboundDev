#include "Texture.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Details.h"

Texture::Texture(std::string_view path) {
  if (!path.empty()) {
    Init(path);
  }
}

Texture::Texture(GLuint opengl_id, GLsizei width,
                 GLsizei height, GLint format)
    : opengl_id_(opengl_id), width_(width),
      height_(height), format_(format) {}

void Texture::Init(std::string_view path) {
  // don't need it in runtime, but good for fast test check
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int channels;
  unsigned char* data = stbi_load(path.data(), &width_,
                                  &height_, &channels, 0);
  std::cout << path << ' ';
  if (data) {
    switch (channels) {
      case 1:
        format_ = GL_RED;
        std::cout << "red" << std::endl;
        break;
      case 2:
        format_ = GL_RG;
        std::cout << "rg" << std::endl;
        break;
      case 3:
        format_ = GL_RGB;
        std::cout << "rgb" << std::endl;
        break;
      default:
        std::cout << "rgba" << std::endl;
        format_ = GL_RGBA;
    }

    if (format_ == GL_RGBA && path.starts_with("../assets/map_")) { // TODO:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
                   format_, GL_UNSIGNED_BYTE, data);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0,
                   format_, GL_UNSIGNED_BYTE, data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    throw std::runtime_error("Failed to load texture");
  }
  stbi_image_free(data);
  copies_counter_ = std::make_shared<int>(0);
}

Texture& Texture::operator=(const Texture& other) {
  if (other.opengl_id_ == this->opengl_id_) {
    return *this;
  }
  DeleteTexture();
  copies_counter_ = other.copies_counter_;
  opengl_id_ = other.opengl_id_;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
  return *this;
}

Texture& Texture::operator=(Texture&& other) noexcept {
  DeleteTexture();
  opengl_id_ = other.opengl_id_;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
  copies_counter_ = other.copies_counter_;
  return *this;
}

void Texture::DeleteTexture() {
  if (copies_counter_.use_count() == 1) {
    glDeleteTextures(1, &opengl_id_);
  }
}

Texture::~Texture() {
  DeleteTexture();
}

void Texture::Bind() const {
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
}
