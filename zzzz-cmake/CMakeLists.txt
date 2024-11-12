cmake_minimum_required(VERSION 3.15)
project(zzzz)

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
)

FetchContent_MakeAvailable(imgui)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)

# Set compiler based on platform
if(WIN32)
    # Ensure we're using Visual Studio 2022
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_GENERATOR "Visual Studio 17 2022")
    # MSVC-specific options
    add_compile_options(/W4)

    target_link_libraries(imgui PRIVATE 
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
    )
else()
    # Use Clang on Linux
    set(CMAKE_CXX_COMPILER clang++)
    # Clang-specific options
    add_compile_options(-Wall -Wextra)
endif()

# Find all source files in src directory
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})


target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# Include directories if needed
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)