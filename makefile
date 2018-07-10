CC=g++
CFLAGS=-I. --std=c++14

all: ApplesnGold

ApplesnGold: applesngold.o menu.o
	g++ -o ApplesnGold applesngold.o menu.o

applesngold.o: ApplesnGold.cpp ApplePickerUpgrade.h menu.h
	$(CC) -c -o applesngold.o $(CFLAGS) ApplesnGold.cpp

menu.o: menu.cpp menu.h
	$(CC) -c -o menu.o $(CFLAGS) menu.cpp

clean:
	rm -f ApplesnGold
	rm -f *.o
