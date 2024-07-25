#include "Grass.h"

#include <random>
#include <vector>

#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// Indirect drawing structure
struct NumBlades {
  std::uint32_t vertexCount = 5;
  std::uint32_t instanceCount = 1;
  std::uint32_t firstVertex = 0;
  std::uint32_t firstInstance = 0;
};

struct Blade {
  glm::vec4 v0; // xyz: Position, w: orientation (in radius)
  glm::vec4 v1; // xyz: Bezier point w: height
  glm::vec4 v2; // xyz: Physical model guide w: width
  glm::vec4 up; // xyz: Up vector w: stiffness coefficient
};

namespace {

std::vector<Blade> GenerateBlades() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> orientation_dis(0, glm::pi<float>());
  std::uniform_real_distribution<float> height_dis(0.6f, 1.2f);
  std::uniform_real_distribution<float> dis(-1, 1);

  std::vector<Blade> blades;
  for (int i = -80; i < 80; ++i) {
    for (int j = -120; j < 140; ++j) {
      // TODO: wrt world-tiles size
      const auto x = static_cast<float>(i) / 10 - 1 + dis(gen) * 0.1f;
      const auto y = static_cast<float>(j) / 10 - 1 + dis(gen) * 0.1f;
      const auto blade_height = height_dis(gen);

      blades.push_back({
          glm::vec4(x, 0, y, orientation_dis(gen)),
          glm::vec4(x, blade_height, y, blade_height),
          glm::vec4(x, blade_height, y, 0.1f),
          glm::vec4(0, blade_height, 0, 0.8f/*0.7f + dis(gen) * 0.3f*/)});
    }
  }
  return blades;
}

} // anonymous namespace

void Grass::Init() {
  const std::vector<Blade> blades = GenerateBlades();
  blades_num_ = static_cast<GLsizei>(blades.size());

  glPatchParameteri(GL_PATCH_VERTICES, 1);

  glGenVertexArrays(1, &grass_vao_);
  glBindVertexArray(grass_vao_);

  glGenBuffers(1, &grass_input_buffer_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_input_buffer_);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizei>(blades.size() * sizeof(Blade)),
               blades.data(), GL_STREAM_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grass_input_buffer_);

  glGenBuffers(1, &grass_output_buffer_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_output_buffer_);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizei>(blades.size() * sizeof(Blade)), nullptr,
               GL_STREAM_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grass_output_buffer_);

  grass_shader_.Bind();
  grass_shader_.SetUniform("tex_displacement", 0);

  /// we removed grass_output_buffer because we don't CPU <-> GPU;
  /// we removed indirect draw for the same reason (we update/draw in 1 call),
  ///   as well as its v0, v1, v2, up, dir config with glVertexAttribPointer()
}

void Grass::Render(float delta_time, const Texture& displacement_map,
                   float dmap_depth) {
  glBindVertexArray(grass_vao_);
  grass_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  displacement_map.Bind();
  grass_shader_.SetUniform("dmap_depth", dmap_depth);
  grass_shader_.SetUniform(
      "current_time", static_cast<float>(glfwGetTime()));
  grass_shader_.SetUniform("delta_time", static_cast<float>(delta_time / 1e3f)); // TODO: what's this
  grass_shader_.SetUniform("wind_magnitude", wind_magnitude_);
  grass_shader_.SetUniform("wind_wave_length", wind_wave_length_);
  grass_shader_.SetUniform("wind_wave_period", wind_wave_period_);

  if (!phase_1_) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grass_input_buffer_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grass_output_buffer_);
  } else {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grass_input_buffer_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grass_output_buffer_);
  }
  phase_1_ = !phase_1_;


  //TODO: fix amount; prev invocation was
  // glDispatchCompute(static_cast<GLuint>(blades_count_), 1, 1);
  // layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
  glPatchParameteri(GL_PATCH_VERTICES, 1);
  glDrawArraysInstanced(GL_PATCHES, 0, 1, blades_num_);
  glBindVertexArray(0);
}
void Grass::SetWindMagnitude(float wind_magnitude) {
  wind_magnitude_ += wind_magnitude;
}
void Grass::SetWindWaveLength(float wind_wave_length) {
  wind_wave_length_ += wind_wave_length;
}
void Grass::SetWindWavePeriod(float wind_wave_period) {
  wind_wave_period_ += wind_wave_period;
}
