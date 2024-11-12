cmake_minimum_required(VERSION 3.15)
project(zzzz)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

# Fetch GLFW for Linux
if(NOT WIN32)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
        GIT_PROGRESS TRUE
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(glfw)
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
    GIT_PROGRESS TRUE
    GIT_SHALLOW 1
)

FetchContent_MakeAvailable(imgui)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific source files for ImGui
if(WIN32)
    set(PLATFORM_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
else()
    set(PLATFORM_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${PLATFORM_SOURCES}
)

# Platform-specific configuration
if(WIN32)
    # Windows with DirectX 11
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_GENERATOR "Visual Studio 17 2022")
    add_compile_options(/W4)

    target_link_libraries(imgui PRIVATE 
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
    )
else()
    # Linux with OpenGL3 and GLFW
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -Wextra)
    
    # Find OpenGL
    find_package(OpenGL REQUIRED)
    
    target_link_libraries(imgui PRIVATE 
        glfw
        OpenGL::GL
        dl
    )
    
    # Add OpenGL3 definitions
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()

file(GLOB SOURCES 
    "src/*.cpp"
    "src/gui/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(NOT WIN32)
    target_include_directories(imgui PUBLIC 
        ${GLFW_SOURCE_DIR}/include
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# Include directories for the main project
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)