#include "../include/animationManager.hpp"

AnimationManager::AnimationManager(std::shared_ptr<Frames>& _frames,
    std::shared_ptr<Frame>& af): 
  frames {_frames}, active_frame{af} {
}

unsigned AnimationManager::next_frame(float dt) {
    current_time_between_frames += dt;
    float time_to_switch = 1.0f / (float)fps;
    if(current_time_between_frames >= time_to_switch) {
      animation_frame_index = (animation_frame_index + 1) % frames->size();
      current_time_between_frames = 0.0;
    }
    //std::cout << "frames size = " << frames->size() << std::endl;
    //std::cout << "current animation frame = " << animation_frame_index << std::endl;
    if(animation_frame_index >= frames->size()) {
      animation_frame_index = 0;
    }
    return animation_frame_index;
}

void AnimationManager::play_animation(float dt) {
  unsigned anim_id = next_frame(dt);
  active_frame = (*frames)[anim_id];
}

unsigned AnimationManager::get_fsp() const {
  return fps;
}

