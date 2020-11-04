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

add_library(usvdata STATIC ${USVDATA_HEADERS} ${USVDATA_SOURCES})

find_package(GeographicLib 1.50.1 REQUIRED STATIC)
target_link_libraries(usvdata PUBLIC ${GeographicLib_STATIC_LIBRARIES})

set(SPOTIFY_JSON_BUILD_TESTS OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/spotify-json" "vendor/spotify-json" EXCLUDE_FROM_ALL)
if(MSVC)
  target_compile_options(double-conversion PRIVATE "/MD$<$<CONFIG:Debug>:d>")
  target_compile_options("spotify-json" PRIVATE "/MD$<$<CONFIG:Debug>:d>")
  target_compile_options(double-conversion PRIVATE "/W0")
endif()
target_link_libraries(usvdata PUBLIC spotify-json)

set_target_properties(usvdata PROPERTIES $<$<BOOL:WIN32>:COMPILE_DEFINITIONS NOMINMAX>)

if(MSVC)
    target_compile_options(usvdata PRIVATE /W4)
else()
    target_compile_options(usvdata PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

