#ifndef __MAGICPOTION_H_INCLUDED__
#define __MAGICPOTION_H_INCLUDED__

#include <string>

#include "absl/strings/str_format.h"

class MagicPotion {
public:
  enum class Type { kApples, kGold, kPlatinum };

  MagicPotion(std::string name, Type type, int duration, float multiplier,
              float price, std::string ansi)
      : name_(name), type_(type), duration_(duration), multiplier_(multiplier),
        price_(price), ansi_(ansi), roundNum_(0) {}

  float cost() { return price_; }

  int duration() { return duration_; }

  Type type() { return type_; }

  std::string name() { return name_; }

  int roundNum() { return roundNum_; }

  void setRoundNum(int num) { roundNum_ = num; }

  float multiplier() { return multiplier_; }

  bool loop() {
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

  std::string StoreLabel() {
    switch (type_) {
    case Type::kApples:
      return absl::StrFormat(
          "%s%s\033[0m - %0.02f Gold - %d Round(s) for %d apples each", ansi_,
          name_, cost(), duration(), (int)multiplier());
      break;
    case Type::kGold:
      return absl::StrFormat(
          "%s%s\033[0m - %d Apples - %d Round(s) for %0.02f gold each", ansi_,
          name_, (int)cost(), duration(), multiplier());
      break;
    case Type::kPlatinum:
      return absl::StrFormat(
          "%s%s\033[0m - %0.02f Gold - %d Round(s) for %d platinum each", ansi_,
          name_, cost(), duration(), (int)multiplier());
      break;
    default:

      break;
    }
  }

  bool active() {
    if (roundNum_ <= duration() && roundNum_ != 0) {
      return true;
    } else {
      return false;
    }
  }

private:
  const std::string name_;
  const Type type_;
  const int duration_;
  const float multiplier_;
  const float price_;
  const std::string ansi_;

  int roundNum_;
};

#endif // __MAGICPOTION_H_INCLUDED__
