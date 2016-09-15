CXX=g++
DEBUG=-Wall -DDEBUG -g -pedantic
OPTIMIZATIONS=-O2

ROOT=/home/ayrton/Development/Hackaton/asyncps
TESTS=$(ROOT)/tests
BIN=$(ROOT)/bin
SRC=$(ROOT)/src


all: framework asyncps_channel asyncps_client asyncps_server asyncps testing

asyncps:
	g++ $(DEBUG) -o $(BIN)/asyncps \
	                      poll_eventd.o    \
	                      networking.o     \
   						  logging.o        \
                          asyncps_channel.o \
	                      asyncps_client.o  \
	                      asyncps_server.o  \
	                      asyncps.cpp
framework:
	g++ $(DEBUG) -c poll_eventd.cpp
	g++ $(DEBUG) -c networking.cpp
	g++ $(DEBUG) -c logging.cpp

asyncps_channel:
	g++ $(DEBUG) -c asyncps_channel.cpp

asyncps_client:
	g++ $(DEBUG) -c asyncps_client.cpp

asyncps_server:
	g++ $(DEBUG) -c asyncps_server.cpp

testing:
	g++ $(DEBUG) -o $(TESTS)/testing test.cpp
