#include <iostream>
#include <string>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

std::string name;
int apples = 0;
float gold = 0;
int roundNum = 0;

void round(bool restarted) {
	system("clear");
	if(!restarted) {
		++roundNum;
	} else {
		std::cout << "RESTARTED ";
	}
	std::cout << "ROUND " << roundNum << std::endl;
	std::cout << "Apples: " << apples << std::endl;
	std::cout << "Gold: " << gold << std::endl << std::endl;
	std::cout << "Would you like to pick an apple? (y/n)" << std::endl;
	std::string ans;
	std::cin >> ans;
	
	if(ans == "y") {
		++apples;
		std::cout << "You picked an apple!" << std::endl;
		sleep(2);
		round(false);
		return;
	} else if(ans == "n") {
		
	} else {
		std::cout << "Sorry! Didn't understand that." << std::endl;
		sleep(2);
		round(true);
		return;
	}
	
	float multiplier = std::floorf((int)(((double)(std::rand()) / RAND_MAX / 2 + .5) * 100)) / 100;
	std::cout << "Do you want to sell your apples for " << multiplier << " each? (y/n)" << std::endl;
	std::cin >> ans;
	
	if(ans == "y") {
		float prevGold = gold;
		gold += apples * multiplier;
		std::cout << "Sold " << apples << " apples for " << gold - prevGold << " gold." << std::endl;
		apples = 0;
		sleep(2);
		round(false);
		return;
	} else if(ans == "n") {
		
	} else {
		std::cout << "Sorry! Didn't understand that." << std::endl;
		sleep(2);
		round(true);
		return;
	}
	
	std::cout << "Do you want to quit? (y/n)" << std::endl;
	std::cin >> ans;
	
	if(ans == "y") {
		return;
	} else {
		round(true);
		return;
	}
	
	return;
	
}

int main(int argc, char** argv) {
	std::srand(std::time(0));
	std::cout << "Hello, there! What is your name?" << std::endl;
	std::cin >> name;
	std::cout << "Hello, " << name << "!" << std::endl;
	std::cout << "Let's play!" << std::endl;
	sleep(2);
	round(false);
	std::cout << "Ok. Bye " << name << "!";
	return 0;
}