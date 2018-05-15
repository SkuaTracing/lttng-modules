#!/bin/bash

#pushd /root/jaeger-client-cpp-0.3.0-mod/build
#make install
#popd

g++ -O3 -march=native -flto -std=c++11 -lopentracing -ljaegertracing -lpthread /root/jaeger-client-cpp-0.3.0-mod/build/libjaegertracing.a main.cpp
