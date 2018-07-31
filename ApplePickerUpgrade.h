#ifndef __APPLEPICKERUPGRADE_H_INCLUDED__
#define __APPLEPICKERUPGRADE_H_INCLUDED__

#include <cmath>
#include <string>

#include "ApplesNGold/apples_n_gold.pb.h"

class ApplePickerUpgrade {
public:
  ApplePickerUpgrade(std::string name, float multiplier, int max,
                     float base_price);

  bool upgrade();

  float cost() const;

  int pick() const;

  int level() const { return level_; }

  void Load(int level) { level_ = std::min(level, max_); }

  bool Load(const applesngold::Picker& proto);

  std::string name() const { return name_; }

  std::string StoreLabel() const;

  applesngold::Picker Save() const;

private:
  const std::string name_;
  const float multiplier_;
  const int max_;
  float base_price_;
  int level_;
};

#endif // __APPLEPICKERUPGRADE_H_INCLUDE__
