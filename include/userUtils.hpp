#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "point.hpp"
#include "bcurve.hpp"

enum State {
  Normal, 
  Move,
  AddCurve,
  AddPoint, 
  PlayAnimation,
  Delete,
  Saving,
  PickColor,
  Help,
  DeleteFrame,
  AddFrame
};

#define STR_VALUE(arg)      #arg
/*
std::string state_to_str(State st) {
  switch(st) {
    case Normal:
      return "Normal";
      break;
    case Move:
      return "Move";
      break;
    case AddCurve:
      return  "AddCurve";
      break;
    case AddPoint:
      return  "AddPoint";
      break;
    case PlayAnimation:
      return  "PlayAnimation";
      break;
    case Delete:
      return  "Delete";
      break;
    case Saving:
      return  "Saving";
      break;
    case PickColor:
      return  "PickColor";
      break;
    case Help:
      return  "Help";
      break;
  }
}
*/
struct DrawingSettings {
  float old_thick{3.0f};
  sf::Color old_col{sf::Color::Green};
  float selected_thick{3.0f};
  sf::Color selected_col{sf::Color::Green};
  float thickness{3.0f};
  sf::Color color{sf::Color::Green};

  void update_settings(bool& selected_curve, std::shared_ptr<BCurve>& active_curve) {
    if(active_curve) {
      if(!selected_curve) {
        selected_curve = true;
        old_col = selected_col;
        old_thick = selected_thick;
        selected_thick = active_curve->get_thickness();
        selected_col = active_curve->get_color();
      } else {
        active_curve->set_color(selected_col);
        active_curve->set_thickness(selected_thick);
        color = selected_col;
        thickness = selected_thick;
      }
    } else {
      if(selected_curve) {
        selected_col = old_col;
        selected_thick = old_thick;
      } else {
        color = selected_col;
        thickness = selected_thick;
      }
      selected_curve = false;
    }
  }
};

struct InputState {
  bool left_mouse_down = false;
  bool z_pressed = false;
  bool ctrl_pressed = false;
  bool selected_curve = false;
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
