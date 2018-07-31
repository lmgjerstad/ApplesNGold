#ifndef __MAGICPOTION_H_INCLUDED__
#define __MAGICPOTION_H_INCLUDED__

#include <string>

#include "ApplesNGold/apples_n_gold.pb.h"

class MagicPotion {
public:
  enum class Type { kApples, kGold, kPlatinum };

  MagicPotion(std::string name, Type type, int duration, float multiplier,
              float price, std::string ansi);

  float cost() const { return price_; }

  int duration() const { return duration_; }

  Type type() const { return type_; }

  const std::string& name() const { return name_; }

  int roundNum() const { return roundNum_; }

  void setRoundNum(int num) { roundNum_ = num; }

  float multiplier() const { return multiplier_; }
  
  int amount() const { return amount_; }
  
  void Load(int amount) { amount_ = amount; }

  bool Load(const applesngold::Potion& proto);

  applesngold::Potion Save() const;

  const std::string& ansi() { return ansi_; }

  bool loop();

  std::string StoreLabel() const;

  bool active() const;

private:
  const std::string name_;
  const Type type_;
  const int duration_;
  const float multiplier_;
  const float price_;
  const std::string ansi_;

  int roundNum_;

  int amount_;
};

#endif // __MAGICPOTION_H_INCLUDED__
