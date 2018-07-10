all: ApplesnGold

ApplesnGold: ApplesnGold.cpp
	g++ -o ApplesnGold --std=c++14 -lcurses ApplesnGold.cpp

clean:
	rm -f ApplesnGold
