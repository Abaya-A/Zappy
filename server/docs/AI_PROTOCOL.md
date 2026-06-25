# AI TCP Protocol

## Overview

The AI protocol is used by autonomous game clients controlling a player on the server.

Each AI client connects through TCP and communicates using a command/response model.

A connection authenticated as an AI client **cannot use GUI commands**.

---

# Connection Handshake

## Server → Client

```text
WELCOME
```

## Client → Server

```text
TEAM_NAME
```

Example:

```text
BlueTeam
```

## Server → Client

```text
CLIENT_NUM
```

Number of remaining available slots for the team.

Example:

```text
3
```

## Server → Client

```text
X Y
```

Map dimensions.

Example:

```text
20 20
```

---

# Protocol Rules

* All messages are UTF-8 text lines terminated by `\n`.
* Commands are executed in the order received.
* A client may queue up to **10 pending commands**.
* Commands beyond this limit may be ignored.
* Unknown commands return:

```text
ko
```

* If the player dies, the server sends:

```text
dead
```

and closes the session.

---

# Available Commands

## Forward

Move one tile forward.

### Request

```text
Forward
```

### Response

```text
ok
```

---

## Right

Rotate 90° clockwise.

### Request

```text
Right
```

### Response

```text
ok
```

---

## Left

Rotate 90° counter-clockwise.

### Request

```text
Left
```

### Response

```text
ok
```

---

## Look

Retrieve visible tiles around the player.

### Request

```text
Look
```

### Response

```text
[player, food, linemate, player, ...]
```

Example:

```text
[player, food,, thystame]
```

---

## Inventory

Retrieve player inventory.

### Request

```text
Inventory
```

### Response

```text
[food 10, linemate 1, deraumere 0, sibur 2, mendiane 0, phiras 0, thystame 0]
```

---

## Broadcast

Send a message to every connected player.

### Request

```text
Broadcast hello world
```

### Response

```text
ok
```

### Received by other players

```text
message K, hello world
```

Where:

* `K = 0` if emitted from the same tile
* otherwise indicates sound direction

---

## Connect_nbr

Retrieve available connection slots.

### Request

```text
Connect_nbr
```

### Response

```text
3
```

---

## Fork

Create an egg and reserve a future player slot.

### Request

```text
Fork
```

### Response

```text
ok
```

---

## Eject

Push all players located on the same tile.

### Request

```text
Eject
```

### Response

```text
ok
```

or

```text
ko
```

### Notification received by pushed players

```text
eject: K
```

---

## Take

Pick up a resource from the current tile.

### Request

```text
Take linemate
```

### Response

```text
ok
```

or

```text
ko
```

### Available resources

```text
food
linemate
deraumere
sibur
mendiane
phiras
thystame
```

---

## Set

Drop a resource on the current tile.

### Request

```text
Set linemate
```

### Response

```text
ok
```

or

```text
ko
```

---

## Incantation

Start an elevation ritual.

### Request

```text
Incantation
```

### Successful Start

```text
Elevation underway
```

### Successful Completion

```text
Current level: K
```

### Failure

```text
ko
```

---

# Asynchronous Messages

These messages can be received at any moment.

## Broadcast Reception

```text
message K, text
```

## Ejection Reception

```text
eject: K
```

## Death

```text
dead
```

---

# Error Handling

## Unknown Command

```text
ko
```

## Invalid Parameters

```text
ko
```

---

# Connection Lifecycle

```text
TCP Connect
    ↓
WELCOME
    ↓
TEAM_NAME
    ↓
CLIENT_NUM
MAP_SIZE
    ↓
Game Commands
    ↓
dead / disconnect
```
