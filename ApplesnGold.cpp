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
#include "IdlePicker.h"
#include "menu.h"
#include "absl/strings/str_format.h"
#include "ApplesNGold/apples_n_gold.pb.h"

class ApplesNGold {
public:
  ApplesNGold()
      : apples_(0), gold_(0), platinum_(0), platinum_prestige_(0),
        lifetime_gold_(0) {
    pickers_.emplace_back("Apple Picker", 1.7, 10, 1);
    pickers_.emplace_back("Wizard", 2, 10, 5);
    pickers_.emplace_back("Tractor", 5, 15, 15);
    pickers_.emplace_back("Self Picker", 10, 30, 25);
    idle_.emplace_back("Plucker", 0.25, 15);

    potions_.emplace_back("Red Potion", MagicPotion::Type::kApples, 10, 50, 400,
                          "\033[1;91m");
    potions_.emplace_back("Yellow Potion", MagicPotion::Type::kGold, 15, 50,
                          400, "\033[1;93m");
    potions_.emplace_back("Blue Potion", MagicPotion::Type::kPlatinum, 1, 500,
                          15000, "\033[1;96m");
  }

  void Run();

private:
  void Save();

  void LoadIdleGain(int time_offline);

  void Load();
  void LegacyLoad();

  template <typename T, typename P>
  void LoadVector(std::vector<T> *vec,
                  const google::protobuf::RepeatedPtrField<P> &protos);

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
  int last_login_;
  std::vector<ApplePickerUpgrade> pickers_;
  std::vector<MagicPotion> potions_;
  std::vector<IdlePicker> idle_;

  const char *ROOT = getenv("HOME");
};

const std::string &ApplesNGold::SaveFile() {
  if (filename_.empty()) {

    std::string temp = name_;
    temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), [](int ch) {
                 return !std::isspace(ch);
               }));
    
    filename_ = absl::StrFormat("%s/ApplesnGold/%s.ang", ROOT, temp.c_str());

    std::ifstream file(filename_.c_str());
    if(!file.good()) {
      system("mkdir -p ~/ApplesnGold");
    }
  }
  return filename_;
}

void ApplesNGold::Save() {
  applesngold::Save proto;
  proto.set_apples(apples_);
  proto.set_gold(gold_);
  proto.set_lifetime_gold(lifetime_gold_);
  proto.set_platinum(platinum_);
  proto.set_platinum_gain(platinum_prestige_);
  proto.set_last_login(std::time(nullptr));

  for (const auto& picker : pickers_) {
    *proto.add_picker() = picker.Save();
  }

  for (const auto& potion : potions_) {
    *proto.add_potion() = potion.Save();
  }

  for (const auto& idle : idle_) {
    *proto.add_idle() = idle.Save();
  }

  std::ofstream out(SaveFile(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
  proto.SerializeToOstream(&out);
  out.close();
}

void ApplesNGold::LoadIdleGain(int time_offline) {
  for(auto &idle : idle_) {
    apples_ += idle.amount() * idle.multiplier() * time_offline;
    std::cout << "added " << idle.amount() << " * " << idle.multiplier() << " * " << time_offline << std::endl;
  }
}

template <typename T, typename P>
void ApplesNGold::LoadVector(std::vector<T>* vec, const google::protobuf::RepeatedPtrField<P> &protos) {
  std::map<std::string, T *> map;
  for (auto &entry : *vec) {
    map[entry.name()] = &entry;
  }
  for (const auto &entry : protos) {
    auto iter = map.find(entry.name());
    if (iter == map.end()) {
      std::cout << "The save file at " << filename_ << " is corrupted."
                << std::endl;
      sleep(2);
      std::cout << "\033[2J\033[H";
      exit(1);
    }
    iter->second->Load(entry);
  }
}

void ApplesNGold::Load() {
  last_login_ = std::time(nullptr);

  std::ifstream in(SaveFile());
  applesngold::Save proto;
  if (!proto.ParseFromIstream(&in)) {
    LegacyLoad();
    return;
  }
  apples_ = proto.apples();
  gold_ = proto.gold();
  lifetime_gold_ = proto.lifetime_gold();
  platinum_ = proto.platinum();
  platinum_prestige_ = proto.platinum_gain();

  LoadVector(&pickers_, proto.picker());
  LoadVector(&potions_, proto.potion());
  LoadVector(&idle_, proto.idle());

  std::cout << proto.DebugString() << std::endl;
  LoadIdleGain(last_login_ - proto.last_login());
}

void ApplesNGold::LegacyLoad() {
  last_login_ = std::time(nullptr);
  
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
        pickers_[0].Load(value);
      } else if (stat == "wizards") {
        pickers_[1].Load(value);
      } else if (stat == "tractors") {
        pickers_[2].Load(value);
      } else if (stat == "selfPickers") {
        pickers_[3].Load(value);
      } else if (stat == "pluckers") {
        idle_[0].Load(value);
      } else if (stat == "lastLogin") {
        LoadIdleGain((int)(last_login_ - value));
      } else {
        throw 1; // Activate catch statement
      }
    } catch (...) {
      std::cout << "The save file at " << filename_ << " is corrupted."
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
    
    last_login_ = std::time(nullptr);

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
    
    menu.BlankLine();

    for(auto &idle : idle_) {
      menu.AddOption(idle.StoreLabel(), [this, &idle]() {
        const float cost = idle.cost();
        if(gold_ >= cost) {
          idle.Load(idle.amount() + 1);
          std::cout << "You bought a(n) " << idle.name() << "!" << std::endl;
          gold_ -= cost;
          Save();
          sleep(1);
        } else {
          std::cout << "Oops! It looks like you can't afford that!" << std::endl;
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
            potion.Load(potion.amount() + 1);
            std::cout << "You bought a(n) " << potion.name() << "!" << std::endl;
            gold_ -= cost;
            Save();
            sleep(1);
          } else if(potion.type() == MagicPotion::Type::kGold && apples_ >= cost) {
            potion.setRoundNum(1);
            potion.Load(potion.amount() + 1);
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
    
    last_login_ = std::time(nullptr);

    Save();

    for(auto &potion : potions_) {
      if(potion.active()) {
        if(!potion.loop()) {
          if(potion.type() == MagicPotion::Type::kApples) {
            apples_ += potion.multiplier() * potion.amount();
          } else if(potion.type() == MagicPotion::Type::kGold) {
            gold_ += potion.multiplier() * potion.amount();
          } else {
            platinum_ += potion.multiplier() * potion.amount();
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

      if(potions_[0].active() ||
         potions_[1].active() ||
         potions_[2].active()) {
        
        std::cout << "Active Potions: ";

        if(potions_[0].active()) { std::cout << potions_[0].ansi() << potions_[0].name() << "(" << potions_[0].amount() << ")\033[0m "; };
        if(potions_[1].active()) { std::cout << potions_[1].ansi() << potions_[1].name() << "(" << potions_[1].amount() << ")\033[0m "; };
        if(potions_[2].active()) { std::cout << potions_[2].ansi() << potions_[2].name() << "(" << potions_[2].amount() << ")\033[0m "; };
        
        std::cout << std::endl << std::endl;

      }
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
                picker.Load(0);
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
