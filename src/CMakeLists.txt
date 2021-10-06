add_subdirectory(usvdata)
option(NANOGUI_BACKEND OpenGL)
option(NANOGUI_BUILD_SHARED OFF)
option(NANOGUI_BUILD_PYTHON OFF)

set(NANOGUI_BUILD_EXAMPLES OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/nanogui" "vendor/nanogui" EXCLUDE_FROM_ALL)
set(GLM_TEST_ENABLE OFF)
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

if (UNIX)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
endif ()

include(${PROJECT_SOURCE_DIR}/modules/CMakeRC.cmake)
cmrc_add_resource_library(glsl_resources
                          glsl/general.vert glsl/glsea.frag
                          glsl/glsea.vert glsl/restrictions.frag
                          glsl/vessels.frag glsl/vessels.vert)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

if(MSVC)
    add_compile_options(/W4 /arch:SSE /arch:SSE2)
else()
    add_compile_options(-Wall -Wextra -pedantic -pedantic-errors -Werror -Wno-unknown-pragmas)
    add_compile_options(-msse -msse2 -mssse3 -msse4 -msse4.1 -msse4.2 -ffast-math)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif()

target_include_directories(usv-gui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/nanogui/include)
target_include_directories(usv-gui PRIVATE ${PROJECT_SOURCE_DIR}/vendor/nanogui/ext/glad/include)
target_link_libraries(usv-gui usvdata nanogui glm ${NANOGUI_LIBS} OpenGL::GL glsl_resources)

install(TARGETS usv-gui DESTINATION bin)
if (UNIX)
    set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    install(FILES resource/usv-gui.png DESTINATION share/icons)
    install(FILES resource/usv-gui.desktop DESTINATION share/applications)
endif ()

set(_fmt TGZ)
if (WIN32)
    set(_fmt ZIP)
endif ()
if (UNIX)
    set(_fmt DEB)
endif ()
set(CPACK_GENERATOR ${_fmt})
set(CPACK_SOURCE_GENERATOR ${_fmt})
#set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_CONTACT "Mark Tartagashev <mangoozt@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenGL-based visualization tool for USV results.")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
string(TOLOWER ${CMAKE_SYSTEM_NAME} _sys)
string(TOLOWER ${PROJECT_NAME} _project_lower)
set(CPACK_PACKAGE_FILE_NAME "${_project_lower}-${_sys}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${_project_lower}-${PROJECT_VERSION}")

set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgeographic19, libgl1, zenity")

include(CPack)


#endif()