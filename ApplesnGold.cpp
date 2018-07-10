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
#include "menu.h"

std::string name;
std::string filename;
int apples = 0;
float gold = 0;

// Prestige mechanics
int platinum = 0;
int platinumPrestige = 0;
float lifetimeGold = 0;

ApplePickerUpgrade applePickers("Apple Picker", 1.1, 10, 1);
ApplePickerUpgrade wizards("Wizard", 2, 10, 5);
ApplePickerUpgrade tractors("Tractor", 5, 15, 15);

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

    Menu menu;
    for (auto* picker : options) {
      menu.AddOption(picker->StoreLabel(), [picker]() {
          const float cost = picker->cost();
          if (gold > cost) {
            if (picker->upgrade()) {
              gold -= cost;
              prepareSaveData();
            }
          } else {
            std::cout << std::endl
                      << "Oops! It looks like you can't afford that!" << std::endl;
          }
          });
    }

    if (menu.Execute() == Menu::Result::kQuit) {
      return;
    }
    sleep(2);
  }
}

void game() {
  std::vector<ApplePickerUpgrade *> upgrades(
      {&applePickers, &wizards, &tractors});
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

    Menu menu;
    std::string pick_text;
    int pick_qty = 1;
    for (auto *picker : upgrades) {
      pick_qty += picker->pick();
    }
    if (pick_qty == 1) {
      pick_text = "Pick an apple";
    } else {
      pick_text = string_format("Pick %d apples", pick_qty);
    }
    menu.AddOption(std::move(pick_text), [pick_qty]() {
      apples += pick_qty;
      std::cout << "You picked " << pick_qty
                << (pick_qty > 1 ? " apples!" : " apple!") << std::endl;
    });

    float multiplier =
        (std::floorf(
             (int)(((double)(std::rand()) / RAND_MAX / 4 + .25) * 100)) /
         100) +
        (platinum / 100.0);

    std::string sell_text;
    if (platinum == 0) {
      sell_text = string_format("Sell %d apples for %0.02f gold", apples,
                                (multiplier * apples));
    } else {
      sell_text = string_format(
          "Sell %d apples for %0.02f gold, including platinum reward", apples,
          (multiplier * apples));
    }
    menu.AddOption(std::move(sell_text), [multiplier]() {
      std::cout << "You sold " << apples
                << ((apples > 1 || apples == 0) ? " apples for " : " apple for")
                << multiplier * apples << " gold!" << std::endl;
      gold += multiplier * apples;
      lifetimeGold += multiplier * apples;
      apples = 0;
    });

    menu.AddOption("Go to the shop", []() { shop(); });

    if (platinum > 0 || lifetimeGold >= 1000) {
      platinumPrestige = lifetimeGold / 100;
      menu.AddOption(
          string_format("Prestige for %d platinum", platinumPrestige),
          [&upgrades]() {
            if (prestige()) {
              std::cout << "Prestiging!" << std::endl;
              apples = 0;
              gold = 0;
              for (int i = 0; i < upgrades.size(); ++i) {
                upgrades[i]->load(0);
              }

              platinum += platinumPrestige;
              platinumPrestige = 0;
              lifetimeGold = 0;
            }
          });
    }

    if (menu.Execute() == Menu::Result::kQuit) {
      prepareSaveData();
      return;
    }
    sleep(1);
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
