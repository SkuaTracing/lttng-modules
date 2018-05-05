#!/bin/bash
set -x
cd ~/golang/src/github.com/hsheth2/lttng-adapter
go build main.go
babeltrace --input-format=lttng-live net://localhost/host/voxel/my-kernel-session | ./main

