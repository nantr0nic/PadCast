# Add source to this project's executable.
add_executable (PadCast
    "src/PadCast.cpp" 
    "src/win_transparency.cpp"
    "include/PadCast.h" 
    "include/mini/ini.h" 
    "include/config.h"
    "include/win_transparency.h"
    "src/config.cpp"
    "src/winmain_proxy.cpp"
)

# Suppress Log window for non-debug builds
set_target_properties(PadCast PROPERTIES
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
)

# Save this for later for Linux builds
#if (WIN32)
#  target_link_options(PadCast PRIVATE 
#    "$<$<NOT:$<CONFIG:Debug>>:-Wl,--subsystem,windows>"
#  )
#endif()

# Add include directories
target_include_directories(PadCast PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# If using vcpkg stuff, add it here vvv

# Link libraries
target_link_libraries(PadCast PRIVATE raylib raylib_cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PadCast PROPERTY CXX_STANDARD 23)
endif()

# Copy resources (images for the controller and pushed buttons)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Copy configuration files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)

# TODO: Add tests and install targets if needed.
