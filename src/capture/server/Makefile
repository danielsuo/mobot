# all:
# 	gcc -c main.cpp device.cpp
# 	gcc -o main main.o device.o

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -Wall -g

# ****************************************************
# Targets needed to bring the executable up to date

main: main.o main.o Device.o
	$(CXX) $(CXXFLAGS) -o main main.o Device.o

# The main.o target can be written more simply
main.o: main.cpp Device.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Device.o: Device.h MovingAverage.h