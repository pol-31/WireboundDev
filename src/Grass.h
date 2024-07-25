#ifndef WIREBOUNDDEV_SRC_GRASS_H_
#define WIREBOUNDDEV_SRC_GRASS_H_

#include <chrono>
#include <string_view>

#include <glad/glad.h>

#include "Shader.h"
#include "Texture.h"

class Grass {
 public:
  Grass(
      std::string_view shader_vert_path,
      std::string_view shader_tessc_path,
      std::string_view shader_tesse_path,
      std::string_view shader_frag_path)
      : grass_shader_(shader_vert_path, shader_tessc_path,
                      shader_tesse_path, shader_frag_path) {
    Init();
  }

//  using DeltaDuration = std::chrono::duration<float, std::milli>;

  //TODO: because both Update() and Render() and solely GPU tasks,
  // there's no benefit to separate rendering into two calls:
  // anyway we will do this each frame. SO:
  //TODO: REMOVE compute shaders (bear functional into vertex/tessellation shader.
  // separate Update() and Render() useful only for CPU+GPU task

  // void Update(float delta_time);
  // glDispatchCompute(static_cast<GLuint>(blades_count_), 1, 1);
  void Render(float delta_time, const Texture& displacement_map,
              float dmap_depth);

  void SetWindMagnitude(float wind_magnitude);
  void SetWindWaveLength(float wind_wave_length);
  void SetWindWavePeriod(float wind_wave_period);

 private:
  void Init();

  GLuint grass_vao_{0};
  Shader grass_shader_;
  GLsizei blades_num_;

  GLuint grass_input_buffer_{0};
  GLuint grass_output_buffer_{0};

  bool phase_1_{false};

  // Wind parameters // TODO:
  float wind_magnitude_ = 1.0f;
  float wind_wave_length_ = 1.0f;
  float wind_wave_period_ = 1.0f;

};

#endif  // WIREBOUNDDEV_SRC_GRASS_H_
