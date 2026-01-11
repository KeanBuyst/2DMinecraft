cmake_minimum_required(VERSION 3.15)

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/triplets" CACHE PATH "Custom triplets")
set(VCPKG_TARGET_TRIPLET "x64-mingw-mixed" CACHE STRING "Force custom mixed triplet")

project(2DMC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(directx-dxc CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_hlsl CONFIG REQUIRED)
find_package(spirv_cross_msl CONFIG REQUIRED)
find_package(spirv_cross_cpp CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)
find_package(spirv_cross_c CONFIG REQUIRED)

add_library(spirv_cross_c_shared ALIAS spirv-cross-c)

add_executable(2DMC
        src/main.cpp
        src/Application.cpp
        src/world/World.cpp
        src/world/Region.cpp
        src/world/Chunk.cpp
        src/world/Block.cpp
        src/world/Biome.cpp
        src/gl/Texture.cpp
        src/resources/Storage.cpp
        src/gl/Shader.cpp
        src/resources/Metadata.cpp
        src/Util.cpp
        src/world/generation/Light.cpp
        src/Constants.cpp
        src/world/generation/OreGeneration.cpp
        src/Noise.cpp
        src/world/entities/EntityHandler.cpp
        src/world/entities/Entity.cpp
        src/world/generation/CaveGeneration.cpp
        src/world/generation/BlockBorder.cpp
        src/world/entities/Item.cpp
#[[        src/ui/Inventory.cpp
        src/ui/UI.cpp
        src/world/crafting/Crafting.cpp]]
        src/world/entities/Procedure.cpp
        src/world/entities/Sprite.cpp
        src/world/entities/Player.cpp
        src/gl/Frame.cpp
        src/resources/Resources.cpp
        src/libs/SDL_shadercross.c
        src/libs/stb_image.cpp
)

# tick shadercross to seeing DXC enabled
target_compile_definitions(2DMC PRIVATE SDL_SHADERCROSS_DXC=1)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

target_link_libraries(2DMC PRIVATE
        glm::glm
        SDL3::SDL3
        Microsoft::DirectXShaderCompiler
        imgui::imgui

        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-msl
        spirv-cross-cpp
        spirv-cross-reflect
        spirv-cross-c
)