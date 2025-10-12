cmake_minimum_required(VERSION 3.28)
project(treadmill-gui LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.2
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
  TGUI
  GIT_REPOSITORY https://github.com/texus/TGUI.git
  GIT_TAG v1.11.0
)
FetchContent_MakeAvailable(TGUI)

FetchContent_Declare(
  TGUI
  GIT_REPOSITORY https://github.com/texus/TGUI.git
  GIT_TAG v1.11.0
)

add_executable(main
  src/main.cpp
  src/TreadmillApp.cpp
  src/SpeedControlPanel.cpp
  src/DataPanel.cpp
  src/ThemeManager.cpp
)

target_compile_features(main PRIVATE cxx_std_17)

# Add the src directory to the include path so header files can be found
target_include_directories(main PRIVATE src)

target_link_libraries(main PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
  TGUI::TGUI
)
