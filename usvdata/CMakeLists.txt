set(USVDATA_HEADERS
        Vector2.h
        )

set(USVDATA_SOURCES
        Frame.h Frame.cpp
        Geometry.h Geometry.cpp
        InputUtils.h InputUtils.cpp
        InputTypes.h InputTypes.cpp
        CurvedPath.h CurvedPath.cpp
        Path.h Path.cpp
        Angle.h Angle.cpp
        Defines.h
        )

#include_directories(include)

add_library(usvdata STATIC ${USVDATA_HEADERS} ${USVDATA_SOURCES})

find_package(GeographicLib 1.50.1 REQUIRED)
target_link_libraries(usvdata ${GeographicLib_LIBRARIES})


#if (WIN32)
#    set_target_properties(usvdata PROPERTIES COMPILE_DEFINITIONS NOMINMAX)
#endif ()
