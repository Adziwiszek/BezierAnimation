#pragma once

#include <SFML/Graphics.hpp>
#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <memory>

#include "point.hpp"
#include "bcurve.hpp"

template<class T>
class History {
  using T_ptr = std::shared_ptr<T>
  constexpr unsigned STACK_SIZE = 20;
  T change_stack[STACK_SIZE]; 
  // current_stack

  unsigned start {0};
  unsigned end {0};
  unsigned k {0};
public:
  History() {
    for(unsigned i = 0; i < STACK_SIZE; i++) {
      change_stack[i] = NULL;
    }
  }

  T_ptr undo() {
    return nullptr; 
  }

  T_ptr redo() {
    return nullptr; 
  }

  void add_change(T new_state) {

  }
};
