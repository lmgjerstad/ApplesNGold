#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <term.h>
#include <unistd.h>
#include <vector>

#include <sys/stat.h>

#include "ApplePickerUpgrade.h"

std::string name;
std::string filename;
int apples = 0;
float gold = 0;

// Prestige mechanics
int platinum = 0;
int platinumPrestige = 0;
float lifetimeGold = 0;

ApplePickerUpgrade applePickers("Apple Picker", 1, 10);
ApplePickerUpgrade wizards("Wizard", 2, 10);
ApplePickerUpgrade tractors("Tractor", 5, 15);

void prepareSaveData() {
  std::ofstream out;
  std::string temp = name;
  temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), [](int ch) {
               return !std::isspace(ch);
             }));
  filename = temp + ".ang";
  out.open(filename, std::ofstream::out | std::ofstream::trunc);
  out << "apples " << apples << "\n";
  out << "gold " << gold << "\n";
  out << "lifetimeGold " << lifetimeGold << "\n";
  out << "platinum " << platinum << "\n";
  out << "platinumGain " << platinumPrestige << "\n\n";
  out << "applePickers " << applePickers.level() << "\n";
  out << "wizards " << wizards.level() << "\n";
  out << "tractors " << tractors.level() << "\n";
  out.close();
}

bool prestige() {
  std::cout << "Are you sure you want to prestige for " << platinumPrestige
            << " platinum? (y/n)" << std::endl;
  std::string ans;
  std::cin >> ans;

  if (ans == "y") {
    return true;
  } else if (ans == "n") {
    return false;
  } else {
    std::cout << "Sorry! Didn't understand that!" << std::endl;
    sleep(2);
    return false;
  }
}

void shop() {
  std::vector<ApplePickerUpgrade *> options({&applePickers, &wizards, &tractors});
  while (true) {
    gold = std::floorf(gold * 100) / 100;
    lifetimeGold = std::floorf(lifetimeGold * 100) / 100;
    std::cout << "\033[2J\033[HSHOP" << std::endl;
    std::cout << "\033[1;93mGold: " << gold << "\033[0m" << std::endl
              << std::endl;

    for (int i = 0; i < options.size(); ++i) {
      std::cout << i + 1 << ": " << options[i]->StoreLabel() << std::endl;
    }

    std::cout << "Enter a number OR enter q/Q." << std::endl << std::endl;
    std::string ans;
    std::cin >> ans;

    if (ans == "q" || ans == "Q") {
      return;
    }

    int selection = strtol(ans.c_str(), nullptr, 10);
    if (selection <= 0 || selection > options.size()) {
      std::cout << "Sorry! Didn't understand that!" << std::endl;
      sleep(2);
      continue;
    }

    auto purchase = options[selection - 1];
    const float cost = purchase->cost();
    if (gold >= cost) {
      if (purchase->upgrade()) {
        gold -= cost;
        prepareSaveData();
        sleep(1);
        continue;
      }
    } else {
      std::cout << std::endl
                << "Oops! It looks like you can't afford that!" << std::endl;
      sleep(2);
      continue;
    }
  }
}

void game() {
  std::vector<ApplePickerUpgrade *> upgrades({&applePickers, &wizards, &tractors});
  while (true) {
    gold = std::floorf(gold * 100) / 100;
    lifetimeGold = std::floorf(lifetimeGold * 100) / 100;
    prepareSaveData();
    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;91mApples: " << apples << "\033[0m" << std::endl;
    std::cout << "\033[1;93mGold: " << gold << "\033[0m" << std::endl
              << std::endl;
    if (platinum > 0) {
      std::cout << "\033[1;36mPlatinum: " << platinum << "\033[0m" << std::endl
                << std::endl;
    }
    
    if((applePickers.pick() + wizards.pick()) == 0) {
      std::cout << "1: Pick an apple" << std::endl;
    } else {
      std::cout << "1: Pick " << (1 + applePickers.pick() + wizards.pick()) << " apples" << std::endl;
    }
    
    
    float multiplier =
      (std::floorf(
        (int)(((double)(std::rand()) / RAND_MAX / 4 + .25) * 100)) /
      100) + 
      (platinum / 100.0);

    if(platinum == 0) {
      std::cout << "2: Sell " << apples << " apples for " << (multiplier * apples) << " gold" << std::endl;
    } else {
      std::cout << "2: Sell " << apples << " apples for " << (multiplier * apples) << " gold, including platinum reward" << std::endl;
    }
    
    std::cout << "3: Go to she shop" << std::endl;

    if(platinum > 0 || lifetimeGold >= 1000) {
      platinumPrestige = lifetimeGold / 100;
      std::cout << "4: Prestige for " << platinumPrestige << " platinum" << std::endl;
    }

    std::cout << "Enter a number OR q/Q to quit." << std::endl << std::endl;

    std::string ans;

    std::cin >> ans;

    if(ans == "1") {
      apples += 1 + applePickers.pick() + wizards.pick();
      std::cout << "You picked " << applePickers.pick() + wizards.pick() + 1 << ((applePickers.pick() + wizards.pick() > 0) ? " apples!" : " apple!") << std::endl;
      sleep(1);
      continue;
    } else if(ans == "2") {
      std::cout << "You sold " << apples << ((apples > 1 || apples == 0) ? " apples for " : " apple for") << multiplier * apples << " gold!" << std::endl;
      gold += multiplier * apples;
      lifetimeGold += multiplier * apples;
      apples = 0;
      sleep(2);
      continue;
    } else if(ans == "3") {
      shop();
      continue;
    } else if(ans == "4") {
      if(platinum > 0 || lifetimeGold >= 1000) {
        if(prestige()) {
          std::cout << "Prestiging!" << std::endl;
          sleep(3);
          apples = 0;
          gold = 0;
          for(int i = 0; i < upgrades.size(); ++i) {
            upgrades[i]->load(0);
          }

          platinum += platinumPrestige;
          platinumPrestige = 0;
          lifetimeGold = 0;
          continue;
        } else {
          continue;
        }
      } else {
        std::cout << "Sorry! Didn't understand that!" << std::endl;
        sleep(2);
        continue;
      }
    } else if(ans == "q" || ans == "Q") {
      prepareSaveData();
      break;
    } else {
      std::cout << "Sorry! Didn't understand that!" << std::endl;
      sleep(2);
      continue;
    }
  }
}

int main(int argc, char **argv) {
  std::srand(std::time(nullptr));
  std::cout << "\033[2J\033[H";
  std::cout << "Hello, there! What is your name?" << std::endl;
  std::cin >> name;
  std::cout << "Hello, " << name << "!" << std::endl;

  std::ifstream in;
  std::string temp = name;
  std::string::iterator buf = std::remove(temp.begin(), temp.end(), ' ');
  temp.erase(buf, temp.end());
  filename = temp + ".ang";
  in.open(filename);

  std::string stat;
  float value;

  while (in >> stat >> value) {
    try {
      if (stat == "apples") {
        apples = value;
      } else if (stat == "gold") {
        gold = value;
      } else if (stat == "lifetimeGold") {
        lifetimeGold = value;
      } else if (stat == "platinum") {
        platinum = value;
      } else if (stat == "platinumGain") {
        platinumPrestige = value;
      } else if (stat == "applePickers") {
        applePickers.load(value);
      } else if (stat == "wizards") {
        wizards.load(value);
      } else if(stat == "tractors") {
        tractors.load(value);
      } else {
        throw 1; // Activate catch statement
      }
    } catch (...) {
      std::cout << "The save file \"" << filename << "\" is corrupted." << std::endl;
      sleep(2);
      std::cout << "\033[2J\033[H";
      return 1;
    }
  }

  in.close();

  std::cout << "Let's play!" << std::endl;
  sleep(2);
  game();
  std::cout << "Ok. Bye " << name << "!" << std::endl;
  sleep(2); // Don't end immediately.
  std::cout << "\033[2J\033[H";
  return 0;
}
