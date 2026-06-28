#!/bin/bash

PORT=${1:-4242}
TEAM=${2:-team1}
HOST=${3:-localhost}
COUNT=${4:-6}

for i in $(seq 1 $COUNT); do
    echo "Starting AI $i..."
    ./zappy_ai -p "$PORT" -n "$TEAM" -h "$HOST" &
    sleep 0.2
done

wait