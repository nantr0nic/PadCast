# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PadCast")

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent module
include(FetchContent)

### Configure raylib build options -- reducing it as much as I can ###
# Core Modules
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)          # Don't build examples
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)             # Don't build games
set(SUPPORT_MODULE_RSHAPES ON CACHE BOOL "" FORCE)   # Basic shapes
set(SUPPORT_MODULE_RTEXTURES ON CACHE BOOL "" FORCE) # Texture support
set(SUPPORT_MODULE_RTEXT ON CACHE BOOL "" FORCE)     # Text rendering
set(SUPPORT_MODULE_RMODELS OFF CACHE BOOL "" FORCE)  # 3D models (not needed)
set(SUPPORT_MODULE_RAUDIO OFF CACHE BOOL "" FORCE)    # Audio support (not needed)

# Format Support Options
# Image formats (you'll want these for gamepad button images)
set(SUPPORT_FILEFORMAT_PNG ON CACHE BOOL "" FORCE)    # PNG support (so far only using PNGs)
set(SUPPORT_FILEFORMAT_JPG OFF CACHE BOOL "" FORCE)    # JPEG support (disabling this and rest)
set(SUPPORT_FILEFORMAT_BMP OFF CACHE BOOL "" FORCE)    # BMP support
set(SUPPORT_FILEFORMAT_TGA OFF CACHE BOOL "" FORCE)   # TGA format
set(SUPPORT_FILEFORMAT_DDS OFF CACHE BOOL "" FORCE)   # DDS format
set(SUPPORT_FILEFORMAT_HDR OFF CACHE BOOL "" FORCE)   # HDR format
set(SUPPORT_FILEFORMAT_KTX OFF CACHE BOOL "" FORCE)   # KTX format
set(SUPPORT_FILEFORMAT_ASTC OFF CACHE BOOL "" FORCE)  # ASTC format
set(SUPPORT_FILEFORMAT_PKM OFF CACHE BOOL "" FORCE)   # PKM format
set(SUPPORT_FILEFORMAT_PVR OFF CACHE BOOL "" FORCE)   # PVR format

# Disable SVG for compatibility reasons...
set(SUPPORT_FILEFORMAT_SVG OFF CACHE BOOL "" FORCE)  # This disables nanosvg dependency

# Font formats
set(SUPPORT_FILEFORMAT_FNT OFF CACHE BOOL "" FORCE)   # AngelCode font format
set(SUPPORT_FILEFORMAT_TTF ON CACHE BOOL "" FORCE)    # TrueType fonts (using this)

# Audio formats (disabling all)
set(SUPPORT_FILEFORMAT_WAV OFF CACHE BOOL "" FORCE)   # WAV audio
set(SUPPORT_FILEFORMAT_OGG OFF CACHE BOOL "" FORCE)   # OGG audio
set(SUPPORT_FILEFORMAT_XM OFF CACHE BOOL "" FORCE)    # XM audio
set(SUPPORT_FILEFORMAT_MOD OFF CACHE BOOL "" FORCE)   # MOD audio
set(SUPPORT_FILEFORMAT_FLAC OFF CACHE BOOL "" FORCE)  # FLAC audio
set(SUPPORT_FILEFORMAT_MP3 OFF CACHE BOOL "" FORCE)   # MP3 audio

# Graphics settings
set(GRAPHICS_API "GRAPHICS_API_OPENGL_33" CACHE STRING "" FORCE)
set(PLATFORM "Desktop" CACHE STRING "" FORCE)

### FetchContent stuff ###
# Fetch raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

# Fetch raylib-cpp
FetchContent_Declare(
    raylib_cpp
    GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
    GIT_TAG v5.5.0
)

# Make both available to all subprojects
FetchContent_MakeAvailable(raylib raylib_cpp)

###

# Include sub-projects.
add_subdirectory ("PadCast")
