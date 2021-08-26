set(USVDATA_HEADERS
    Vector2.h
    Angle.h
    )

set(USVDATA_SOURCES
    Frame.h Frame.cpp
    InputUtils.h InputUtils.cpp
    InputTypes.h CurvedPath.h
    CaseData.h CaseData.cpp
    Path.h Path.cpp
    Angle.cpp
    Defines.h
    Restrictions.h Restrictions.cpp
    UsvRun.h UsvRun.cpp
    FeatureCollection.h)

add_library(usvdata STATIC ${USVDATA_HEADERS} ${USVDATA_SOURCES})
set_property(TARGET usvdata PROPERTY CXX_STANDARD  17)
set_property(TARGET usvdata PROPERTY CXX_STANDARD_REQUIRED ON)

if (UNIX)
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules)
endif ()

if (WIN32)
    set(GEOGRAPHICLIB_LIB_TYPE "STATIC" CACHE STRING "Static GeographicLib")
    add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/geographiclib-code" "vendor/geographiclib" EXCLUDE_FROM_ALL)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/geographiclib-code/include)
    target_link_libraries(usvdata PUBLIC GeographicLib_STATIC)
else()
    find_package(GeographicLib REQUIRED)
    target_link_libraries(usvdata PUBLIC ${GeographicLib_LIBRARIES})
endif()
if (CMAKE_CROSSCOMPILING)
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
        target_link_libraries(usvdata PUBLIC -static-libgcc)
        target_link_libraries(usvdata PUBLIC -static-libstdc++)
        target_link_libraries(usvdata PUBLIC "-static -lpthread")
    endif ()
endif ()

set(SPOTIFY_JSON_BUILD_TESTS OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/spotify-json" "vendor/spotify-json" EXCLUDE_FROM_ALL)
if (WIN32)
    if (MSVC)
        target_compile_options(double-conversion PRIVATE "/MD$<$<CONFIG:Debug>:d>")
        target_compile_options("spotify-json" PRIVATE "/MD$<$<CONFIG:Debug>:d>")
        target_compile_options(double-conversion PRIVATE "/W0")
    else ()
        set_target_properties(double-conversion PROPERTIES COMPILE_OPTIONS "")
        set_target_properties("spotify-json" PROPERTIES COMPILE_OPTIONS "")
        target_compile_options("spotify-json" PRIVATE -msse -msse2 -mssse3 -msse4 -msse4.1 -msse4.2 -Wno-unused-variable)
    endif ()
endif ()

get_target_property(lib_include_dirs "spotify-json" INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(usvdata SYSTEM PRIVATE ${lib_include_dirs})
target_link_libraries(usvdata PRIVATE spotify-json)

if (MSVC)
    target_compile_options(usvdata PRIVATE /W4 /arch:SSE /arch:SSE2)
else ()
    target_compile_options(usvdata PRIVATE -Wall -Wextra -pedantic -Werror -msse -msse2 -mssse3 -msse4 -msse4.1 -msse4.2)
endif ()

