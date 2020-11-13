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
    resources.qrc
    )

set_property(TARGET USV-gui PROPERTY WIN32_EXECUTABLE true)

target_link_libraries(USV-gui usvdata Qt5::Widgets Qt5::OpenGL)
