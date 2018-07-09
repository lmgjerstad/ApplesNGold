all: ApplesnGold

AplesnGold: ApplesnGold.cpp
	g++ -o ApplesnGold --std=c++14 ApplesnGold.cpp

clean:
	rm -f ApplesnGold
