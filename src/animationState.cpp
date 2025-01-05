#include "../include/animationState.hpp"
#include <iostream>
using std::cout, std::endl;

AnimationState::AnimationState(std::shared_ptr<Frames> _frames):
  frames{std::move(_frames)}, active_frame{} {}

void AnimationState::next_frame() {
  if(frame_index < frames->size() - 1) { 
    active_frame = (*frames)[++frame_index];
    cout<<"new frame id = "<<active_frame->get_id()<<endl;
  }
}
void AnimationState::prev_frame() {
  if(frame_index > 0) { 
    active_frame = (*frames)[--frame_index];
    cout<<"new frame id = "<<active_frame->get_id()<<endl;
  }
}
void AnimationState::add_frame(bool copy_frame) {
  std::shared_ptr<Frame> new_frame;
  if(copy_frame) {
    new_frame = std::make_shared<Frame>(*((*frames)[frame_index]), ++frame_counter);
  } else { 
    new_frame = std::make_shared<Frame>(++frame_counter);
  }
  cout<<"created frame with id = "<<new_frame->get_id()<<endl;
  // if frame we are adding is first we do special stuff
  if(frames->empty() || frame_index == frames->size()-1) {
    frames->push_back(new_frame);
  } else {
    frames->insert(frames->begin() + frame_index + 1, new_frame);
  }
}

unsigned AnimationState::get_frame_index() {
  return frame_index;
}

std::shared_ptr<Frame> AnimationState::get_active_frame() {
  return frames->at(frame_index);
}

void AnimationState::save_to_file(std::string filename) {

}
void AnimationState::load_from_file(std::string filename) {

}

