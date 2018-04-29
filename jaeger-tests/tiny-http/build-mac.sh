#!/bin/bash

g++ -std=c++11 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lssl -lz -lcrypto -luv ./uWS/Epoll.cpp ./uWS/Extensions.cpp ./uWS/Group.cpp ./uWS/HTTPSocket.cpp ./uWS/Hub.cpp ./uWS/Networking.cpp ./uWS/Node.cpp ./uWS/Room.cpp ./uWS/Socket.cpp ./uWS/WebSocket.cpp test.cpp