cmake_minimum_required(VERSION 3.5)

project(USV-gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
# OpenGL & GLEW library
find_package(OpenGL)


add_subdirectory(usvdata)
add_executable(USV-gui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    oglwidget.h
    oglwidget.cpp
    hellogles3.qrc
  )

target_link_libraries(USV-gui usvdata Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES})
