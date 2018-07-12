#include <iostream>
#include <cstdlib>

#include "menu.h"

void Menu::AddOption(std::string text, std::function<void()> callback) {
  std::unique_ptr<Option> option(new Option);
  option->text = std::move(text);
  option->callback = std::move(callback);
  display_options_.push_back(option.get());
  options_.push_back(std::move(option));
}

Menu::Result Menu::Execute() {
  int i = 1;
  for (const auto* option : display_options_) {
    if (option == nullptr) {
      std::cout << std::endl;
    } else {
      std::cout << i << ": " << option->text << std::endl;
      ++i;
    }
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

  options_[selection - 1]->callback();
  return Result::kAction;
}

void Menu::BlankLine() {
  display_options_.emplace_back(nullptr);
}
