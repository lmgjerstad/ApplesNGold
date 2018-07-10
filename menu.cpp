#include <iostream>
#include <cstdlib>

#include "menu.h"

void Menu::AddOption(std::string text, std::function<void()> callback) {
  options_.push_back({std::move(text), std::move(callback)});
}

Menu::Result Menu::Execute() {
  for (int i = 0; i < options_.size(); ++i) {
    std::cout << i + 1 << ": " << options_[i].text << std::endl;
  }
  std::cout << "Enter a number OR enter q/Q." << std::endl << std::endl;
  std::string ans;
  std::cin >> ans;

  if (ans == "q" || ans == "Q") {
    return Result::kQuit;
  }

  int selection = strtol(ans.c_str(), nullptr, 10);
  if (selection <= 0 || selection > options_.size()) {
    std::cout << "Sorry! Didn't understand that!" << std::endl;
    return Result::kNoAction;
  }

  options_[selection - 1].callback();
  return Result::kAction;
}
