#!/bin/bash
if [ "$1" = "1" ]
then
	echo "optimized compile"
	g++ main.cpp -o main.sh -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp --libs --cflags` -fuse-ld=mold -Ofast
else
	echo "debugging compile"
	g++ main.cpp -o main.sh -lcurl -lpthread -lm -ldl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp --libs --cflags` -fuse-ld=mold -g
fi
