#pragma once

#include <fstream>
#include <ranges>
#include <sstream>
#include "frame.hpp"
#include "animationManager.hpp"

class AnimationState {
public:
  AnimationState(AnimationManager& am, sf::Vector2u ws);
  void next_frame(); 
  void prev_frame();
  void add_frame(bool copy_frame);
  void play_animation(float dt);
  void change_fps(int);

  unsigned get_frame_index();
  unsigned get_frame_count();
  std::shared_ptr<Frame> get_active_frame();

  void save_to_file(std::string filename);
  void load_from_file(std::string filename);
  void save_to_gif(std::string filename);

  sf::Color background_color{66,66,66};
  std::shared_ptr<Frames> frames; 
  std::shared_ptr<Frame> active_frame;
private:
  sf::Vector2u window_size;
  AnimationManager& animation_manager;
  unsigned frame_counter {0};
  unsigned frame_index {0};
};

