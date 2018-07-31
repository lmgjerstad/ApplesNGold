#include "ApplesNGold/IdlePicker.h"
#include "absl/strings/str_format.h"

IdlePicker::IdlePicker(std::string name, float multiplier, float base_price)
    : name_(name), multiplier_(multiplier), base_price_(base_price),
      amount_(0) {}

IdlePicker::~IdlePicker() {}

std::string IdlePicker::StoreLabel() const {
  return absl::StrFormat("%s - %0.02f Gold - %0.02f Apples/sec - %d", name_,
                         cost(), multiplier(), amount_);
}

bool IdlePicker::Load(const applesngold::Idle &proto) {
  if (proto.name() != name()) {
    return false;
  }
  amount_ = proto.amount();
  return true;
}

applesngold::Idle IdlePicker::Save() const {
  applesngold::Idle proto;
  proto.set_name(name());
  proto.set_amount(amount_);
  return proto;
}
