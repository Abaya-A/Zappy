cmake_minimum_required(VERSION 3.15)

project(zappy_gui CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
    GL
    Shaders
    Sdl2Application
)

set(ZAPPY_GUI_SOURCES
    src/main.cpp

    src/client/GuiArgs.cpp
    src/client/GuiClient.cpp

    src/net/NetworkClient.cpp

    src/protocol/ProtocolParser.cpp
    src/protocol/ProtocolCommand.cpp
    src/protocol/GuiProtocolDecoder.cpp
    src/protocol/GuiEventApplier.cpp

    src/state/Resource.cpp
    src/state/Tile.cpp
    src/state/Egg.cpp
    src/state/Player.cpp
    src/state/GameState.cpp

    src/render/MagnumRenderer.cpp
    src/render/RenderCamera.cpp
    src/render/RenderGeometry.cpp
    src/render/RenderMeshFactory.cpp
    src/render/MapRenderer.cpp
    src/render/ResourceRenderer.cpp
    src/render/EggRenderer.cpp
    src/render/PlayerRenderer.cpp
    src/render/IncantationRenderer.cpp
)

add_executable(zappy_gui
    ${ZAPPY_GUI_SOURCES}
)

target_include_directories(zappy_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(zappy_gui PRIVATE
    -Wall
    -Wextra
    -Werror
)

target_link_libraries(zappy_gui PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Shaders
)