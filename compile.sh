g++ main.cpp -o main.sh -I/RSSParser.h -lcurl -Wall -rdynamic `pkg-config gtk+-3.0 curlpp --libs --cflags` -g
