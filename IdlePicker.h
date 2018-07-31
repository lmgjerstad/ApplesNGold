#ifndef __IDLEPICKER_H_INCLUDED__
#define __IDLEPICKER_H_INCLUDED__

#include <string>

#include "ApplesNGold/apples_n_gold.pb.h"

class IdlePicker {
public:
  IdlePicker(std::string name, float multiplier, float base_price);
  ~IdlePicker();

  const std::string &name() const { return name_; }

  float multiplier() const { return multiplier_; }

  float base_price() const { return base_price_; }

  float cost() const { return multiplier_ * base_price_; }

  int amount() const { return amount_; }

  void Load(int amount) { amount_ = amount; }

  std::string StoreLabel() const;

  bool Load(const applesngold::Idle &proto);

  applesngold::Idle Save() const;

private:
  const std::string name_;
  const float multiplier_;
  const float base_price_;
  int amount_;
};

#endif // __IDLEPICKER_H_INCLUDED__

