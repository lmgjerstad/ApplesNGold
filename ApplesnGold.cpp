#include <iostream>
#include <string>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <term.h>

#include <sys/stat.h>

std::string name;
std::string filename;
int apples = 0;
float gold = 0;
int roundNum = 0;

// Prestige mechanics
int platinum = 0;
int platinumPrestige = 0;
float lifetimeGold = 0;

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

class ApplePickerUpgrade {
 public:
  ApplePickerUpgrade(std::string name, int multiplier, int max) :
                     name_(std::move(name)),
                     multiplier_(multiplier),
                     max_(max),
                     level_(0) {}

  bool upgrade() {
    if (level_ < max_) {
      ++level_;
      return true;
    }
    return false;
  }

  float cost() {
    return multiplier_ * (level_ + 1) * .5;
  }

  int pick() {
    return multiplier_ * level_;
  }

  int level() {
    return level_;
  }

  void load(int level) {
    level_ = std::min(level, max_);
  }
  
  std::string name() {
    return name_;
  }

  std::string StoreLabel() {
    return string_format("%s - %0.02f Gold - %d/%d", name_.c_str(), cost(), level_, max_);
  }

 private:
  const std::string name_;
  const int multiplier_;
  const int max_;
  int level_;
};

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
  std::cout << "Are you sure you want to prestige for " << platinumPrestige << " platinum? (y/n)" << std::endl;
  std::string ans;
  std::cin >> ans;

  if(ans == "y") {
    return true;
  } else if(ans == "n") {
    return false;
  } else {
    std::cout << "Sorry! Didn't understand that!" << std::endl;
    sleep(2);
    return false;
  }
}

void shop() {
  gold = std::floorf(gold * 100) / 100;
  lifetimeGold = std::floorf(lifetimeGold * 100) / 100;
  std::cout << "\033[2J\033[HSHOP" << std::endl;
  std::cout << "\033[1;93mGold: " << gold << "\033[0m" << std::endl << std::endl;
  std::cout << "1: " << applePickers.StoreLabel() << std::endl;
  std::cout << "2: " << wizards.StoreLabel() << std::endl;
  std::cout << "Enter a number OR enter q/Q." << std::endl << std::endl;
  std::string ans;
  std::cin >> ans;
  
  if(ans == "1") {
    const float cost = applePickers.cost();
    if(gold >= cost) {
      if(applePickers.upgrade()) {
        gold -= cost;
        std::cout << std::endl <<  "You bought an apple picker!" << std::endl;
        sleep(1);
        prepareSaveData();
        shop();
        return;
      } else {
        std::cout << std::endl << "Oops! It is at it's max level!" << std::endl;
        sleep(2);
      }
    } else {
      std::cout << std::endl << "Oops! It looks like you can't afford that!" << std::endl;
      sleep(2);
      prepareSaveData();
      shop();
      return;
    }
  } else if(ans == "2") {
    const float cost = wizards.cost();
    if(gold >= cost) {
      if(wizards.upgrade()) {
        gold -= cost;
        std::cout << std::endl <<  "You bought a wizard!" << std::endl;
        sleep(1);
        prepareSaveData();
        shop();
        return;
      } else {
        std::cout << std::endl << "Oops! It is at it's max level!" << std::endl;
        sleep(2);
      }
    } else {
      std::cout << std::endl << "Oops! It looks like you can't afford that!" << std::endl;
      sleep(2);
      prepareSaveData();
      shop();
      return;
    }
  } else if(ans == "q" || ans == "Q") {
      prepareSaveData();
      return;
  } else {
      std::cout << "Sorry! Didn't understand that." << std::endl;
      sleep(2);
      shop();
      return;
  }
}

void round(bool restarted) {
  gold = std::floorf(gold * 100) / 100;
  lifetimeGold = std::floorf(lifetimeGold * 100) / 100;
  prepareSaveData();
  system("clear");
  if(!restarted) {
    ++roundNum;
  } else {
    std::cout << "RESTARTED ";
  }
  std::cout << "ROUND " << roundNum << std::endl;
  std::cout << "\033[1;91mApples: " << apples << "\033[0m" << std::endl;
  std::cout << "\033[1;93mGold: " << gold << "\033[0m" << std::endl << std::endl;
  if(platinum > 0) {
    std::cout << "\033[1;36mPlatinum: " << platinum << "\033[0m"<< std::endl << std::endl;
  }
  std::cout << "Would you like to pick an apple? (y/n)" << std::endl;
  std::string ans;
  std::cin >> ans;
  
  if(ans == "y") {
    int prevApples = apples;
    apples += 1 + applePickers.pick() + wizards.pick();
    if(apples - prevApples == 1) {
      std::cout << "You picked an apple!" << std::endl;
    } else {
      std::cout << "You picked " << apples - prevApples << " apples!" << std::endl;
    }
    sleep(1);
    round(false);
    return;
  } else if(ans == "n") {
    
  } else if(ans == "s") {
    shop();
    --roundNum;
    round(false);
    return;
  } else {
    std::cout << "Sorry! Didn't understand that." << std::endl;
    sleep(2);
    round(true);
    return;
  }
  
  float multiplier = (std::floorf((int)(((double)(std::rand()) / RAND_MAX / 4 + .25) * 100)) / 100) + (platinum / 100.0);
  if(platinum == 0) {
    std::cout << "Do you want to sell your apples for " << multiplier << " each? (y/n)" << std::endl;
  } else {
    std::cout << "Do you want to sell your apples for " << multiplier - (platinum / 100.0) << " + " << platinum / 100.0 << " for platinum each? (y/n)" << std::endl;
  }
  std::cin >> ans;
  
  if(ans == "y") {
    float prevGold = gold;
    gold += apples * multiplier;
    lifetimeGold += apples * multiplier;
    std::cout << "Sold " << apples << ((apples != 1) ? " apples for " : " apple for ") << gold - prevGold << " gold." << std::endl;
    apples = 0;
    sleep(2);
    round(false);
    return;
  } else if(ans == "n") {
    
  } else  if(ans == "s") {
    shop();
    --roundNum;
    round(false);
    return;
  } else {
    std::cout << "Sorry! Didn't understand that." << std::endl;
    sleep(2);
    round(true);
    return;
  }
  
  if(lifetimeGold >= 1500 || platinum > 0) {
    std::cout << "Would you like to prestige for " << platinumPrestige << " platinum? (y/n)" << std::endl;
    std::cin >> ans;

    if(ans == "y") {
      if(prestige()) {
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
        round(false);
        return;
      }
    } else if(ans == "n") {
      
    } else {
      std::cout << "Sorry! Didn't understand that!" << std::endl;
      sleep(2);
      round(true);
      return;
    }
  }

  std::cout << "Do you want to quit? (y/n)" << std::endl;
  std::cin >> ans;
  
  if(ans == "y") {
    return;
  } else if(ans == "s") {
    shop();
    --roundNum;
    round(false);
    return;
  } else { 
    --roundNum;
    round(false);
    return;
  }
  
  return;
  
}

int main(int argc, char** argv) {
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
  
  while(in >> stat >> value) {
    try {
      if(stat == "apples") {
        apples = value;
      } else if(stat == "gold") {
        gold = value;
      } else if(stat == "lifetimeGold") {
        lifetimeGold = value;
      } else if(stat == "platinum") {
        platinum = value;
      } else if(stat == "platinumGain") {
        platinumPrestige = value;
      } else if(stat == "applePickers") {
        applePickers.load(value);
      } else if(stat == "wizards") {
        wizards.load(value);
      } else {
        throw 1; // Activate catch statement
      }
    } catch(...) {
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
