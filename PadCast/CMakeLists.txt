# Add source to this project's executable.
add_executable (PadCast
    "src/main.cpp"
    "src/PadCast.cpp" 
    "src/config.cpp"
    "src/winmain_proxy.cpp"
    "src/menus.cpp"
 )

### MSVC-specific optimizations to reduce false positives ###
if(MSVC)
    # Force Zi instead of ZI (Edit and Continue debug)
    target_compile_options(PadCast PRIVATE /Zi)

    target_compile_options(PadCast PRIVATE 
        /GL          # Whole program optimization
        /guard:cf    # Control Flow Guard (security feature)
        /Gy          # Enable function-level linking
    )
    target_link_options(PadCast PRIVATE 
        /LTCG        # Link-time code generation
        /GUARD:CF    # Control Flow Guard for linking
        /OPT:REF     # Eliminate unreferenced functions and data
        /OPT:ICF     # Identical COMDAT folding
    )
    
    # Use dynamic runtime library
    set_property(TARGET PadCast PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
    
    # Add version resource
    target_sources(PadCast PRIVATE
        app.manifest
        version.rc
    )
endif()
### End MSVC false positive mitigation section ###

### Cross-platform floating-point optimizations ###

# Save this for testing later when doing floating-point heavy stuff
# (e.g. animating joystick positions etc.)

#if(MSVC)
#    target_compile_options(PadCast PRIVATE
#        $<$<CONFIG:Release>:/fp:fast>
#    )
#elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(PadCast PRIVATE
#        $<$<CONFIG:Release>:-ffast-math>
#        $<$<CONFIG:Release>:-march=native>
#    )
#endif()


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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mini
)

# Include GNUInstallDirs to use standard installation path variables
include(GNUInstallDirs)

# Define the system data directory path for the application
target_compile_definitions(PadCast PRIVATE
    PADCAST_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/padcast"
)

# If using vcpkg stuff, add it here vvv

# Link libraries
target_link_libraries(PadCast PRIVATE raylib)
if(NOT BUILD_FOR_FLATPAK)
    target_link_libraries(PadCast PRIVATE raylib_cpp)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PadCast PROPERTY CXX_STANDARD 23)
endif()

# Copy resources (images for the controller and pushed buttons)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Copy configuration files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)

###################
### LINUX STUFF ###
###################

# Install binary
install(TARGETS PadCast DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install resources
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/padcast/resources
        FILES_MATCHING
        PATTERN "*.png")

# Install config template
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/config.ini 
        DESTINATION ${CMAKE_INSTALL_DATADIR}/padcast/config)

# Create desktop entry for Linux
if(UNIX AND NOT APPLE)
    # Install the icon in the standard location
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/padcast.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps)

    # Configure and install the desktop file
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/padcast.desktop.in"
        "${CMAKE_CURRENT_BINARY_DIR}/padcast.desktop"
        @ONLY
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/padcast.desktop"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    if (NOT BUILD_FOR_FLATPAK)
        # Install the metainfo file for packages other than flatpak because goddamn...
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/com.github.nantr0nic.PadCast.metainfo.xml
                DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    endif()
endif()

# CPack configuration for packaging
set(CPACK_GENERATOR "RPM;TGZ")

set(CPACK_PACKAGE_NAME "padcast")
set(CPACK_PACKAGE_VERSION "0.2.5")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PadCast - Gamepad visualization tool for streamers")
set(CPACK_PACKAGE_DESCRIPTION "PadCast is a lightweight gamepad visualization tool designed for streamers.
 It shows a real-time display of gamepad button presses, making it easy for viewers to see 
 what inputs are being made during gameplay.")
set(CPACK_PACKAGE_CONTACT "Andrew Khazanovsky <andykhazanovsky@gmail.com>")
set(CPACK_PACKAGE_VENDOR "Andrew Khazanovsky")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nantr0nic/PadCast")

# RPM-specific settings  
set(CPACK_RPM_PACKAGE_REQUIRES "raylib-devel")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Games")
set(CPACK_RPM_PACKAGE_LICENSE "BSD")
set(CPACK_RPM_PACKAGE_VENDOR "Andrew Khazanovsky")
set(CPACK_RPM_PACKAGE_URL "https://github.com/nantr0nic/PadCast")
set(CPACK_RPM_PACKAGE_DESCRIPTION "PadCast is a lightweight gamepad visualization tool designed for streamers.
 It shows a real-time display of gamepad button presses, making it easy for viewers to see 
 what inputs are being made during gameplay.")

 # Set the package file naming
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

set(CPACK_GENERATORS_INSTALL ON)
include(CPack)