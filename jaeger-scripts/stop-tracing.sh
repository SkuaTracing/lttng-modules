#!/bin/bash
set -x
lttng stop
sleep 10
lttng destroy
sleep 5
killall babeltrace


