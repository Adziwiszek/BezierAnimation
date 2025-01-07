cmake_minimum_required(VERSION 3.3)
project(BezierAnimation LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(SOURCES
    src/main.cpp
    src/point.cpp
    src/bcurve.cpp
    src/user.cpp
    src/frame.cpp
    src/drawer.cpp
    src/inputHandler.cpp
    src/animationManager.cpp
    src/animationState.cpp
)

set(HEADERS
  include/point.hpp
  include/bcurve.hpp
  include/user.hpp
  include/drawer.hpp
  include/inputHandler.hpp
  include/userUtils.hpp
  include/animationManager.hpp
  include/animationState.hpp
  include/uiManager.hpp
)

add_executable(BezierAnimation ${SOURCES} ${HEADERS})
target_link_libraries(BezierAnimation PRIVATE sfml-graphics sfml-window sfml-system)
target_compile_features(BezierAnimation PRIVATE cxx_std_23)

if(WIN32)
    add_custom_command(
        TARGET BezierAnimation 
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
