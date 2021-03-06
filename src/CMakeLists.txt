add_subdirectory(usvdata)
option(NANOGUI_BACKEND OpenGL)
option(NANOGUI_BUILD_SHARED OFF)
option(NANOGUI_BUILD_PYTHON OFF)

add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/nanogui" "vendor/nanogui" EXCLUDE_FROM_ALL)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/glm" "vendor/glm" EXCLUDE_FROM_ALL)
add_compile_definitions(USV_GUI_VERSION=\"${CMAKE_PROJECT_VERSION}\")

add_executable(usv-gui
               main.cpp
               oglwidget.h
               oglwidget.cpp
               glgrid.cpp
               glsea.cpp
               earcut.h
               glrestrictions.cpp
               Program.cpp
               Program.h
               Buffer.cpp Buffer.h
               # Unfortunately need to do this
               ${PROJECT_SOURCE_DIR}/vendor/nanogui/ext/glad/src/glad.c
               main.rc
               MyScreen.cpp MyScreen.h
               ui/IgnorantTextBox.h
               ui/ScrollableSlider.cpp ui/ScrollableSlider.h
               App.cpp App.h
               Compass.cpp Compass.h
               ui/SettingsWindow.cpp ui/SettingsWindow.h
               glvessels.cpp glvessels.h)

set_property(TARGET usv-gui PROPERTY CXX_STANDARD 17)
set_property(TARGET usv-gui PROPERTY CXX_STANDARD_REQUIRED ON)

include(${PROJECT_SOURCE_DIR}/modules/CMakeRC.cmake)
cmrc_add_resource_library(glsl_resources
                          glsl/general.vert glsl/glsea.frag
                          glsl/glsea.vert glsl/restrictions.frag
                          glsl/vessels.frag glsl/vessels.vert)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

target_include_directories(usv-gui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/nanogui/include)
target_include_directories(usv-gui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/nanogui/ext/glad/include)
target_link_libraries(usv-gui usvdata nanogui glm ${NANOGUI_LIBS} OpenGL::GL glsl_resources)

install(TARGETS usv-gui DESTINATION bin)
