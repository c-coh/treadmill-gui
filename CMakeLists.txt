cmake_minimum_required(VERSION 3.28)
project(treadmill-gui LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(TGUI_BACKEND SFML_GRAPHICS CACHE STRING "TGUI backend to use")

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
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)

add_executable(main
  src/main.cpp
  src/core/TreadmillApp.cpp
  src/ui/panels/SpeedControlPanel.cpp
  src/ui/panels/TestingPanel.cpp
  src/ui/panels/DataPanel.cpp
  src/ui/ThemeManager.cpp
  src/utils/FileManager.cpp
  src/utils/SerialManager.cpp
  src/utils/TreadmillController.cpp
)

target_compile_features(main PRIVATE cxx_std_17)

if(MSVC)
  target_compile_options(main PRIVATE /FS)
endif()

target_include_directories(main PRIVATE
  src
  src/core
  src/ui
  src/ui/panels
  src/utils
)

target_link_libraries(main PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
  TGUI::TGUI
)

target_include_directories(main PRIVATE ${asio_SOURCE_DIR}/asio/include)

target_compile_definitions(main PRIVATE
  ASIO_STANDALONE
  _WIN32_WINNT=0x0A00
)
