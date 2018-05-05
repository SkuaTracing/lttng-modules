#!/bin/bash

$@ &

PROCESS_PID=$!

lttng track --pid $PROCESS_PID

trap "kill $PROCESS_PID" sigint
wait $PROCESS_PID

lttng untrack --pid $PROCESS_PID

