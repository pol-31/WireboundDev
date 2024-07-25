#include "ModelLoader.h"

#include <algorithm>
#include <iostream>

#include <glad/glad.h>

bool LoadImageData(
    tinygltf::Image *image, const int image_idx, std::string *err,
    std::string *warn, int req_width, int req_height,
    const unsigned char *bytes, int size, void *user_data) {
  (void)image;
  (void)image_idx;
  (void)err;
  (void)warn;
  (void)req_width;
  (void)req_height;
  (void)bytes;
  (void)size;
  (void)user_data;
  return true;
}

void ModelLoader::Init() {
  loader_.SetImageLoader(LoadImageData, nullptr);
}

ModelData ModelLoader::Load(std::string_view path) {
  std::string err;
  std::string warn;
  ModelData data;
//  bool res = loader_.LoadASCIIFromFile(
  bool res = loader_.LoadASCIIFromFile(
      &data.model, &err, &warn, path.data());
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }
  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }
  if (!res) {
    std::cout << "Failed to load glTF: " << path << std::endl;
  }

  std::filesystem::path dir_path(path);
  dir_path = dir_path.parent_path();
  LoadTextures(dir_path, data.model, data.material);

  BindModel(data);
  return data;
}

void ModelLoader::LoadTextures(const std::filesystem::path& dir_path,
                               tinygltf::Model& model, Material& material) {
  auto albedo_tex =
      model.materials[0].pbrMetallicRoughness.baseColorTexture.index;
  auto emission_tex =
      model.materials[0].emissiveTexture.index;
  auto metal_rough =
      model.materials[0].pbrMetallicRoughness.metallicRoughnessTexture.index;
  auto normal_tex =
      model.materials[0].normalTexture.index;
  auto occlusion_tex =
      model.materials[0].occlusionTexture.index;

  std::filesystem::path image_uri(dir_path);
  image_uri /= "filename"; // so then we could uniformly call replace_filename()

  if (albedo_tex != -1) {
    auto& albedo_texture = model.textures[albedo_tex];
    auto image_index = albedo_texture.source;
    if (image_index != -1) {
      image_uri.replace_filename(model.images[image_index].uri);
      std::cout << "Albedo texture URI: " << image_uri << std::endl;
      material.albedo = Texture(image_uri.string());
    } else {
      std::cout << "There's no albedo texture" << std::endl;
    }
  } else {
    std::cout << "There's no albedo texture" << std::endl;
  }

  if (emission_tex != -1) {
    auto& emission_texture = model.textures[emission_tex];
    auto image_index = emission_texture.source;
    if (image_index != -1) {
      image_uri.replace_filename(model.images[image_index].uri);
      std::cout << "Emission texture URI: " << image_uri << std::endl;
      material.emission = Texture(image_uri.string());
    } else {
      std::cout << "There's no emission texture" << std::endl;
    }
  } else {
    std::cout << "There's no emission texture" << std::endl;
  }

  if (metal_rough != -1) {
    auto& metallic_roughness_texture = model.textures[metal_rough];
    auto image_index = metallic_roughness_texture.source;
    if (image_index != -1) {
      image_uri.replace_filename(model.images[image_index].uri);
      std::cout << "Metallic-roughness metallic-roughness URI: "
                << image_uri << std::endl;
      material.metal_rough = Texture(image_uri.string());
    }
  } else {
    std::cout << "There's no metallic-roughness texture" << std::endl;
  }

  if (normal_tex != -1) {
    auto& normal_texture = model.textures[normal_tex];
    auto image_index = normal_texture.source;
    if (image_index != -1) {
      image_uri.replace_filename(model.images[image_index].uri);
      std::cout << "Normal texture URI: " << image_uri << std::endl;
      material.normal = Texture(image_uri.string());
    } else {
      std::cout << "There's no normal texture" << std::endl;
    }
  } else {
    std::cout << "There's no normal texture" << std::endl;
  }

  if (occlusion_tex != -1) {
    auto& occlusion_texture = model.textures[occlusion_tex];
    auto image_index = occlusion_texture.source;
    if (image_index != -1) {
      image_uri.replace_filename(model.images[image_index].uri);
      std::cout << "Occlusion texture URI: " << image_uri << std::endl;
      material.occlusion = Texture(image_uri.string());
    } else {
      std::cout << "There's no occlusion texture" << std::endl;
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }
}

///TODO: unused by now, but useful for calculating bounding shapes like capsule
std::vector<glm::vec3> ModelLoader::CollectModelVertices(tinygltf::Model model) {
  std::vector<glm::vec3> vertices;
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == GL_ARRAY_BUFFER) {
      const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
      glm::vec3 value;
      for (size_t j = 0; j < bufferView.byteLength; j += sizeof(glm::vec3)) {
        std::memcpy(&value, &buffer.data[j], sizeof(glm::vec3));
        vertices.push_back(value);
      }
    }
  }
  return vertices;
}

void ModelLoader::BindMesh(ModelData& data, tinygltf::Mesh &mesh) {
  tinygltf::Model& model = data.model;
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {
      /*Unsupported bufferView (from spec2.0 readme):
       * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
       * ... drawArrays function should be used with a count equal to
       * the count property of the accessors referenced by the
       * attributes property (they are all equal for a given primitive)
       */
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

    GLuint vbo;
    glGenBuffers(1, &vbo);
    data.vbo_and_ebos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, data.vbo_and_ebos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      /** according to GLTF-2.0 spec POSITION is always VEC3 float
       * and min/max always present, so we can directly use it
       * */
      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) {
        vaa = 0;
        //TODO: bear out to function (it's specified by gltf-spec)
        data.aabb.min.x = accessor.minValues[0];
        data.aabb.min.y = accessor.minValues[1];
        data.aabb.min.z = accessor.minValues[2];
        data.aabb.max.x = accessor.maxValues[0];
        data.aabb.max.y = accessor.maxValues[1];
        data.aabb.max.z = accessor.maxValues[2];
        data.descent = 0.0f - data.aabb.min.y;
      }
      if (attrib.first.compare("NORMAL") == 0) {
        vaa = 1;
      }
      if (attrib.first.compare("TEXCOORD_0") == 0) {
        vaa = 2;
      }
      if (attrib.first.compare("JOINTS_0") == 0) {
        vaa = 3;
      }
      if (attrib.first.compare("WEIGHTS_0") == 0) {
        vaa = 4;
      }

      //TODO:
      /* GLTF specification directly specify format and type of data,
       * so we can not rely on tinygltf structures, BUT we can write
       * simple script-validator to check validity of all models.
       * gltf-spec states: position should have min,max properties, so we
       * don't need to calculate aabb on our own.
       *
       *
       * */

      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        if (accessor.normalized) {
          std::cerr << "normalization for model required" << std::endl;
        }
        glVertexAttribPointer(
            vaa, size, accessor.componentType, GL_FALSE,
            byteStride, reinterpret_cast<void*>(accessor.byteOffset));
      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }
  }
}

void ModelLoader::BindModelNodes(ModelData& data, tinygltf::Node &node) {
  tinygltf::Model& model = data.model;
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(data, model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(data, model.nodes[node.children[i]]);
  }
}

void ModelLoader::BindModel(ModelData& data) {
  glGenVertexArrays(1, &data.vao);
  glBindVertexArray(data.vao);

  tinygltf::Model& model = data.model;
  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(data, model.nodes[scene.nodes[i]]);
  }
  glBindVertexArray(0);
  std::map<int, GLuint>& vbos = data.vbo_and_ebos;
  // cleanup vbos but do not delete index buffers yet
  for (auto it = vbos.cbegin(); it != vbos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &vbos[it->first]);
      vbos.erase(it++);
    } else {
      ++it;
    }
  }
}

