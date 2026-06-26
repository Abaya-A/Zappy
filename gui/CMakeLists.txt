cmake_minimum_required(VERSION 3.15)

project(zappy_gui CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
    GL
    Shaders
    Trade
    Sdl2Application
)

find_package(MagnumPlugins REQUIRED
    GltfImporter
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
    src/state/Incantation.cpp
    src/state/Broadcast.cpp
    src/state/Expulsion.cpp
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
    src/render/BroadcastRenderer.cpp
    src/render/ExpulsionRenderer.cpp

    src/render/3d/RenderCamera3D.cpp
    src/render/3d/RenderGeometry3D.cpp
    src/render/3d/RenderMeshFactory3D.cpp
    src/render/3d/MapRenderer3D.cpp
    src/render/3d/PlanetCameraController.cpp
    src/render/3d/Model3D.cpp
    src/render/3d/ObjModelLoader3D.cpp
    src/render/3d/ModelRenderer3D.cpp
    src/render/3d/ResourceModelRenderer3D.cpp
    src/render/3d/PlanetGeometry3D.cpp
    src/render/3d/MtlMaterialLoader3D.cpp
    src/render/3d/GltfModelLoader3D.cpp

    src/render/3d/EggModelRenderer3D.cpp
    src/render/3d/PlayerModelRenderer3D.cpp
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
    Magnum::Trade
    MagnumPlugins::GltfImporter
)