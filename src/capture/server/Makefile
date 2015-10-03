# all:
# 	gcc -c main.cpp device.cpp
# 	gcc -o main main.o device.o

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -Wall -g -O3

# ****************************************************
# Targets needed to bring the executable up to date

debug: CXXFLAGS += -DDEBUG -g -fno-pie
debug: main

rewrite: CXXFLAGS += -DREWRITE
rewrite: debug

all: main

main: main.o Device.o TCPServer.o StackTrace.o File.o file_processors.o file_writers.o utilities.o
	$(CXX) $(CXXFLAGS) -o main main.o Device.o TCPServer.o StackTrace.o File.o file_processors.o file_writers.o utilities.o

# The main.o target can be written more simply
main.o: main.cpp Device.h TCPServer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Device.o: Device.h MovingAverage.h	

TCPServer.o: TCPServer.h Device.o handler_client.h

File.o: File.h utilities.h

clean:
	rm -rf *.o main

xlean:
	rm -rf image_data iPhone blue pink gold silver
