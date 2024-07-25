#ifndef WIREBOUNDTEXTBAKER__SHADER_H_
#define WIREBOUNDTEXTBAKER__SHADER_H_

#include <string>
#include <string_view>

#include <glad/glad.h>

class Shader {
 public:
  Shader() = default;

  //TODO: mess with param, maybe enum_class_Type will make it clearer

  /// in current program we don't need other shaders
  Shader(std::string_view vert_path, std::string_view frag_path,
         std::string_view geom_path = "") {
    Init(vert_path, frag_path, geom_path);
  }

  Shader(std::string_view comp_path) {
    Init(comp_path);
  }

  Shader(std::string_view vert_path, std::string_view tesc_path,
         std::string_view tese_path, std::string_view frag_path) {
    Init (vert_path, tesc_path, tese_path, frag_path);
  }

  ~Shader();

  Shader(const Shader& other) = delete;
  Shader& operator=(const Shader& other) = delete;

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  // technically it's not const, because internal data
  // can be affected by other OpenGL functions
  void Bind() const;

  // technically they're not const, because they affect internal OpenGL
  // shader representation (by opengl_id_)

  inline void SetUniform(const GLchar* name, GLboolean v0) const {
    glUniform1f(glGetUniformLocation(opengl_id_, name),
                static_cast<float>(v0));
  }

  inline void SetUniform(const GLchar* name, GLfloat v0) const {
    glUniform1f(glGetUniformLocation(opengl_id_, name), v0);
  }

  inline void SetUniform(const GLchar* name, GLfloat v0, GLfloat v1) const {
    glUniform2f(glGetUniformLocation(opengl_id_, name), v0, v1);
  }

  inline void SetUniform(const GLchar* name, GLfloat v0,
                         GLfloat v1, GLfloat v2) const {
    glUniform3f(glGetUniformLocation(opengl_id_, name), v0, v1, v2);
  }

  inline void SetUniform(const GLchar* name, GLfloat v0,
                         GLfloat v1, GLfloat v2, GLfloat v3) const {
    glUniform4f(glGetUniformLocation(opengl_id_, name), v0, v1, v2, v3);
  }

  inline void SetUniform(const GLchar* name, GLint v0) const {
    glUniform1i(glGetUniformLocation(opengl_id_, name), v0);

  }

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1) const {
    glUniform2i(glGetUniformLocation(opengl_id_, name), v0, v1);

  }

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1, GLint v2) const {
    glUniform3i(glGetUniformLocation(opengl_id_, name), v0, v1, v2);

  }

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1,
                         GLint v2, GLint v3) const {
    glUniform4i(glGetUniformLocation(opengl_id_, name), v0, v1, v2, v3);

  }

  inline void SetUniform(const GLchar* name, GLuint v0) const {
    glUniform1ui(glGetUniformLocation(opengl_id_, name), v0);

  }

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1) const {
    glUniform2ui(glGetUniformLocation(opengl_id_, name), v0, v1);

  }

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1, GLuint v2) const {
    glUniform3ui(glGetUniformLocation(opengl_id_, name), v0, v1, v2);

  }

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1,
                         GLuint v2, GLuint v3) const {
    glUniform4ui(glGetUniformLocation(opengl_id_, name), v0, v1, v2, v3);

  }

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLfloat* value) const {
    glUniform1fv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLfloat* value) const {
    glUniform2fv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLfloat* value) const {
    glUniform3fv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLfloat* value) const {
    glUniform4fv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLint* value) const {
    glUniform1iv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLint* value) const {
    glUniform2iv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLint* value) const {
    glUniform3iv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLint* value) const {
    glUniform4iv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLuint* value) const {
    glUniform1uiv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLuint* value) const {
    glUniform2uiv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLuint* value) const {
    glUniform3uiv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLuint* value) const {
    glUniform4uiv(glGetUniformLocation(opengl_id_, name), count, value);

  }

  inline void SetUniformMat2v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix2fv(glGetUniformLocation(opengl_id_, name),
                       count, transpose, value);
  }

  inline void SetUniformMat3v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix3fv(glGetUniformLocation(opengl_id_, name),
                       count, transpose, value);
  }

  inline void SetUniformMat4v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix4fv(glGetUniformLocation(opengl_id_, name),
                       count, transpose, value);
  }

  inline void SetUniformMat2x3v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix2x3fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

  inline void SetUniformMat3x2v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix3x2fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

  inline void SetUniformMat2x4v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix2x4fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

  inline void SetUniformMat4x2v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix4x2fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

  inline void SetUniformMat3x4v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix3x4fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

  inline void SetUniformMat4x3v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value) const {
    glUniformMatrix4x3fv(glGetUniformLocation(opengl_id_, name),
                         count, transpose, value);
  }

 private:
  class ShaderObject {
   public:
    ShaderObject() = default;
    ShaderObject(const std::string& path, GLenum type);
    ~ShaderObject();

    ShaderObject(const ShaderObject& other) = delete;
    ShaderObject& operator=(const ShaderObject& other) = delete;

    ShaderObject(ShaderObject&& other) noexcept;
    ShaderObject& operator=(ShaderObject&& other) noexcept;

    // technically it's not const, because opengl_id_ is internal OpenGL id
    // which can be affected by OpenGL functions
    [[nodiscard]] GLuint GetId() const {
      return opengl_id_;
    }

   private:
    GLuint opengl_id_{0};
  };

  void Init(std::string_view vert_path, std::string_view frag_path,
            std::string_view geom_path);

  void Init(std::string_view comp_path);

  void Init(std::string_view vert_path, std::string_view tesc_path,
            std::string_view tese_path, std::string_view frag_path);

  static std::string LoadShader(std::string_view path);

  static void CheckCompilation(GLuint shader, GLenum type);
  static void CheckLinking(GLuint shader);

  static std::string ShaderNameFromType(GLenum type);

  GLuint opengl_id_{0};
};

#endif //WIREBOUNDTEXTBAKER__SHADER_H_
