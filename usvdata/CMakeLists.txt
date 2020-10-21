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
        )

#include_directories(include)

add_library(usvdata STATIC ${USVDATA_HEADERS} ${USVDATA_SOURCES})

find_package(GeographicLib 1.50.1 REQUIRED)
target_link_libraries(usvdata ${GeographicLib_LIBRARIES})
set(SPOTIFY_JSON_BUILD_TESTS OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/spotify-json" "vendor/spotify-json" EXCLUDE_FROM_ALL)
target_link_libraries(usvdata spotify-json)

#if (WIN32)
#    set_target_properties(usvdata PROPERTIES COMPILE_DEFINITIONS NOMINMAX)
#endif ()
