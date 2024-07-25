cmake_minimum_required(VERSION 3.28)
project(WireboundDev)

cmake_policy(SET CMP0069 NEW) # LTO support
cmake_policy(SET CMP0091 NEW) # MSVC runtime support
cmake_policy(SET CMP0075 NEW) # honor to CMAKE_REQUIRED_LIBRARIES

# for WireboundDev/external/
cmake_policy(SET CMP0077 NEW) # honors normal variables

set(CMAKE_WARN_DEPRECATED OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external EXCLUDE_FROM_ALL)

add_executable(WireboundDev
        src/main.cpp
        src/Shader.cpp
#        src/text/TextRenderer.cpp
        src/Texture.cpp
        src/Grass.cpp

        src/models3d/ModelLoader.cpp
        src/models3d/AnimatableModel.cpp
        src/models3d/NonAnimatableModel.cpp
)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(WireboundDev
        PUBLIC ${EXTERNAL_DIR}/glad/include
        PUBLIC ${EXTERNAL_DIR}/glfw/include
        PUBLIC ${EXTERNAL_DIR}/utfcpp
        PUBLIC ${EXTERNAL_DIR}/stb
        PRIVATE ${CMAKE_SOURCE_DIR}/external/rapidjson
        PRIVATE ${CMAKE_SOURCE_DIR}/external/tinygltf
)

target_link_libraries(WireboundDev
        PUBLIC glad
        PUBLIC glfw
        PUBLIC stb
        PRIVATE tinygltf
)

# TODO: rapidjson don't like new gcc...
# c++23 installed here on my pc; you better to replace or just remove
#target_include_directories(WireboundDev
#        PUBLIC /usr/local/include/c++/14.1.0
#)
#target_link_directories(WireboundDev
#        PUBLIC /usr/local/lib/gcc/x86_64-pc-linux-gnu/14.1.0
#)

#set_target_properties(WireboundDev PROPERTIES
#        BUILD_RPATH "/usr/local/lib64"
#        INSTALL_RPATH "/usr/local/lib64"
#)
