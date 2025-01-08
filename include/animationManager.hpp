#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
using std::cout;

#include "frame.hpp"

class AnimationManager {
public:
  AnimationManager(std::shared_ptr<Frames>&, std::shared_ptr<Frame>&);
  unsigned next_frame(float);
  void play_animation(float dt);

  unsigned get_fsp() const;
private:
  std::shared_ptr<Frame>& active_frame;
  std::shared_ptr<Frames>& frames;  
  unsigned fps {15};
  float current_time_between_frames{0};
  unsigned animation_frame_index{0};
};
