#include "../include/animationState.hpp"
#include "../include/gif.h"
#include "../include/drawer.hpp"
#include "../include/userUtils.hpp"
#include <iostream>

using std::cout, std::endl;

AnimationState::AnimationState(AnimationManager& am, sf::Vector2u ws):
  animation_manager{am}, active_frame{nullptr},
  frames{std::make_shared<Frames>()},
  window_size{ws} {}

void AnimationState::next_frame() {
  if(frame_index < frames->size() - 1) { 
    frame_index++;
  } else {
    frame_index = 0;
    //active_frame = (*frames)[0];
  }
  active_frame->active_curve = nullptr;
  active_frame->active_point = nullptr;
}

void AnimationState::prev_frame() {
  if(frame_index == 1) {
    frame_index = 0;
  } else if(frame_index > 0) { 
    frame_index--;
  } else if(frame_index == 0) {
    frame_index = frames->size() - 1;
  }
  active_frame->active_curve = nullptr;
  active_frame->active_point = nullptr;
}


void AnimationState::save_to_gif(std::string filename) {
  unsigned width = window_size.x;
  unsigned height = window_size.y;
  unsigned fps = animation_manager.get_fps();
  GifWriter gifWriter = {};
  GifBegin(&gifWriter, filename.c_str(), window_size.x, window_size.y,
      fps);

  sf::RenderTexture rtex;
  if(!rtex.create(window_size.x, window_size.y)) {
      std::cerr << "failed to init render texture in save_to_gif" << std::endl;
  }

  for(const auto& frame: *frames) {
    // drawing frames onto sf::Image
    rtex.clear(background_color);
    Drawer::draw_frame(rtex, frame, State::PlayAnimation);
    rtex.display();
    sf::Image frame_img = rtex.getTexture().copyToImage();

    const sf::Uint8* pixels = frame_img.getPixelsPtr();
    uint8_t gif_pixels[width * height * 4];
    for(size_t i = 0; i < width * height; i++) {
      gif_pixels[i * 4] = pixels[i * 4];
      gif_pixels[i * 4 + 1] = pixels[i * 4 + 1];
      gif_pixels[i * 4 + 2] = pixels[i * 4 + 2];
      gif_pixels[i * 4 + 3] = 255;
    }

    GifWriteFrame(&gifWriter, gif_pixels, frame_img.getSize().x, 
        frame_img.getSize().y, fps);  
  }

  GifEnd(&gifWriter);
  std::cout << "Succesfully saved gif to " << filename << std::endl;
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

void AnimationState::change_fps(int c) {
  animation_manager.set_fps((int)animation_manager.get_fps() + c);
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
    for(auto [curve_id, curve] : frame->curves | std::views::enumerate) {
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
  for(auto& frame: *frames) {
    frame->active_curve = nullptr;
    frame->active_point = nullptr;
    for(auto& curve: frame->curves) {
      curve->started_moving=true;
      curve->update();
      curve->started_moving=false;
    }
  }
  std::cout << "success!" << std::endl;
  input.close();
}

