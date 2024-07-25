#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Details.h"

// TODO: exceptions, not std::cerr

std::string Shader::LoadShader(std::string_view path) {
  std::ifstream shader_file;
  shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  shader_file.open(path.data());
  std::stringstream shader_stream;
  shader_stream << shader_file.rdbuf();
  return shader_stream.str();
}

void Shader::Init(std::string_view vert_path, std::string_view frag_path,
                  std::string_view geom_path) {
  std::string vert_code, frag_code, geom_code;
  try {
    vert_code = LoadShader(vert_path);
    frag_code = LoadShader(frag_path);
    if (!geom_path.empty()) {
      geom_code = LoadShader(geom_path);
    }
  }
  catch (const std::ifstream::failure& e) {
    std::cerr << "shader reading error: " << e.what() << std::endl;
  }

  ShaderObject vert_shader, frag_shader;
  vert_shader = ShaderObject(vert_code, GL_VERTEX_SHADER);
  frag_shader = ShaderObject(frag_code, GL_FRAGMENT_SHADER);

  opengl_id_ = glCreateProgram();
  glAttachShader(opengl_id_, vert_shader.GetId());
  glAttachShader(opengl_id_, frag_shader.GetId());

  if (!geom_path.empty()) {
    ShaderObject geom_shader;
    geom_shader = ShaderObject(geom_code, GL_GEOMETRY_SHADER);
    glAttachShader(opengl_id_, geom_shader.GetId());
  }

  glLinkProgram(opengl_id_);
  CheckLinking(opengl_id_);
}

void Shader::Init(std::string_view comp_path) {
  std::string comp_code;
  try {
    comp_code = LoadShader(comp_path);
  }
  catch (const std::ifstream::failure& e) {
    std::cerr << "shader reading error: " << e.what() << std::endl;
  }

  ShaderObject comp_shader;
  comp_shader = ShaderObject(comp_code, GL_COMPUTE_SHADER);
  opengl_id_ = glCreateProgram();
  glAttachShader(opengl_id_, comp_shader.GetId());

  glLinkProgram(opengl_id_);
  CheckLinking(opengl_id_);
}

void Shader::Init(std::string_view vert_path, std::string_view tesc_path,
                  std::string_view tese_path, std::string_view frag_path) {
  std::string vert_code, tesc_code, tese_code, frag_code;
  try {
    vert_code = LoadShader(vert_path);
    tesc_code = LoadShader(tesc_path);
    tese_code = LoadShader(tese_path);
    frag_code = LoadShader(frag_path);
  }
  catch (const std::ifstream::failure& e) {
    std::cerr << "shader reading error: " << e.what() << std::endl;
  }

  ShaderObject vert_shader, tesc_shader, tese_shader, frag_shader;
  vert_shader = ShaderObject(vert_code, GL_VERTEX_SHADER);
  tesc_shader = ShaderObject(tesc_code, GL_TESS_CONTROL_SHADER);
  tese_shader = ShaderObject(tese_code, GL_TESS_EVALUATION_SHADER);
  frag_shader = ShaderObject(frag_code, GL_FRAGMENT_SHADER);

  opengl_id_ = glCreateProgram();
  glAttachShader(opengl_id_, vert_shader.GetId());
  glAttachShader(opengl_id_, tesc_shader.GetId());
  glAttachShader(opengl_id_, tese_shader.GetId());
  glAttachShader(opengl_id_, frag_shader.GetId());

  glLinkProgram(opengl_id_);
  CheckLinking(opengl_id_);
}


Shader::Shader(Shader&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
  if (opengl_id_ != 0) {
    glDeleteProgram(opengl_id_);
  }
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  return *this;
}

Shader::~Shader() {
  if (opengl_id_ != 0) {
    glDeleteProgram(opengl_id_);
  }
}

Shader::ShaderObject::ShaderObject(const std::string& path, GLenum type) {
  opengl_id_ = glCreateShader(type);
  const char* path_c = path.c_str();
  glShaderSource(opengl_id_, 1, &path_c, nullptr);
  glCompileShader(opengl_id_);
  CheckCompilation(opengl_id_, type);
}


Shader::ShaderObject::ShaderObject(ShaderObject&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
}

Shader::ShaderObject& Shader::ShaderObject::operator=(
    ShaderObject&& other) noexcept {
  glDeleteShader(opengl_id_);
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  return *this;
}

Shader::ShaderObject::~ShaderObject() {
  glDeleteShader(opengl_id_);
}

void Shader::Bind() const {
  glUseProgram(opengl_id_);
}

void Shader::CheckCompilation(GLuint shader, GLenum type) {
  GLint success;
  GLchar buffer[details::kShaderErrorBufferSize];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, details::kShaderErrorBufferSize,
                       nullptr, buffer);
    std::cerr
        << "Shader compiling error at " << ShaderNameFromType(type)
        << " shader:\n" << buffer << std::endl;
  }
}

void Shader::CheckLinking(GLuint shader) {
  GLint success;
  GLchar buffer[details::kShaderErrorBufferSize];
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, details::kShaderErrorBufferSize,
                        nullptr, buffer);
    std::cerr << "Shader linking error:\n" << buffer << std::endl;
  }
}

std::string Shader::ShaderNameFromType(GLenum type) {
  switch (type) {
    case GL_VERTEX_SHADER:
      return "vertex";
    case GL_FRAGMENT_SHADER:
      return "fragment";
    case GL_GEOMETRY_SHADER:
      return "geometry";
    case GL_COMPUTE_SHADER:
      return "compute";
    default:
      throw std::runtime_error("incorrect shader type");
  }
}
