CXX=g++
DEBUG=-Wall -DDEBUG -g -pedantic
OPTIMIZATIONS=-O2

ROOT=/home/ayrtoni/dev/research/asyncps

TESTS=$(ROOT)/tests
BIN=$(ROOT)/bin
SRC=$(ROOT)/src


all: asyncps_channel asyncps_client asyncps_server asyncps

asyncps:
	g++ $(DEBUG) -o $(BIN)/asyncps \
	                      poll_eventd.o    \
	                      networking.o     \
   						  logging.o        \
                          asyncps_channel.o \
	                      asyncps_client.o  \
	                      asyncps_server.o  \
	                      asyncps.cpp

asyncps_channel:
	g++ $(DEBUG) -c asyncps_channel.cpp

asyncps_client:
	g++ $(DEBUG) -c asyncps_client.cpp

asyncps_server:
	g++ $(DEBUG) -c asyncps_server.cpp

test_command:
	g++ $(DEBUG) -o $(TESTS)/testing test.cpp
