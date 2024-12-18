#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <tuple>
#include <array>

#include "point.hpp"
#include "bcurve.hpp"
#include "frame.hpp"

struct InputState {
  bool left_mouse_down = false;
  bool z_pressed = false;
  bool ctrl_pressed = false;
  Vec2f mouse_position;
  Vec2f prev_mouse_position { 0.0f, 0.0f }; 
  Vec2f mouse_delta;
  std::array<bool, sf::Keyboard::KeyCount> keys;

  void update_mouse(sf::RenderWindow& window) {
    prev_mouse_position = mouse_position;
    Vec2f current_mouse_position = 
      static_cast<Vec2f>(sf::Mouse::getPosition(window)); 
    mouse_delta = current_mouse_position - prev_mouse_position;
    mouse_position = current_mouse_position;
  }

  void update_key(sf::Keyboard::Key key, bool is_pressed) {
    keys[static_cast<std::size_t>(key)] = is_pressed;
  }
};

using Frames = std::vector<std::shared_ptr<Frame>>;

class User {
private:
  enum State {
    Normal, 
    Move,
    AddCurve,
    AddPoint
  };

  //std::shared_ptr<BCurve> active_curve;
  //std::shared_ptr<Point> active_point;
  //vector<std::shared_ptr<BCurve>> curves;

  unsigned frame_counter {0};
  Frames frames; 
  Frames::iterator f_iter;
  std::shared_ptr<Frame> active_frame;

  State current_state { Normal };
public:
  User(); 
  void handle_input(sf::Event event, InputState& input); 
  void handle_mouse_pressed(const InputState& input);
  void handle_key_pressed(sf::Keyboard::Key key, const InputState& input);
  void update(const InputState& input); 
  void switch_to_state(State new_state, const std::string& state_name);
  
  void add_new_curve(Vec2f);
  void add_point_to_current_curve(Vec2f);
 
  void add_frame();
  void next_frame();
  void prev_frame();

  void draw_curve_points(sf::RenderWindow *window); 
  void draw_convex_hull(sf::RenderWindow *window);
  void draw_bezier_curve(sf::RenderWindow *window);
  void draw(sf::RenderWindow *window);
};
