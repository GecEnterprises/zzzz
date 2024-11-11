project('zzzz', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++20', 'warning_level=3'])

portaudio_dep = dependency('portaudio-2.0', required : true)

imgui_dep = subproject('imgui').get_variable('imgui_dep')
glfw_dep = subproject('glfw3').get_variable('glfw_dep')
boost_dep = dependency('boost', modules: ['thread', 'atomic'])

opengl_dep = dependency('gl', required : true)

exe = executable(
  'zzzz', 
  'src/zzzz.cpp',
  sources: ['src/zzzz.cpp', 'src/gui.cpp', 'src/simpleaudio.cpp'],
  dependencies: [portaudio_dep, boost_dep, glfw_dep, opengl_dep, imgui_dep],
  install : true
)

test('basic', exe)
