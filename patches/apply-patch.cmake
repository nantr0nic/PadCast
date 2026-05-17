# This script applies the MSVC keyword fix to raylib-cpp
# On Windows with MSVC, 'near' and 'far' are often macro-defined as empty strings
# by windows.h (legacy 16-bit compatibility), causing "syntax error: ','" 
# This patch renames parameters to znear/zfar to avoid the conflict.
# On Linux/non-Windows, this script is safe - the replacements simply won't match.

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/Matrix.hpp" matrix_content)

# Check if the problem exists (parameter names are 'near' and 'far')
if(matrix_content MATCHES "double near, double far")
    message(STATUS "Patching raylib-cpp Matrix.hpp: renaming near/far parameters to znear/zfar")

    # Replace near/far parameter names with znear/zfar
    string(REGEX REPLACE 
        "static Matrix Frustum\\(double left, double right, double bottom, double top, double near, double far\\)"
        "static Matrix Frustum(double left, double right, double bottom, double top, double znear, double zfar)"
        matrix_content "${matrix_content}")

    string(REGEX REPLACE 
        "return ::MatrixFrustum\\(left, right, bottom, top, near, far\\)"
        "return ::MatrixFrustum(left, right, bottom, top, znear, zfar)"
        matrix_content "${matrix_content}")

    string(REGEX REPLACE 
        "static Matrix Perspective\\(double fovy, double aspect, double near, double far\\)"
        "static Matrix Perspective(double fovy, double aspect, double znear, double zfar)"
        matrix_content "${matrix_content}")

    string(REGEX REPLACE 
        "return ::MatrixPerspective\\(fovy, aspect, near, far\\)"
        "return ::MatrixPerspective(fovy, aspect, znear, zfar)"
        matrix_content "${matrix_content}")

    string(REGEX REPLACE 
        "static Matrix Ortho\\(double left, double right, double bottom, double top, double near, double far\\)"
        "static Matrix Ortho(double left, double right, double bottom, double top, double znear, double zfar)"
        matrix_content "${matrix_content}")

    string(REGEX REPLACE 
        "return ::MatrixOrtho\\(left, right, bottom, top, near, far\\)"
        "return ::MatrixOrtho(left, right, bottom, top, znear, zfar)"
        matrix_content "${matrix_content}")

    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/include/Matrix.hpp" "${matrix_content}")
else()
    message(STATUS "raylib-cpp Matrix.hpp patch not needed - parameters already use different names")
endif()

