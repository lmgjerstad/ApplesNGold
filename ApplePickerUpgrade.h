#ifndef __APPLEPICKERUPGRADE_H_INCLUDED__
#define __APPLEPICKERUPGRADE_H_INCLUDED__

#include <cmath>
#include <string>

#include "absl/strings/str_format.h"

class ApplePickerUpgrade {
public:
  ApplePickerUpgrade(std::string name, float multiplier, int max,
                     float base_price)
      : name_(std::move(name)), multiplier_(multiplier), max_(max),
        base_price_(base_price), level_(0) {}

  bool upgrade() {
    if (level_ < max_) {
      ++level_;
      std::cout << std::endl << "You bought a(n) " << name_ << "!" << std::endl;
      return true;
    }
    std::cout << std::endl << "Oops! It is at it's max level!" << std::endl;
    return false;
  }

  float cost() { return std::pow(multiplier_, level_) * base_price_; }

  int pick() { return multiplier_ * level_; }

  int level() { return level_; }

  void load(int level) { level_ = std::min(level, max_); }

  std::string name() { return name_; }

  std::string StoreLabel() {
    return absl::StrFormat("%s - %0.02f Gold - %d/%d", name_.c_str(), cost(),
                           level_, max_);
  }

private:
  const std::string name_;
  const float multiplier_;
  const int max_;
  float base_price_;
  int level_;
};

#endif // __APPLEPICKERUPGRADE_H_INCLUDE__
