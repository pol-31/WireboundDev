#include <iostream>

#include <fstream>
#include <sstream>

#include <stb_image.h>
#include "stb_image_write.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SetupWindow.h"
#include "models3d/ModelLoader.h"
#include "models3d/NonAnimatableModel.h"

#include "Details.h"
#include "Terrain.h"
#include "Cubemap.h"
#include "Grass.h"
#include "../assets/PoissonPoints.h"

struct ModelInfo {
  std::string path;
  glm::vec3 position;
  glm::vec3 rotation;
  float scale;
};

std::vector<ModelInfo> ParseModelFile(std::string_view path) {
  std::ifstream model_info_file(path.data());
  std::string buffer;
  std::getline(model_info_file, buffer); // skip columns names
  std::istringstream iss;

  std::vector<ModelInfo> models;
  while (std::getline(model_info_file, buffer)) {
    iss.clear();
    iss.str(buffer);
    ModelInfo model_info;
    std::getline(iss, model_info.path, ';');
    std::getline(iss, buffer, ';');
    model_info.scale = std::stof(buffer);

    std::getline(iss, buffer, ';');
    model_info.position.x = std::stof(buffer);
    std::getline(iss, buffer, ';');
    model_info.position.y = std::stof(buffer);
    std::getline(iss, buffer, ';');
    model_info.position.z = std::stof(buffer);

    std::getline(iss, buffer, ';');
    model_info.rotation.x = std::stof(buffer);
    std::getline(iss, buffer, ';');
    model_info.rotation.y = std::stof(buffer);
    std::getline(iss, buffer, ';');
    model_info.rotation.z = std::stof(buffer);
    models.push_back(model_info);
  }
  return models;
}

void RunLoop(GLFWwindow* window) {
  Shader shader_static_sprite("../shaders/StaticSprite.vert",
                              "../shaders/StaticSprite.frag");

  Texture tex_phrases("../assets/text/tex_en_prerendered_text.png");

  shader_static_sprite.Bind();
  shader_static_sprite.SetUniform("tex", 0);

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

  ModelLoader model_loader;
  auto models_info = ParseModelFile("/home/pavlo/Desktop/models.csv");
  std::vector<NonAnimatableModel> models;
  models.reserve(models_info.size());
  for (const auto& model :models_info) {
    models.emplace_back(model_loader.Load(model.path));
  }

  Shader model_shader("../shaders/Model.vert", "../shaders/Model.frag");
  model_shader.Bind();

  glm::mat4 proj_mat =
      glm::perspective(glm::radians(45.0f),
                       static_cast<float>(details::kWindowWidth)
                           / static_cast<float>(details::kWindowHeight),
                       0.01f, 1000.0f);

  std::vector<std::string_view> faces_sea {
      "../assets/sky/right.jpg", "../assets/sky/left.jpg",
      "../assets/sky/top.jpg", "../assets/sky/bottom.jpg",
      "../assets/sky/front.jpg", "../assets/sky/back.jpg"
  };
  std::vector<std::string_view> faces_hills {
      "../assets/sky/hills_ft.tga", "../assets/sky/hills_bk.tga",
      "../assets/sky/hills_up.tga", "../assets/sky/hills_dn.tga",
      "../assets/sky/hills_rt.tga", "../assets/sky/hills_lf.tga",
  };
  Cubemap cubemap(faces_hills, "../shaders/Skybox.vert", "../shaders/Skybox.frag");

  Grass grass(
      "../shaders/Grass.vert", "../shaders/Grass.tesc",
      "../shaders/Grass.tese", "../shaders/Grass.frag"
  );

  Terrain terrain(
      "../shaders/Terrain.vert", "../shaders/Terrain.tesc",
      "../shaders/Terrain.tese", "../shaders/Terrain.frag",
      "../assets/terrain/map_terrain_height.png",
//      "/home/pavlo/CLionProjects/WireboundDev/assets/terrain/map_terrain_normals.png"
      "../assets/terrain/map_terrain_material.png",
      "../assets/terrain/map_terrain_occlusion.png"
      );

  Texture dither_texture("../assets/map_test_placement_tree_rgba.png");
  Shader dither_shader("../shaders/Dithering.comp");
  dither_shader.Bind();

  GLuint dithered_texture_id;
  glGenTextures(1, &dithered_texture_id);
  glBindTexture(GL_TEXTURE_2D, dithered_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 1024, 1024, 0, GL_RED_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  Texture dithered_texture(dithered_texture_id, 1024, 1024, GL_RED_INTEGER);

  glBindImageTexture(0, dither_texture.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
  glBindImageTexture(1, dithered_texture.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
  glDispatchCompute((1024 + 15) / 16, (1024 + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


  Shader poisson_shader("../shaders/PoissonPoints.comp");

  GLuint poissonSSBO;
  glGenBuffers(1, &poissonSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, poissonSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(details::PoissonPoints), details::PoissonPoints, GL_DYNAMIC_COPY);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, poissonSSBO);

  poisson_shader.Bind();
  poisson_shader.SetUniform("threshold", 20.0f);
  poisson_shader.SetUniform("textureSize", 1024);

  GLuint placement_tex_id;
  glGenTextures(1, &placement_tex_id);
  glBindTexture(GL_TEXTURE_2D, placement_tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 1024, 1024, 0, GL_RED_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  Texture placement_texture(placement_tex_id, 1024, 1024, GL_RED_INTEGER);

  glBindImageTexture(2, placement_texture.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
  glBindImageTexture(1, dithered_texture.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8UI);
  glDispatchCompute(1018, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


  std::vector<unsigned char> data(1024 * 1024, 255);

  glBindTexture(GL_TEXTURE_2D, placement_tex_id);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_write_png("placement.png", 1024, 1024, 1, data.data(), 0);

  int passed_threshold{0};

  std::vector<glm::mat4> transforms;
  for (int i = 0; i < 1024 * 1024; ++i) {
    if (data[i] != 0) {
      ++passed_threshold;
      glm::mat4 transform(1.0f);
      auto pos = terrain.GetPosition(0, static_cast<int>(i & 1023), i >> 10);
      pos.y += models[1].GetDescent() * models_info[1].scale;
      transform = glm::translate(transform, pos);
//      transform = glm::translate(transform, 64.0f * glm::vec3(0.0f, 0.0f, i >> 10));
      transform = glm::scale(transform, {models_info[1].scale,
                                         1.0f,
                                         models_info[1].scale});
      transforms.push_back(transform);
//      std::cout << where.x << ' ' << where.y << ' ' << where.z << std::endl;
    }
//    if (transforms.size() > 10) {
//      break;
//    }
  }



  std::cout << "total passed: " << passed_threshold << std::endl;

  glActiveTexture(GL_TEXTURE0);
//  dithered_texture.Bind();
  placement_texture.Bind();

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0, details::kTexCoordsSize,
      details::kTexPositionDefault.data());
  glBufferSubData(
      GL_ARRAY_BUFFER, details::kTexPositionSize,
      details::kTexCoordsSize, details::kTexCoordsDefault.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(vao);
  shader_static_sprite.Bind();
  models[1].UpdatePositions(transforms);

  GLuint camera_uniform_buffer{0};
  glGenBuffers(1, &camera_uniform_buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, camera_uniform_buffer);
  glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_uniform_buffer);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_uniform_buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, camera_uniform_buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &proj_mat);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.7f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    ProcessInput(window);

    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
//    std::cout << "fps " << 60.0f / deltaTime << std::endl;
    last_frame = current_frame;

    model_shader.Bind();
    glm::mat4 view = camera.GetViewMatrix();


    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, camera_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &view);

    glActiveTexture(GL_TEXTURE7);
    placement_texture.Bind();

//    models[1].Render(model_shader);
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
      grass.SetWindMagnitude(1.0f);
//      grass.SetWindWaveLength(1.0f);
//      grass.SetWindWavePeriod(1.0f);
    } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
      grass.SetWindMagnitude(-1.0f);
//      grass.SetWindWaveLength(-1.0f);
//      grass.SetWindWavePeriod(-1.0f);
    }
    terrain.Render();
    cubemap.Render(view, proj_mat);

    grass.Render(delta_time, terrain.GetHeightMap(), terrain.GetDmapDepth());

    glfwPollEvents(); // TODO: is there correct order
    glfwSwapBuffers(window);

    // Мы сужаем множество значений параметра шероховатости до интервала 0.025 - 1.0,
    // т.к. идеально гладкие поверхности (шероховатость = 0.0) при прямом освещении имеют тенденцию выглядеть немного иначе
    /*std::vector<glm::mat4> light_transforms;
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
      glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
      newPos = lightPositions[i];
      std::string light_positions = "lightPositions[" + std::to_string(i) + "]";
      std::string light_colors = "lightColors[" + std::to_string(i) + "]";
      model_shader.SetUniformVec3(light_positions.c_str(), 1, glm::value_ptr(newPos));
      model_shader.SetUniformVec3(light_colors.c_str(), 1, glm::value_ptr(lightColors[i]));

      model = glm::mat4(1.0f);
      model = glm::translate(model, newPos);
      model = glm::scale(model, glm::vec3(0.5f));
      light_transforms.push_back(model);
    }
    tree.Render(light_transforms, model_shader);*/
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

int main() {
  auto window = SetupWindow();


  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  RunLoop(window);
  glfwTerminate();
  return 0;
}
