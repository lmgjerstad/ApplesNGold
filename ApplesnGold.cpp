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
#include "MagicPotion.h"
#include "menu.h"
#include "absl/strings/str_format.h"

class ApplesNGold {
public:
  ApplesNGold()
      : apples_(0), gold_(0), platinum_(0), platinum_prestige_(0),
        lifetime_gold_(0) {
    pickers_.emplace_back("Apple Picker", 1.7, 10, 1);
    pickers_.emplace_back("Wizard", 2, 10, 5);
    pickers_.emplace_back("Tractor", 5, 15, 15);

    potions_.emplace_back("Red Potion", MagicPotion::Type::kApples, 10, 50, 400, "\033[1;91m");
    potions_.emplace_back("Yellow Potion", MagicPotion::Type::kGold, 15, 50, 400, "\033[1;93m");
    potions_.emplace_back("Blue Potion", MagicPotion::Type::kPlatinum, 1, 500, 15000, "\033[1;96m");
  }

  void Run();
private:
  void Save();

  void Load();

  const std::string &SaveFile();

  bool Prestige();

  void Shop();

  std::string name_;
  std::string filename_;
  int apples_;
  float gold_;
  int platinum_;
  int platinum_prestige_;
  float lifetime_gold_;
  std::vector<ApplePickerUpgrade> pickers_;
  std::vector<MagicPotion> potions_;
};

const std::string &ApplesNGold::SaveFile() {
  if (filename_.empty()) {
    std::string temp = name_;
    temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), [](int ch) {
                 return !std::isspace(ch);
               }));
    filename_ = temp + ".ang";
  }
  return filename_;
}

void ApplesNGold::Save() {
  std::ofstream out;
  out.open(SaveFile(), std::ofstream::out | std::ofstream::trunc);
  out << "apples " << apples_ << "\n";
  out << "gold " << gold_ << "\n";
  out << "lifetimeGold " << lifetime_gold_ << "\n";
  out << "platinum " << platinum_ << "\n";
  out << "platinumGain " << platinum_prestige_ << "\n\n";
  out << "applePickers " << pickers_[0].level() << "\n";
  out << "wizards " << pickers_[1].level() << "\n";
  out << "tractors " << pickers_[2].level() << "\n";
  out.close();
}

void ApplesNGold::Load() {
  std::ifstream in;
  in.open(SaveFile());

  std::string stat;
  float value;

  while (in >> stat >> value) {
    try {
      if (stat == "apples") {
        apples_ = value;
      } else if (stat == "gold") {
        gold_ = value;
      } else if (stat == "lifetimeGold") {
        lifetime_gold_ = value;
      } else if (stat == "platinum") {
        platinum_ = value;
      } else if (stat == "platinumGain") {
        platinum_prestige_ = value;
      } else if (stat == "applePickers") {
        pickers_[0].load(value);
      } else if (stat == "wizards") {
        pickers_[1].load(value);
      } else if (stat == "tractors") {
        pickers_[2].load(value);
      } else {
        throw 1; // Activate catch statement
      }
    } catch (...) {
      std::cout << "The save file \"" << filename_ << "\" is corrupted."
                << std::endl;
      sleep(2);
      std::cout << "\033[2J\033[H";
      exit(1);
    }
  }

  in.close();
}

bool ApplesNGold::Prestige() {
  std::cout << "Are you sure you want to prestige for " << platinum_prestige_
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

void ApplesNGold::Shop() {
  while (true) {
    gold_ = std::floorf(gold_ * 100) / 100;
    lifetime_gold_ = std::floorf(lifetime_gold_ * 100) / 100;
    std::cout << "\033[2J\033[HSHOP" << std::endl;
    std::cout << "\033[1;93mGold: " << gold_ << "\033[0m" << std::endl
              << std::endl;

    Menu menu;
    for (auto &picker : pickers_) {
      menu.AddOption(picker.StoreLabel(), [this, &picker]() {
        const float cost = picker.cost();
        if (gold_ >= cost) {
          if (picker.upgrade()) {
            gold_ -= cost;
            Save();
            sleep(1);
          }
        } else {
          std::cout << "Oops! It looks like you can't afford that!"
                    << std::endl;
          sleep(2);
        }
      });
    }
    if(platinum_ > 0) {
      menu.BlankLine();
  
      for (auto &potion : potions_) {
        menu.AddOption(potion.StoreLabel(), [this, &potion]() {
          const float cost = potion.cost();
          if (gold_ >= cost && potion.type() != MagicPotion::Type::kGold) {
            potion.setRoundNum(1);
            std::cout << "You bought a(n) " << potion.name() << "!" << std::endl;
            gold_ -= cost;
            Save();
            sleep(1);
          } else if(potion.type() == MagicPotion::Type::kGold && apples_ >= cost) {
            potion.setRoundNum(1);
            std::cout << "You bought a(n) " << potion.name() << "!" << std::endl;
            apples_ -= cost;
            Save();
            sleep(1);
          } else {
            std::cout << "Oops! It looks like you can't afford that!" << std::endl;
            sleep(2);
          }
        });
      }
    }

    if (menu.Execute() == Menu::Result::kQuit) {
      break;
    }
  }
}

void ApplesNGold::Run() {
  std::srand(std::time(nullptr));
  std::cout << "\033[2J\033[H";
  std::cout << "Hello, there! What is your name?" << std::endl;
  std::cin >> name_;
  std::cout << "Hello, " << name_ << "!" << std::endl;

  Load();
  
  std::cout << "Let's play!" << std::endl;
  sleep(2);

  while (true) {
    gold_ = std::floorf(gold_ * 100) / 100;
    lifetime_gold_ = std::floorf(lifetime_gold_ * 100) / 100;
    Save();
    
    for(auto &potion : potions_) {
      if(potion.active()) {
        if(!potion.loop()) {
          if(potion.type() == MagicPotion::Type::kApples) {
            apples_ += potion.multiplier();
          } else if(potion.type() == MagicPotion::Type::kGold) {
            gold_ += potion.multiplier();
          } else {
            platinum_ += potion.multiplier();
          }
        }
      } else {
        potion.setRoundNum(0);
      }
    }

    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;91mApples: " << apples_ << "\033[0m" << std::endl;
    std::cout << "\033[1;93mGold: " << gold_ << "\033[0m" << std::endl
              << std::endl;
    if (platinum_ > 0) {
      std::cout << "\033[1;96mPlatinum: " << platinum_ << "\033[0m" << std::endl
                << std::endl;
    }

    Menu menu;
    std::string pick_text;
    int pick_qty = 1;
    for (auto &picker : pickers_) {
      pick_qty += picker.pick();
    }
    if (pick_qty == 1) {
      pick_text = "Pick an apple";
    } else {
      pick_text = absl::StrFormat("Pick %d apples", pick_qty);
    }
    menu.AddOption(std::move(pick_text), [this, pick_qty]() {
      apples_ += pick_qty;
      std::cout << "You picked " << pick_qty
                << (pick_qty > 1 ? " apples!" : " apple!") << std::endl;
    });

    float multiplier =
        (std::floorf(
             (int)(((double)(std::rand()) / RAND_MAX / 4 + .25) * 100)) /
         100) +
        (platinum_ / 100.0);

    std::string sell_text;
    if (platinum_ == 0) {
      sell_text = absl::StrFormat("Sell %d apples for %0.02f gold each", apples_,
                                (multiplier));
    } else {
      sell_text = absl::StrFormat(
          "Sell %d apples for %0.02f + %0.02f gold each", apples_,
          (multiplier - (platinum_ / 100.0)), platinum_ / 100.0);
    }
    menu.AddOption(std::move(sell_text), [this, multiplier]() {
      std::cout << "You sold " << apples_
                << ((apples_ > 1 || apples_ == 0) ? " apples for "
                                                  : " apple for")
                << multiplier * apples_ << " gold!" << std::endl;
      gold_ += multiplier * apples_;
      lifetime_gold_ += multiplier * apples_;
      apples_ = 0;
    });

    menu.AddOption("Go to the shop", [this]() { Shop(); });

    if (platinum_ > 0 || lifetime_gold_ >= 1000) {
      platinum_prestige_ = lifetime_gold_ / 100;
      menu.AddOption(
          absl::StrFormat("Prestige for %d platinum", platinum_prestige_),
          [this]() {
            if (Prestige()) {
              std::cout << "Prestiging!" << std::endl;
              apples_ = 0;
              gold_ = 0;
              for (auto &picker : pickers_) {
                picker.load(0);
              }

              platinum_ += platinum_prestige_;
              platinum_prestige_ = 0;
              lifetime_gold_ = 0;
            }
          });
    }

    if (menu.Execute() == Menu::Result::kQuit) {
      Save();
      break;
    }
    sleep(1);
  }

  std::cout << "Ok. Bye " << name_ << "!" << std::endl;
  sleep(2); // Don't end immediately.
  std::cout << "\033[2J\033[H";
}

int main(int argc, char **argv) {
  ApplesNGold game;
  game.Run();
  return 0;
}
