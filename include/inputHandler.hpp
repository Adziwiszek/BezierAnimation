#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

#include "frame.hpp"
#include "userUtils.hpp"
#include "animationState.hpp"
#include "uiManager.hpp"

class KeyEvent {
public:
  virtual void action() = 0;
  sf::Keyboard::Key get_key() {return key;}
protected:
  sf::Keyboard::Key key;
};

class InputHandler {
public:
  InputHandler(std::shared_ptr<Frame>& active_frame,
              std::shared_ptr<Frames>& frames,
              State& current_state,
              std::vector<std::string>& actions,
              AnimationState& _anim_state,
              UI::Manager& _uiman,
              DrawingSettings& ds);

  void handle_event(sf::Event event, InputState& input);
  void update(const InputState& input);
  //move down eventually 
  void handle_mouse_movement(const InputState& input);
  void handle_mouse_pressed(InputState& input);
  void handle_key_pressed(sf::Keyboard::Key key, const InputState& input);
  void handle_mouse_release(const InputState& input);
  void add_frame(bool copy_frame);
  void switch_to_state(State new_state, const std::string& state_name);
private:
  void add_new_curve(Vec2f pos);
  void add_point_to_active_curve(Vec2f pos);
  void handle_point_deletion();
  void handle_curve_deletion();
  void move_active_point(Vec2f pos);
  void move_active_curver(Vec2f pos);

  void next_frame();
  void prev_frame();
  void delete_current_frame();

  DrawingSettings& drawing_settings;
  AnimationState& animation_state;
  UI::Manager& ui_manager;
  std::shared_ptr<Frame>& active_frame;
  std::shared_ptr<Frames>& frames;
  State& current_state;
  std::vector<std::string>& actions;
};
