cmake_minimum_required(VERSION 3.28)
project(zzzz)

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)


FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.7z
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    minimp3
    GIT_REPOSITORY https://github.com/lieff/minimp3
    GIT_PROGRESS TRUE
    GIT_SHALLOW 1
)

FetchContent_MakeAvailable(minimp3)

# Fetch RtAudio
FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG 6.0.1
    GIT_PROGRESS TRUE
    GIT_SHALLOW 1
)

set(RTAUDIO_BUILD_STATIC_LIBS true)

# Configure RtAudio options before making it available
if(WIN32)
    add_definitions(
        -DWIN32
        -D__WINDOWS_WASAPI__
    )
    set(RTAUDIO_API_WASAPI ON CACHE BOOL "Build WASAPI API" FORCE)
    set(RTAUDIO_API_DS OFF CACHE BOOL "Build DirectSound API" FORCE)
    set(RTAUDIO_API_ASIO OFF CACHE BOOL "Build ASIO API" FORCE)
    
    # Explicitly disable other APIs
    set(RTAUDIO_API_JACK OFF CACHE BOOL "Build JACK API" FORCE)
    set(RTAUDIO_API_ALSA OFF CACHE BOOL "Build ALSA API" FORCE)
    set(RTAUDIO_API_OSS OFF CACHE BOOL "Build OSS4 API" FORCE)
    set(RTAUDIO_API_PULSE OFF CACHE BOOL "Build PulseAudio API" FORCE)
    set(RTAUDIO_API_CORE OFF CACHE BOOL "Build CoreAudio API" FORCE)
else()
    set(RTAUDIO_API_ALSA ON CACHE BOOL "Build ALSA API" FORCE)
endif()

# Important: Make RtAudio available before other dependencies
FetchContent_MakeAvailable(rtaudio)

# Get the RtAudio source directory for later use
FetchContent_GetProperties(rtaudio)

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
set(CMAKE_CXX_STANDARD 20)
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
    target_link_libraries(imgui PRIVATE d3d11.lib dxgi.lib d3dcompiler.lib)
else()
    # Linux with OpenGL3 and GLFW
    set(CMAKE_CXX_COMPILER g++)
    add_compile_options(-Wall -Wextra)
    
    # Find OpenGL
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PRIVATE glfw OpenGL::GL dl)
    
    # Add OpenGL3 definitions
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
    
    # Find ALSA for RtAudio
    find_package(ALSA REQUIRED)
endif()

file(GLOB SOURCES 
    "src/*.cpp"
    "src/gui/*.cpp"
    "src/audio/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories for ImGui
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(NOT WIN32)
    target_include_directories(imgui PUBLIC ${GLFW_SOURCE_DIR}/include)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    imgui
    rtaudio
    Boost::container
    Boost::thread
)

# Include minimp3 headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${minimp3_SOURCE_DIR}  # Include directory of minimp3
)

# Include directories for the main project
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${rtaudio_SOURCE_DIR}
    ${rtaudio_BINARY_DIR}  # Add this for RtAudio config header
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)