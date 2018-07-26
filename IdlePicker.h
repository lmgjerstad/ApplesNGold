#ifndef __IDLEPICKER_H_INCLUDED__
#define __IDLEPICKER_H_INCLUDED__

#include "absl/strings/str_format.h"

class IdlePicker {
public:
  IdlePicker(std::string name, float multiplier, float base_price)
      : name_(name), multiplier_(multiplier), base_price_(base_price), amount_(0) {}
  
  std::string name() { return name_; }

  float multiplier() { return multiplier_; }

  float base_price() { return base_price_; }
  
  float cost() { return multiplier_ * base_price_; }
  
  int amount() { return amount_; }

  void loadAmount(int amount) { amount_ = amount; }

  std::string StoreLabel() {
    return absl::StrFormat("%s - %0.02f Gold - %0.02f Apples/sec - %d", name_.c_str(), cost(), multiplier(), amount_);
  }

private:
  const std::string name_;
  const float multiplier_;
  const float base_price_;
  int amount_;
};

#endif // __IDLEPICKER_H_INCLUDED__

