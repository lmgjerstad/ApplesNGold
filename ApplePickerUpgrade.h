#ifndef __APPLEPICKERUPGRADE_H_INCLUDED__
#define __APPLEPICKERUPGRADE_H_INCLUDED__

template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
  size_t size =
      snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}

class ApplePickerUpgrade {
public:
  ApplePickerUpgrade(std::string name, int multiplier, int max)
      : name_(std::move(name)), multiplier_(multiplier), max_(max), level_(0) {}

  bool upgrade() {
    if (level_ < max_) {
      ++level_;
      std::cout << std::endl << "You bought a(n) " << name_ << "!" << std::endl;
      return true;
    }
    std::cout << std::endl << "Oops! It is at it's max level!" << std::endl;
    return false;
  }

  float cost() { return multiplier_ * (level_ + 1) * .5; }

  int pick() { return multiplier_ * level_; }

  int level() { return level_; }

  void load(int level) { level_ = std::min(level, max_); }

  std::string name() { return name_; }

  std::string StoreLabel() {
    return string_format("%s - %0.02f Gold - %d/%d", name_.c_str(), cost(),
                         level_, max_);
  }

private:
  const std::string name_;
  const int multiplier_;
  const int max_;
  int level_;
};

#endif // __APPLEPICKERUPGRADE_H_INCLUDE__
