cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME Simple3DEngine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Core/Render.cpp
        src/Core/Render.h
        src/Math/Vec2i.h
        src/Math/Vec3f.h
        src/Math/Mat4x4.h
        src/Math/Vec3i.h
        src/Math/Vec4f.h
        src/Math/MathFwd.h
        src/Core/Mesh.cpp
        src/Core/Mesh.h
        src/Core/ResourceManager.cpp
        src/Core/ResourceManager.h
        src/Platform/Abstract/Screen.h
        src/Platform/Windows/ScreenOpenGL.cpp
        src/Platform/Windows/ScreenOpenGL.h
        src/Platform/Platform.h
        src/Core/Transform.h
        src/Core/Camera.h
        src/Platform/Abstract/SystemTime.h
        src/Platform/Windows/TimeWindows.cpp
        src/Platform/Windows/TimeWindows.h
        src/Core/Time.h
        src/Core/Time.cpp
        src/Platform/Abstract/Application.h
        src/Platform/Windows/ApplicationWindows.h
        src/Platform/Platform.cpp
        src/Platform/Abstract/Input.h src/Platform/Windows/InputGLFW.h src/Platform/Windows/InputGLFW.cpp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/fast_obj)
target_link_libraries(${PROJECT_NAME} fast_obj)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)



add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

