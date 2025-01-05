#include "../include/animationState.hpp"


AnimationState::AnimationState(std::shared_ptr<Frames> _frames):
  frames{std::move(_frames)}, active_frame{} {}

void AnimationState::next_frame() {
  if(frame_index < frames->size() - 1) { 
    active_frame = (*frames)[++frame_index];
  }
}
void AnimationState::prev_frame() {
  if(frame_index > 0) { 
    active_frame = (*frames)[--frame_index];
  }
}
void AnimationState::add_frame(bool copy_frame) {
  std::shared_ptr<Frame> new_frame;
  if(copy_frame) {
    new_frame = std::make_shared<Frame>((*frames)[frame_index], frame_counter++);
  } else { 
    new_frame = std::make_shared<Frame>(frame_counter++);
  }
  // if frame we are adding is first we do special stuff
  if(frames->empty() || frame_index == frames->size()-1) {
    frames->push_back(new_frame);
  } else {
    frames->insert(frames->begin() + frame_index + 1, new_frame);
  }
}
void AnimationState::save_to_file(std::string filename) {

}
void AnimationState::load_from_file(std::string filename) {

}

