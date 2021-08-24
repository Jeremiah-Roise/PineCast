g++ main.cpp -o main.sh -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp dbus-1 --libs --cflags` -g
