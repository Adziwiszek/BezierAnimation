#pragma once

#include <fstream>
#include <ranges>
#include "frame.hpp"

class AnimationState {
public:
  std::shared_ptr<Frames> frames; 
  std::shared_ptr<Frame> active_frame;

  AnimationState(std::shared_ptr<Frames> _frames);
  void next_frame(); 
  void prev_frame();
  void add_frame(bool copy_frame);

  unsigned get_frame_index();
  std::shared_ptr<Frame> get_active_frame();

  void save_to_file(std::string filename);
  void load_from_file(std::string filename);
private:
  unsigned frame_counter {0};
  unsigned frame_index {0};
};

