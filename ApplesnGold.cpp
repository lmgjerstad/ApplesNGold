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
int roundNum = 0;

// Prestige mechanics
int platinum = 0;
int platinumPrestige = 0;
float lifetimeGold = 0;

ApplePickerUpgrade applePickers("Apple Picker", 1, 10);
ApplePickerUpgrade wizards("Wizard", 2, 10);

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
  out << "wizards " << wizards.level();
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
  std::vector<ApplePickerUpgrade *> options({&applePickers, &wizards});
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
      std::cout << "Sorry! Didn't understand that." << std::endl;
      continue;
    }

    auto purchase = options[selection - 1];
    const float cost = purchase->cost();
    if (gold >= cost) {
      if (purchase->upgrade()) {
        gold -= cost;
        prepareSaveData();
      }
    } else {
      std::cout << std::endl
                << "Oops! It looks like you can't afford that!" << std::endl;
    }
    sleep(2);
  }
}

void round(bool restarted) {
  std::vector<ApplePickerUpgrade *> upgrades({&applePickers, &wizards});
  ++roundNum;
  while (true) {
    gold = std::floorf(gold * 100) / 100;
    lifetimeGold = std::floorf(lifetimeGold * 100) / 100;
    prepareSaveData();
    std::cout << "\033[2J\033[H";
    std::cout << "ROUND " << roundNum << std::endl;
    std::cout << "\033[1;91mApples: " << apples << "\033[0m" << std::endl;
    std::cout << "\033[1;93mGold: " << gold << "\033[0m" << std::endl
              << std::endl;
    if (platinum > 0) {
      std::cout << "\033[1;36mPlatinum: " << platinum << "\033[0m" << std::endl
                << std::endl;
    }
    std::cout << "Would you like to pick an apple? (y/n)" << std::endl;
    std::string ans;
    std::cin >> ans;

    if (ans == "y") {
      int pick = 1;
      for (auto *upgrade : upgrades) {
        pick += upgrade->pick();
      }
      apples += pick;
      if (pick == 1) {
        std::cout << "You picked an apple!" << std::endl;
      } else {
        std::cout << "You picked " << pick << " apples!" << std::endl;
      }
      sleep(1);
      ++roundNum;
      continue;
    } else if (ans == "n") {

    } else if (ans == "s") {
      shop();
      continue;
    } else {
      std::cout << "Sorry! Didn't understand that." << std::endl;
      sleep(2);
      continue;
    }

    float multiplier =
        (std::floorf(
             (int)(((double)(std::rand()) / RAND_MAX / 4 + .25) * 100)) /
         100) +
        (platinum / 100.0);
    if (platinum == 0) {
      std::cout << "Do you want to sell your apples for " << multiplier
                << " each? (y/n)" << std::endl;
    } else {
      std::cout << "Do you want to sell your apples for "
                << multiplier - (platinum / 100.0) << " + " << platinum / 100.0
                << " for platinum each? (y/n)" << std::endl;
    }
    std::cin >> ans;

    if (ans == "y") {
      float prevGold = gold;
      gold += apples * multiplier;
      lifetimeGold += apples * multiplier;
      std::cout << "Sold " << apples
                << ((apples != 1) ? " apples for " : " apple for ")
                << gold - prevGold << " gold." << std::endl;
      apples = 0;
      sleep(2);
      ++roundNum;
      continue;
    } else if (ans == "n") {

    } else if (ans == "s") {
      shop();
      continue;
    } else {
      std::cout << "Sorry! Didn't understand that." << std::endl;
      sleep(2);
      continue;
    }

    if (lifetimeGold >= 1500 || platinum > 0) {
      std::cout << "Would you like to prestige for " << platinumPrestige
                << " platinum? (y/n)" << std::endl;
      std::cin >> ans;

      if (ans == "y") {
        if (prestige()) {
          apples = 0;
          gold = 0;
          lifetimeGold = 0;
          roundNum = 0;
          platinum += platinumPrestige;
          platinumPrestige = 0;
          applePickers.load(0);
          wizards.load(0);
          std::cout << "Prestiging!" << std::endl;
          sleep(3);
          continue;
        }
      } else if (ans == "n") {

      } else {
        std::cout << "Sorry! Didn't understand that!" << std::endl;
        sleep(2);
        continue;
      }
    }

    std::cout << "Do you want to quit? (y/n)" << std::endl;
    std::cin >> ans;

    if (ans == "y") {
      return;
    } else if (ans == "s") {
      shop();
    }
  }
}

int main(int argc, char **argv) {
  system("clear");
  std::srand(std::time(nullptr));
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
      } else {
        throw 1; // Activate catch statement
      }
    } catch (...) {
      std::cout << "The save file \"" << filename << "\" is corrupted.";
      sleep(2);
      return 1;
    }
  }

  in.close();

  std::cout << "Let's play!" << std::endl;
  sleep(2);
  round(false);
  std::cout << "Ok. Bye " << name << "!" << std::endl;
  sleep(2); // Don't end immediately.
  system("clear");
  return 0;
}
