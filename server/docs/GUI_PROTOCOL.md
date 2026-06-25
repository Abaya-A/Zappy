# GUI TCP Protocol

## Overview

The GUI protocol is used by visualization clients.

Unlike AI clients, GUI clients do not control a player.

The server continuously pushes game events to GUI connections.

A connection authenticated as a GUI client **cannot use AI commands**.

---

# Connection Handshake

## Server → Client

```text
WELCOME
```

## Client → Server

```text
GRAPHIC
```

This reserved keyword authenticates the connection as a GUI client.

---

# Protocol Model

The protocol is event-driven.

Most messages are emitted automatically by the server.

The GUI only sends a few state query commands.

---

# GUI Requests

## msz

Request map size.

### Request

```text
msz
```

### Response

```text
msz X Y
```

Example:

```text
msz 20 20
```

---

## bct

Request tile content.

### Request

```text
bct X Y
```

### Response

```text
bct X Y q0 q1 q2 q3 q4 q5 q6
```

Where:

| Resource  | Index |
| --------- | ----- |
| food      | q0    |
| linemate  | q1    |
| deraumere | q2    |
| sibur     | q3    |
| mendiane  | q4    |
| phiras    | q5    |
| thystame  | q6    |

Example:

```text
bct 4 8 2 1 0 0 0 0 0
```

---

## mct

Request complete map content.

### Request

```text
mct
```

### Response

```text
bct ...
bct ...
bct ...
```

One `bct` line per tile.

---

## tna

Request all team names.

### Request

```text
tna
```

### Response

```text
tna TeamA
tna TeamB
tna TeamC
```

---

## ppo

Request player position.

### Request

```text
ppo #n
```

### Response

```text
ppo #n X Y O
```

Where:

```text
1 = North
2 = East
3 = South
4 = West
```

---

## plv

Request player level.

### Request

```text
plv #n
```

### Response

```text
plv #n L
```

---

## pin

Request player inventory.

### Request

```text
pin #n
```

### Response

```text
pin #n X Y q0 q1 q2 q3 q4 q5 q6
```

---

## sgt

Request time unit.

### Request

```text
sgt
```

### Response

```text
sgt T
```

---

## sst

Modify time unit.

### Request

```text
sst T
```

### Response

```text
sst T
```

---

# Server Events

These messages are pushed automatically.

---

## New Player

```text
pnw #n X Y O L N
```

---

## Player Movement

```text
ppo #n X Y O
```

---

## Level Change

```text
plv #n L
```

---

## Inventory Update

```text
pin #n X Y q0 q1 q2 q3 q4 q5 q6
```

---

## Player Expulsion

```text
pex #n
```

---

## Broadcast

```text
pbc #n MESSAGE
```

---

## Incantation Started

```text
pic X Y L #n #n ...
```

---

## Incantation Ended

```text
pie X Y R
```

Where:

```text
R = 1 success
R = 0 failure
```

---

## Egg Laying

```text
pfk #n
```

---

## Resource Dropped

```text
pdr #n i
```

---

## Resource Collected

```text
pgt #n i
```

---

## Player Death

```text
pdi #n
```

---

## Egg Created

```text
enw #e #n X Y
```

---

## Egg Hatched

```text
ebo #e
```

---

## Egg Destroyed

```text
edi #e
```

---

## End of Game

```text
seg TEAM_NAME
```

---

## Server Message

```text
smg MESSAGE
```

---

# Error Responses

## Unknown Command

```text
suc
```

## Invalid Parameters

```text
sbp
```

---

# Connection Lifecycle

```text
TCP Connect
    ↓
WELCOME
    ↓
GRAPHIC
    ↓
Initial State Queries
(msz, mct, tna...)
    ↓
Live Event Stream
(pnw, ppo, pgt, pie...)
    ↓
disconnect
```
