#ifndef __MENU_H_INCLUDED__
#define __MENU_H_INCLUDED__

#include <functional>
#include <string>
#include <vector>

class Menu {
 public:
   Menu() {}

   void AddOption(std::string text, std::function<void()> callback);

   enum class Result { kQuit, kNoAction, kAction };

   Result Execute();
 private:
   struct Option {
     std::string text;
     std::function<void()> callback;
   };

   std::vector<Option> options_;
};

#endif  // __MENU_H_INCLUDED__
