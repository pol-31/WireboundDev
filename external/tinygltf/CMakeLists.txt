
configure_file(tiny_gltf.h tinygltf.cpp COPYONLY)
add_library(tinygltf STATIC tinygltf.cpp)
target_compile_definitions(tinygltf
        PRIVATE TINYGLTF_IMPLEMENTATION
        PRIVATE TINYGLTF_USE_RAPIDJSON
)
target_include_directories(tinygltf
        PRIVATE ../rapidjson/include
        PRIVATE ../stb
)
target_link_libraries(tinygltf PRIVATE stb)

add_dependencies(tinygltf stb)
