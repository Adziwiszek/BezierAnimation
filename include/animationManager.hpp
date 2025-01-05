#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
using std::cout;

#include "frame.hpp"

class AnimationManager {
public:
  AnimationManager(std::shared_ptr<Frames>);
  unsigned next_frame(float);

  unsigned get_fsp() const;
private:
  std::shared_ptr<Frames> frames;  
  unsigned fps {15};
  float current_time_between_frames{0};
  unsigned animation_frame_index{0};
};
