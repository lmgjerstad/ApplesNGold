#include "ApplesNGold/MagicPotion.h"
#include "absl/strings/str_format.h"

MagicPotion::MagicPotion(std::string name, Type type, int duration,
                         float multiplier, float price, std::string ansi)
    : name_(name), type_(type), duration_(duration), multiplier_(multiplier),
      price_(price), ansi_(ansi), roundNum_(0), amount_(0) {}

bool MagicPotion::Load(const applesngold::Potion& proto) {
  if (proto.name() != name()) {
    return false;
  }
  amount_ = proto.amount();
  roundNum_ = proto.round();
  return true;
}

applesngold::Potion MagicPotion::Save() const {
  applesngold::Potion proto;
  proto.set_name(name());
  proto.set_amount(amount_);
  proto.set_round(roundNum_);
  return proto;
}

bool MagicPotion::loop() {
  if (roundNum_ <= duration_ && roundNum_ != 0) {
    ++roundNum_;
    return false;
  } else if (roundNum_ == 0) {
    roundNum_ = 1;
    return false;
  }

  roundNum_ = 0;
  return true;
}

std::string MagicPotion::StoreLabel() const {
  switch (type_) {
  case Type::kApples:
    return absl::StrFormat("%s%s\033[0m - %0.02f Gold - %d Round(s) for %d "
                           "apples each - %d active",
                           ansi_, name_, cost(), duration(), (int)multiplier(),
                           amount_);
    break;
  case Type::kGold:
    return absl::StrFormat("%s%s\033[0m - %d Apples - %d Round(s) for %0.02f "
                           "gold each - %d active",
                           ansi_, name_, (int)cost(), duration(), multiplier(),
                           amount_);
    break;
  case Type::kPlatinum:
    return absl::StrFormat("%s%s\033[0m - %0.02f Gold - %d Round(s) for %d "
                           "platinum each - %d active",
                           ansi_, name_, cost(), duration(), (int)multiplier(),
                           amount_);
    break;
  default:

    break;
  }
}

bool MagicPotion::active() const {
  if (roundNum_ <= duration() && roundNum_ != 0) {
    return true;
  } else {
    return false;
  }
}
