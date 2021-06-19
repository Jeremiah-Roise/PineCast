g++ main.cpp -o main.sh -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp --libs --cflags` -g
