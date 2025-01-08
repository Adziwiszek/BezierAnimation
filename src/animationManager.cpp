#include "../include/animationManager.hpp"

AnimationManager::AnimationManager(std::shared_ptr<Frames> _frames): 
  frames {_frames} {
}

unsigned AnimationManager::next_frame(float dt) {
    current_time_between_frames += dt;
    float time_to_switch = 1.0f / (float)fps;
    if(current_time_between_frames >= time_to_switch) {
      animation_frame_index = (animation_frame_index + 1) % frames->size();
      current_time_between_frames = 0.0;
    }

    if(animation_frame_index >= frames->size()) {
      animation_frame_index = 0;
    }

    return animation_frame_index;
}

unsigned AnimationManager::get_fsp() const {
  return fps;
}
