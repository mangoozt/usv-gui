cmake_minimum_required(VERSION 3.5)

project(USV-gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt5_USE_STATIC_LIBS ON)
set(Qt5_USE_STATIC_RUNTIME ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

add_subdirectory(usvdata)
add_executable(USV-gui
               main.cpp
               mainwindow.cpp
               mainwindow.h
               mainwindow.ui
               oglwidget.h
               oglwidget.cpp
               text.cpp
               glgrid.cpp
               glsea.cpp
               resources.qrc
               earcut.h
               glrestrictions.cpp
               utils.h)

#set_target_properties(USV-gui PROPERTIES WIN32_EXECUTABLE TRUE)

file(GLOB glsl_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS glsl/*.vert glsl/*.frag)
foreach (glsl_file ${glsl_files})
    message(STATUS "Found " ${glsl_file})
    configure_file(${glsl_file} ${glsl_file} COPYONLY)
endforeach ()

target_link_libraries(USV-gui usvdata Qt5::Widgets Qt5::OpenGL)
