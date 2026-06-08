# Zappy GUI Architecture

## Overview

The Zappy GUI is responsible for connecting to the Zappy server, authenticating as the graphical client, decoding server protocol messages, maintaining a local representation of the game state, and eventually rendering that state visually.

The current GUI architecture is designed around a typed protocol event pipeline.

The main idea is:

Network data should not directly mutate the GUI state.

Instead, raw server lines are progressively transformed into safer, typed C++ objects before being applied to the local GameState.

Current pipeline:

Protocol line
-> ProtocolParser
-> ProtocolCommand
-> GuiProtocolDecoder
-> GuiProtocolEvent
-> GuiEventApplier
-> GameState
-> Renderer

This keeps networking, protocol decoding, state mutation, and rendering separated.

---

## High-level flow

When the GUI receives data from the server, the flow is:

1. NetworkClient receives raw bytes from the TCP socket.
2. NetworkClient buffers data and extracts complete newline-terminated protocol lines.
3. GuiClient receives each line.
4. GuiClient handles the WELCOME bootstrap case.
5. ProtocolParser converts the raw line into a ProtocolCommand.
6. GuiProtocolDecoder converts the ProtocolCommand into a typed GuiProtocolEvent.
7. GuiEventApplier applies the typed event to GameState.
8. The renderer reads GameState through const accessors and draws the world.

The renderer should never parse protocol strings.

The decoder should never mutate GameState.

GameState should never know about raw protocol command names such as msz, bct, pnw, ppo, pin, or enw.

---

## Main components

### GuiClient

GuiClient is the central runtime object for the graphical client.

Responsibilities:

- own the network client
- own the protocol parser
- own the protocol decoder
- own the event applier
- own the GameState
- connect to the server
- run the main receive loop
- handle the initial WELCOME handshake
- send GRAPHIC authentication
- send initial bootstrap requests
- forward decoded events to the applier

GuiClient should not contain protocol-specific business logic.

It should not manually update players, eggs, tiles, or resources.

Current expected bootstrap sequence:

- receive WELCOME
- send GRAPHIC
- send msz
- send mct
- send tna
- send sgt

---

### NetworkClient

NetworkClient owns the TCP socket connection.

Responsibilities:

- resolve the server host
- connect to the server port
- set the socket as non-blocking
- receive available bytes
- buffer partial data
- split complete protocol lines by newline
- send complete lines to the server
- handle disconnects and socket errors

Important design note:

NetworkClient should only know about bytes, lines, and socket state.

It should not know about Zappy protocol semantics.

It should not know what msz, bct, pnw, or GRAPHIC mean.

---

### ProtocolParser

ProtocolParser converts one complete raw protocol line into a ProtocolCommand.

Example:

Raw line:

pnw #4 2 3 1 5 teamA

Parsed ProtocolCommand:

- name: pnw
- args: ["#4", "2", "3", "1", "5", "teamA"]
- raw: "pnw #4 2 3 1 5 teamA"

Responsibilities:

- split a line into command name and arguments
- preserve the original raw line
- return std::nullopt for empty or invalid lines

ProtocolParser does not validate the meaning of the command.

It only performs lexical parsing.

---

### ProtocolCommand

ProtocolCommand is a small wrapper around a parsed protocol line.

Responsibilities:

- expose command name
- expose argument list
- expose raw line
- provide helper methods for argument parsing

Useful methods:

- name()
- args()
- raw()
- is(commandName)
- hasArgCount(count)
- arg(index)
- intArg(index)
- idArg(index)

The idArg method is used for protocol identifiers written with a leading #.

Example:

#42 -> 42

This is used for player IDs and egg IDs.

---

## Typed protocol event pipeline

The GUI uses a typed event pipeline to avoid spreading raw protocol parsing everywhere.

The pipeline is:

ProtocolCommand
-> GuiProtocolDecoder
-> GuiProtocolEvent
-> GuiEventApplier
-> GameState

This design combines:

- Adapter pattern
- Event pattern
- Visitor pattern through std::variant and std::visit

Short name:

Typed Protocol Event Pipeline

---

## GuiProtocolEvent

GuiProtocolEvent is a std::variant containing all typed protocol events currently supported by the GUI.

Each event struct is a simple data carrier.

Example:

struct PlayerPositionEvent {
    int id;
    int x;
    int y;
    int orientation;
};

These structs should stay simple.

They should not mutate state.

They should not know how to render themselves.

They should not parse raw protocol strings.

They only represent already-decoded protocol information.

---

## Supported event categories

### Map and tile events

MapSizeEvent

Represents:

msz X Y

Used to initialize or update the map dimensions.

TileContentEvent

Represents:

bct X Y q0 q1 q2 q3 q4 q5 q6

Used to update one tile's resource content.

The seven resource quantities are:

- q0: food
- q1: linemate
- q2: deraumere
- q3: sibur
- q4: mendiane
- q5: phiras
- q6: thystame

---

### Team and server events

TeamNameEvent

Represents:

tna N

Used to register one team name.

TimeUnitEvent

Represents:

sgt T
sst T

Used to store the current server time unit.

EndGameEvent

Represents:

seg N

Used to store the winning team.

ServerMessageEvent

Represents:

smg M

Used to log a server message.

UnknownCommandEvent

Represents:

suc

Used when the server reports an unknown command.

BadParameterEvent

Represents:

sbp

Used when the server reports a bad command parameter.

---

### Player events

PlayerNewEvent

Represents:

pnw #n X Y O L N

Used when a new player appears.

Fields:

- id
- x
- y
- orientation
- level
- teamName

PlayerPositionEvent

Represents:

ppo #n X Y O

Used when a player position or orientation changes.

PlayerLevelEvent

Represents:

plv #n L

Used when a player level changes.

PlayerInventoryEvent

Represents:

pin #n X Y q0 q1 q2 q3 q4 q5 q6

Used when a player's inventory is updated.

PlayerDeathEvent

Represents:

pdi #n

Used when a player dies and must be removed from GameState.

---

### Egg events

PlayerForkEvent

Represents:

pfk #n

Used when a player starts laying an egg.

For now, this is a log-only event.

EggNewEvent

Represents:

enw #e #n X Y

Used when a new egg is created.

Fields:

- egg id
- player id
- x
- y

EggHatchEvent

Represents:

ebo #e

Used when an egg hatches.

Currently, this removes the egg from GameState.

EggDeathEvent

Represents:

edi #e

Used when an egg dies.

Currently, this removes the egg from GameState.

---

### Resource action events

PlayerDropResourceEvent

Represents:

pdr #n i

Used when a player drops a resource.

For now, this is mostly a visual/log event.

Tile state is expected to be synchronized through bct messages.

PlayerCollectResourceEvent

Represents:

pgt #n i

Used when a player collects a resource.

For now, this is mostly a visual/log event.

Tile and inventory state remain synchronized through bct and pin messages.

---

### Action events

PlayerExpulsionEvent

Represents:

pex #n

Used when a player expels other players.

Currently log-only.

PlayerBroadcastEvent

Represents:

pbc #n M

Used when a player broadcasts a message.

The message can contain spaces, so the decoder rebuilds the message from all arguments after the player id.

IncantationStartEvent

Represents:

pic X Y L #n #n ...

Used when an incantation starts on a tile.

Fields:

- x
- y
- level
- playerIds

IncantationEndEvent

Represents:

pie X Y R

Used when an incantation ends.

Fields:

- x
- y
- result

The result indicates whether the incantation succeeded or failed.

---

## GuiProtocolDecoder

GuiProtocolDecoder converts a ProtocolCommand into a GuiProtocolEvent.

Responsibilities:

- dispatch by command name
- validate argument count
- parse integers
- parse player IDs and egg IDs
- parse resource arrays
- parse inventory arrays
- build typed event structs
- reject unsupported or malformed commands

GuiProtocolDecoder does not mutate GameState.

GuiProtocolDecoder does not print rendering output.

GuiProtocolDecoder does not own any persistent game data.

---

## Decoder dispatch table

GuiProtocolDecoder uses a dispatch table instead of a long if chain.

Conceptually:

msz -> decodeMapSize
bct -> decodeTileContent
pnw -> decodePlayerNew
ppo -> decodePlayerPosition
pin -> decodePlayerInventory
enw -> decodeEggNew

This keeps decode() simple and stable.

When adding a new protocol command, the steps are:

1. Add a typed event struct in GuiProtocolEvent.hpp.
2. Add a decode method declaration in GuiProtocolDecoder.hpp.
3. Register the command name in registerDecoders().
4. Implement the decode method in GuiProtocolDecoder.cpp.
5. Add an applyEvent overload in GuiEventApplier.

---

## GuiEventApplier

GuiEventApplier applies typed protocol events to GameState.

It uses std::visit on GuiProtocolEvent.

Responsibilities:

- receive a typed event
- call the correct GameState method
- log successful state updates
- return false if GameState rejects the update

Example mappings:

MapSizeEvent
-> GameState::setMapSize

TileContentEvent
-> GameState::setTileContent

TeamNameEvent
-> GameState::addTeam

TimeUnitEvent
-> GameState::setTimeUnit

EndGameEvent
-> GameState::setWinner

PlayerNewEvent
-> GameState::addPlayer

PlayerPositionEvent
-> GameState::updatePlayerPosition

PlayerLevelEvent
-> GameState::updatePlayerLevel

PlayerInventoryEvent
-> GameState::updatePlayerInventory

PlayerDeathEvent
-> GameState::removePlayer

EggNewEvent
-> GameState::addEgg

EggHatchEvent
-> GameState::removeEgg

EggDeathEvent
-> GameState::removeEgg

Some events are currently log-only:

- PlayerForkEvent
- PlayerDropResourceEvent
- PlayerCollectResourceEvent
- PlayerExpulsionEvent
- PlayerBroadcastEvent
- IncantationStartEvent
- IncantationEndEvent
- UnknownCommandEvent
- BadParameterEvent

These may later be used by the renderer for animations, effects, or debug panels.

---

## GameState

GameState owns the persistent local representation of the game.

Responsibilities:

- store map dimensions
- store tile contents
- store team names
- store current time unit
- store winner
- store players
- store eggs
- validate state updates
- expose read-only access for rendering

GameState should not know about raw protocol command names.

It should expose meaningful methods such as:

- setMapSize
- setTileContent
- addTeam
- setTimeUnit
- setWinner
- addPlayer
- updatePlayerPosition
- updatePlayerLevel
- updatePlayerInventory
- removePlayer
- addEgg
- removeEgg

GameState is the source of truth for the renderer.

---

## GameState read API

The renderer should read GameState through const accessors.

Important read methods:

- width()
- height()
- timeUnit()
- isReady()
- hasWinner()
- winner()
- tileAt(x, y)
- tiles()
- teams()
- players()
- eggs()

The renderer should not mutate GameState.

The renderer should receive:

const GameState &state

This keeps rendering separate from state management.

---

## Entity classes

### Tile

Tile stores resource quantities for one map tile.

Responsibilities:

- store seven resource quantities
- validate quantities are non-negative
- expose resource totals
- expose whether the tile is empty

Tile does not know about protocol commands or rendering.

---

### Player

Player stores one player entity.

Responsibilities:

- store player id
- store position
- store orientation
- store level
- store team name
- store inventory
- validate player-local updates

Player does not know about protocol commands.

Player does not own the player collection.

GameState owns all Player instances.

---

### Egg

Egg stores one egg entity.

Responsibilities:

- store egg id
- store parent player id
- store position
- validate egg-local values

Egg does not know about protocol commands.

GameState owns all Egg instances.

---

## Renderer design direction

The next major component should be GuiRenderer.

Recommended design:

class GuiRenderer {
public:
    bool initialize();
    bool isOpen() const;
    void handleEvents();
    void render(const GameState &state);
};

Responsibilities:

- create and own the SFML window
- handle window events
- draw a waiting screen when GameState is not ready
- draw the map grid when GameState is ready
- draw tile resources
- draw players
- draw eggs
- draw basic debug information
- never mutate GameState

The renderer should only depend on const GameState.

---

## Expected future rendering flow

Eventually GuiClient::run() should look conceptually like:

while running:
    poll network lines
    parse/decode/apply protocol events
    renderer.handleEvents()
    renderer.render(state)

This keeps networking and rendering in the same main loop initially.

A more advanced threaded design is not needed yet.

---

## Testing strategy

The non-visual GUI logic should be tested before rendering.

Recommended tests:

### Protocol decoder tests

Validate that raw ProtocolCommand objects decode into the expected GuiProtocolEvent alternatives.

Examples:

- msz -> MapSizeEvent
- bct -> TileContentEvent
- pnw -> PlayerNewEvent
- pin -> PlayerInventoryEvent
- enw -> EggNewEvent
- pbc -> PlayerBroadcastEvent
- invalid argument counts return std::nullopt

### Event applier tests

Validate that events correctly mutate GameState.

Examples:

- MapSizeEvent updates width, height, and readiness
- PlayerNewEvent adds a player
- PlayerPositionEvent updates a player
- PlayerDeathEvent removes a player
- EggNewEvent adds an egg
- EggHatchEvent removes an egg

### GameState read API tests

Validate renderer-facing accessors.

Examples:

- state is not ready before map size
- state is ready after map size
- tiles().size() equals width * height
- tileAt returns nullptr for invalid positions
- players(), eggs(), and teams() expose read-only collections

---

## Adding a new protocol command

To add a new protocol command, follow this checklist:

1. Add a new event struct in GuiProtocolEvent.hpp.
2. Add the event type to the GuiProtocolEvent variant.
3. Add a decoder method declaration in GuiProtocolDecoder.hpp.
4. Register the command in GuiProtocolDecoder::registerDecoders().
5. Implement the decoder method in GuiProtocolDecoder.cpp.
6. Add an applyEvent overload declaration in GuiEventApplier.hpp.
7. Implement the applyEvent overload in GuiEventApplier.cpp.
8. Add or update GameState methods if the event changes persistent state.
9. Add tests for the decoder and applier.
10. Build with make zappy_gui.
11. Run GUI tests.

---

## Design rules

The GUI architecture should follow these rules:

1. NetworkClient only handles sockets and lines.
2. ProtocolParser only creates ProtocolCommand objects.
3. ProtocolCommand only exposes parsed command data.
4. GuiProtocolDecoder only decodes raw commands into typed events.
5. GuiEventApplier only applies typed events to GameState.
6. GameState owns persistent game data.
7. Entity classes validate their own local state.
8. Renderer only reads GameState.
9. Renderer must not parse protocol commands.
10. Protocol code must not contain rendering logic.

---

## Current status

The GUI currently has a strong protocol/state foundation.

Implemented:

- CLI argument parsing
- TCP connection
- non-blocking receive buffer
- line extraction
- GRAPHIC authentication
- bootstrap protocol requests
- raw line parsing
- typed protocol event decoding
- typed event application to GameState
- player state synchronization
- egg state synchronization
- resource action event decoding
- broadcast and expulsion event decoding
- incantation event decoding
- server control event decoding
- read-only GameState access for rendering

Next major milestone:

Start GuiRenderer and draw a minimal 2D representation of GameState.

Initial renderer goal:

- open SFML window
- draw waiting screen before map is ready
- draw map grid after msz/mct
- draw players
- draw eggs
- draw basic tile resource indicators