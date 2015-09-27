# all:
# 	gcc -c main.cpp device.cpp
# 	gcc -o main main.o device.o

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -Wall -g -O3

# ****************************************************
# Targets needed to bring the executable up to date

main: main.o main.o Device.o TCPServer.o
	$(CXX) $(CXXFLAGS) -o main main.o Device.o TCPServer.o

# The main.o target can be written more simply
main.o: main.cpp Device.h TCPServer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Device.o: Device.h MovingAverage.h

# TCPServer.o: TCPServer.h Device.h

clean:
	rm -rf *.o main