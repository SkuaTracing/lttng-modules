#!/bin/bash

g++ -O3 -march=native -flto -std=c++11 -lssl -lz -lcrypto -luv -luWS -DNO_JAEGER test.cpp
