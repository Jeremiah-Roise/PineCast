#!/bin/bash
if [ "$1" = "1" ]
then
	echo "optimized compile"
	g++ main.cpp -o main.sh -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp dbus-1 --libs --cflags` -Ofast
else
	echo "debugging compile"
	g++ main.cpp -o main.sh -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp dbus-1 --libs --cflags` -g
fi
