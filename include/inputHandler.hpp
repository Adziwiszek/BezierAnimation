#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

#include "frame.hpp"
#include "userUtils.hpp"

class KeyEvent {
public:
  virtual void action() = 0;
  sf::Keyboard::Key get_key() {return key;}
protected:
  sf::Keyboard::Key key;
};

class InputHandler {
public:
  InputHandler(std::vector<std::shared_ptr<Frame>>& frames, 
              std::shared_ptr<Frame>& active_frame,
              State& current_state,
              unsigned& frame_counter,
              unsigned& frame_index,
              std::vector<std::string>& actions);

  void handle_event(sf::Event event, InputState& input);
  void update(const InputState& input);
  void handle_mouse_movement(const InputState& input);
private:
  void handle_mouse_pressed(const InputState& input);
  void handle_key_pressed(sf::Keyboard::Key key, const InputState& input);
  void handle_mouse_release(const InputState& input);

  void add_new_curve(Vec2f pos);
  void add_point_to_current_curve(Vec2f pos);
  void handle_point_deletion();
  void handle_curve_deletion();
  void switch_to_state(State new_state, const std::string& state_name);
  void move_active_point(Vec2f pos);
  void move_active_curver(Vec2f pos);

  void add_frame(bool copy_frame);
  void next_frame();
  void prev_frame();

  std::vector<std::shared_ptr<Frame>>& frames;
  std::shared_ptr<Frame>& active_frame;
  State& current_state;
  unsigned& frame_counter;
  unsigned& frame_index;
  std::vector<std::string>& actions;
};