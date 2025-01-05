#include "../include/animationState.hpp"
#include <iostream>
using std::cout, std::endl;

AnimationState::AnimationState(std::shared_ptr<Frames> _frames):
  frames{_frames}, active_frame{} {}

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
    new_frame = std::make_shared<Frame>(*((*frames)[frame_index]), ++frame_counter);
  } else { 
    new_frame = std::make_shared<Frame>(++frame_counter);
  }
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

unsigned AnimationState::get_frame_count() {
  return frames->size();
}

std::shared_ptr<Frame> AnimationState::get_active_frame() {
  return frames->at(frame_index);
}

void put_to_stream(std::ofstream& output, std::string name, float x, float y, 
      int fid, int cid) {
  output << name << " " 
    << x << " " 
    << y << " "
    << fid << " " 
    << cid << std::endl;
}

void AnimationState::save_to_file(std::string path) {
  std::ofstream output(path);
  if(!output) {
    std::cerr << "Error: failed to open: " << path << std::endl;
    return;
  }
  output << std::to_string(frames->size()) << std::endl;
  for(const auto& frame: *frames) {
    for(auto [curve_id, curve] : frame->curves | std::views::enumerate) {
      bool started {false};
      for(const auto& point: curve->get_control_points()) {
        if(!started) {
          put_to_stream(output, "ADDC", point->x, point->y, frame->get_id(), 0);
          started = true;
        } else {
          put_to_stream(output, "ADDP", point->x, point->y, 
              frame->get_id(), curve_id);
        }
      }
    }
  }
  output.close();
  std::cout<<"saved succesfully!"<<std::endl;
}
void AnimationState::load_from_file(std::string path) {
  std::ifstream input(path);
  if(!input) {
    std::cerr << "Error: can't open " << path << std::endl;
    return;
  }
  
  std::string line;
  std::getline(input, line);
  int n_frames = std::stoi(line);
  Frames new_frames;
  for(int i = 0; i < n_frames; i++) {
    new_frames.push_back(std::make_shared<Frame>(i));
  }
  while(std::getline(input, line)) {
    std::istringstream stream(line);
    std::string action, _x, _y, _f_id, _c_id;
    if(stream >> action >> _x >> _y >> _f_id >> _c_id) {
      float x = std::stof(_x);
      float y = std::stof(_y);
      int f_id = std::stoi(_f_id);
      int c_id = std::stoi(_c_id);
      if(action == "ADDC") {
        cout<<"DUPA\n";
        cout<<"f_id = "<<f_id<<endl; 
        cout<<"new_frames size = "<<new_frames.size()<<endl;
        new_frames[f_id]->add_curve({x, y}); 
        cout<<"DUPA2\n";
      } else if(action == "ADDP") {
        new_frames[f_id]->active_curve = new_frames[f_id]->curves[c_id];
        new_frames[f_id]->add_point_to_current_curve({x, y}); 
      } 
    } else {
      std::cerr << "file error" << std::endl;
    }
  }
  std::cout << "success!" << std::endl;
  input.close();
  frames = std::make_shared<Frames>(new_frames);
}

