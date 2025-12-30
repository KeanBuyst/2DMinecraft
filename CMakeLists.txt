cmake_minimum_required(VERSION 3.5)
project(2DMC)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED)

# Fetch SDL3
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.28
        GIT_SHALLOW TRUE
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL3)

# Fetch GLM (headers only library)
FetchContent_Declare(
        GLM
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(GLM)

# Fetch GLEW
IF (WIN32)
    FetchContent_Declare(
            GLEW
            URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
ELSE ()
    FetchContent_Declare(
            GLEW
            URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
ENDIF ()
FetchContent_MakeAvailable(GLEW)

if (NOT WIN32)
    # Build GLEW as a static library
    add_subdirectory(${glew_SOURCE_DIR}/build/cmake ${glew_BINARY_DIR})
endif()

# Add include directories
include_directories(${SDL2_SOURCE_DIR}/include)
include_directories(${glm_SOURCE_DIR}/glm)
include_directories(${glew_SOURCE_DIR}/include)

# Add the executable
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
        src/libs/stb_image.cpp
        src/Util.cpp
        src/world/generation/Light.cpp
        src/Constants.cpp
        src/world/generation/OreGeneration.cpp
        src/world/entities/EntityHandler.cpp
        src/world/entities/Entity.cpp
        src/Noise.cpp
        src/world/generation/CaveGeneration.cpp
        src/world/generation/BlockBorder.cpp
        src/world/entities/Item.cpp
        src/ui/Inventory.cpp
        src/ui/UI.cpp
        src/world/crafting/Crafting.cpp
        src/world/entities/Procedure.cpp
        src/world/entities/Sprite.cpp
        src/world/entities/Player.cpp
        src/world/entities/Player.h
        src/gl/Frame.cpp
        src/resources/Resources.cpp
)

# Link libraries based on the platform
if (WIN32)
    target_link_libraries(2DMC SDL3-static ${glew_SOURCE_DIR}/lib/Release/x64/glew32s.lib opengl32)
else()
    target_link_libraries(2DMC SDL3-static glew_s GL GLU pthread)
endif()
