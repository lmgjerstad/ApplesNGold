#include "ApplePickerUpgrade.h"
#include "absl/strings/str_format.h"

ApplePickerUpgrade::ApplePickerUpgrade(std::string name, float multiplier,
                                       int max, float base_price)
    : name_(std::move(name)), multiplier_(multiplier), max_(max),
      base_price_(base_price), level_(0) {}

bool ApplePickerUpgrade::upgrade() {
  if (level_ < max_) {
    ++level_;
    std::cout << std::endl << "You bought a(n) " << name_ << "!" << std::endl;
    return true;
  }
  std::cout << std::endl << "Oops! It is at it's max level!" << std::endl;
  return false;
}

float ApplePickerUpgrade::cost() const {
  return std::pow(multiplier_, level_) * base_price_;
}

int ApplePickerUpgrade::pick() const { return multiplier_ * level_; }

bool ApplePickerUpgrade::Load(const applesngold::Picker &proto) {
  if (proto.name() != name()) {
    return false;
  }
  level_ = proto.level();
  return true;
}

std::string ApplePickerUpgrade::StoreLabel() const {
  return absl::StrFormat("%s - %0.02f Gold - %d/%d", name_, cost(), level_,
                         max_);
}

applesngold::Picker ApplePickerUpgrade::Save() const {
  applesngold::Picker proto;
  proto.set_name(name());
  proto.set_level(level_);
  return proto;
}
