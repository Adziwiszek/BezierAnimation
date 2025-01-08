#include "../include/animationState.hpp"
#include <iostream>
using std::cout, std::endl;

AnimationState::AnimationState(std::shared_ptr<Frames> _frames,
    AnimationManager& am, std::shared_ptr<Frame>& af):
  frames{_frames}, active_frame{af}, animation_manager{am} {}

void AnimationState::next_frame() {
  if(frame_index < frames->size() - 1) { 
    active_frame = (*frames)[++frame_index];
    /*std::cout << "new frame id = " << active_frame->get_id() << std::endl;
    std::cout << "frames size = " << frames->size() << std::endl;*/
  }
}
void AnimationState::prev_frame() {
  if(frame_index > 0) { 
    active_frame = (*frames)[--frame_index];
    /*std::cout << "new frame id = " << active_frame->get_id() << std::endl;
    std::cout << "frames size = " << frames->size() << std::endl;*/
  }
}


void AnimationState::add_frame(bool copy_frame) {
  std::shared_ptr<Frame> new_frame;
  if(copy_frame) {
    new_frame = std::make_shared<Frame>(*((*frames)[frame_index]), frame_counter++);
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
    << cid << " ";
}

void AnimationState::save_to_file(std::string path) {
  std::ofstream output(path);
  if(!output) {
    std::cerr << "Error: failed to open: " << path << std::endl;
    return;
  }
  output << std::to_string(frames->size()) << std::endl;
  for(const auto& frame: *frames) {
    cout << "f_id saving = " << frame->get_id() << endl;
    for(auto [curve_id, curve] : frame->curves | std::views::enumerate) {
      cout << "adding curve\n";
      bool started {false};
      for(const auto& point: curve->get_control_points()) {
        if(!started) {
          //cout << "colors = " << (int)curve->get_color().r << (int)curve->get_color().g << (int)curve->get_color().b << endl;
          put_to_stream(output, "ADDC", point->x, point->y, frame->get_id(), 0);
          output << (int)curve->get_color().r << " " <<  (int)curve->get_color().g << " " << (int)curve->get_color().b << " "; 
          output << curve->get_thickness();
          output << std::endl;
          started = true;
        } else {
          put_to_stream(output, "ADDP", point->x, point->y, 
              frame->get_id(), curve_id);
          output << 1 << " " << 1 << " " << 1 << " ";
          output << 1 << " ";
          output << std::endl;
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

  frames->clear();
  frames->reserve(n_frames);

  for(int i = 0; i < n_frames; i++) {
    frames->push_back(std::make_shared<Frame>(i));
  }

  while(std::getline(input, line)) {
    std::istringstream stream(line);
    std::string action, _x, _y, _f_id, _c_id, r, g, b, t;
    if(stream >> action >> _x >> _y >> _f_id >> _c_id >> r >> g >> b >> t) {
      float x = std::stof(_x);
      float y = std::stof(_y);
      int f_id = std::stoi(_f_id);
      int c_id = std::stoi(_c_id);
      sf::Color col {(sf::Uint8)std::stoi(r), (sf::Uint8)std::stoi(g), (sf::Uint8)std::stoi(b)};
      float thick = std::stof(t);
      if(action == "ADDC") {
        /*cout<<"f_id = "<<f_id<<endl; 
        cout<<"frames size = "<<frames.size()<<endl;
        cout << "thick = " << thick << endl;
        cout << "colors = " << (int)col.r << (int)col.g << (int)col.b << endl;*/
        frames->at(f_id)->add_curve({x, y}, thick, col); 
        //cout << "DUPA1\n";
      } else if(action == "ADDP") {
        frames->at(f_id)->active_curve = frames->at(f_id)->curves[c_id];
        frames->at(f_id)->add_point_to_current_curve({x, y}); 
      } 
    } else {
      std::cerr << "file error" << std::endl;
    }
  }
  std::cout << "success!" << std::endl;
  input.close();
}

