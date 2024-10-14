cmake_minimum_required(VERSION 3.29)
project(2DMC)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Fetch SDL2
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2)

# Fetch GLM (headers only library)
FetchContent_Declare(
        GLM
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(GLM)

# Fetch GLEW
IF (WIN32)
    FetchContent_Declare(
            GLEW
            URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
    )
ELSE ()
    FetchContent_Declare(
            GLEW
            URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
    )
ENDIF ()
FetchContent_MakeAvailable(GLEW)

# Add include directories for GLM
include_directories(${SDL2_SOURCE_DIR}/include)
include_directories(${glm_SOURCE_DIR}/glm)
include_directories(${glew_SOURCE_DIR}/include)

# Add include directories for the project headers
include_directories("headers")

# Add the executable
add_executable(2DMC src/main.cpp src/Application.cpp
        src/world/World.cpp src/world/Region.cpp src/world/Chunk.cpp
        src/world/Block.cpp src/world/Biome.cpp src/resources/Texture.cpp
        src/resources/Storage.cpp src/resources/Shader.cpp src/resources/Metadata.cpp
        src/libs/stb_image.cpp src/Util.cpp
)

# Link libraries based on the platform
if (WIN32)
    target_link_libraries(2DMC SDL2-static ${glew_SOURCE_DIR}/lib/Release/x64/glew32s.lib opengl32)
else()
    target_link_libraries(2DMC SDL2-static GLEW GL pthread)
endif()