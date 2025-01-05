#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "frame.hpp"
#include "drawer.hpp"
#include "inputHandler.hpp"
#include "userUtils.hpp"
#include "animationManager.hpp"

class User {
private:
  // frames
  unsigned frame_counter {0};
  unsigned frame_index {0};
  std::shared_ptr<Frames> frames; 
  std::shared_ptr<Frame> active_frame;

  State current_state { Normal };

  Drawer drawer;
  InputHandler input_handler;
  AnimationManager animation_manager;
  AnimationState animation_state;
public:
  std::vector<std::string> actions;
  User(sf::RenderWindow& _window); 
  User(Frames, unsigned, sf::RenderWindow&);
  // handle user input
  void handle_input(sf::Event event, InputState& input); 
  // update state
  void update(const InputState& input); 
  // manage frames of animation
  unsigned get_frame_index();
  unsigned get_frame_count();
  unsigned get_fps();
  // saving files 
  void save_to_file(std::string);
  // drawing program
  void draw(sf::RenderWindow *window);
};
