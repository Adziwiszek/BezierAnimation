#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

#include "point.hpp"
#include "bcurve.hpp"
#include "frame.hpp"
#include "drawer.hpp"
#include "userUtils.hpp"

enum State {
  Normal, 
  Move,
  AddCurve,
  AddPoint, 
  PlayAnimation,
  Delete,
};

struct InputState {
  bool left_mouse_down = false;
  bool z_pressed = false;
  bool ctrl_pressed = false;
  Vec2f mouse_position;
  Vec2f prev_mouse_position { 0.0f, 0.0f }; 
  Vec2f mouse_delta;
  float dt {0};
  std::array<bool, sf::Keyboard::KeyCount> keys;

  void update_mouse(sf::RenderWindow& window) {
    prev_mouse_position = mouse_position;
    Vec2f current_mouse_position = 
      static_cast<Vec2f>(sf::Mouse::getPosition(window)); 
    mouse_delta = current_mouse_position - prev_mouse_position;
    mouse_position = current_mouse_position;
  }

  void update_dt(float _dt) { dt = _dt; }

  void update_key(sf::Keyboard::Key key, bool is_pressed) {
    keys[static_cast<std::size_t>(key)] = is_pressed;
  }
};
