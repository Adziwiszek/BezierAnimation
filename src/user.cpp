#include "../include/user.hpp"
#include <fstream>
#include <ranges>

User::User(sf::RenderWindow& _window) : 
    frames{std::make_shared<Frames>()},
    drawer(_window), 
    animation_manager(frames),
    animation_state(frames),
    input_handler(frames, active_frame, current_state, 
    frame_counter, frame_index, actions, animation_state) {

  input_handler.add_frame(false);
  active_frame = frames->at(0);
  frame_index = 0;
}

User::User(Frames _frames, unsigned fc, sf::RenderWindow& _window)
  : frames {std::make_shared<Frames>(std::move(_frames))}, 
    frame_counter {fc}, 
    drawer(_window),
    animation_manager(frames),
    animation_state(frames),
    input_handler(frames, active_frame, current_state, 
        frame_counter, frame_index, actions, animation_state) {
  
  active_frame = frames->at(0);
  frame_index = 0;
}

void put_to_stream(std::ofstream& output, std::string name, float x, float y, 
      int fid, int cid) {
  output << name << " " 
    << x << " " 
    << y << " "
    << fid << " " 
    << cid << std::endl;
}

void User::save_to_file(std::string path) {
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

void User::handle_input(sf::Event event, InputState& input) {
  input_handler.handle_event(event, input);
}

unsigned User::get_fps() {
  return animation_manager.get_fsp();
}

unsigned update_index(Frames &frames, Frames::iterator it) {
  return (unsigned)std::distance(frames.begin(), it);
}

unsigned User::get_frame_index() {
  return frame_index + 1;
}

unsigned User::get_frame_count() {
  return frames->size();
}

void User::update(const InputState& input) {
  //handling input here idk
  input_handler.handle_mouse_movement(input);

  // we dropped of a point
  if(!input.left_mouse_down && active_frame->active_point && 
      active_frame->active_point->started_moving) {
    actions.push_back("moved point");
    std::cout << "-------------\nmoved point\nparent curve id = " 
      << active_frame->active_point->get_parent_id() 
      << "\npoint id = " << active_frame->active_point->get_id()
      << "\npoint pos = " << active_frame->active_point->x << ", " 
      << active_frame->active_point->y << std::endl;
    active_frame->active_point->started_moving = false;
    active_frame->active_point = nullptr;
  }

  // we dropped of a curve
  if(!input.left_mouse_down && active_frame->active_curve && 
      active_frame->active_curve->started_moving) {
    actions.push_back("moved curve");
    std::cout << "-------------\nmoved curve\n" 
      << "id = "<< active_frame->active_curve->get_id() <<std::endl;
    active_frame->active_curve->started_moving = false;
    active_frame->active_curve = nullptr;
  }

  if(current_state == State::PlayAnimation) {
    unsigned anim_id = animation_manager.next_frame(input.dt);
    active_frame = (*frames)[anim_id];
  }

  for(auto& curve: active_frame->curves) {
    curve->update();
  }
}

void User::draw(sf::RenderWindow *window) {
  for(auto curve: active_frame->curves) {
    // drawing control points
    if(current_state != State::PlayAnimation) {
      bool active = false;
      if(active_frame->active_curve && 
          curve->get_id() == active_frame->active_curve->get_id())
        active = true;
      drawer.draw_control_points(curve->get_control_points(), active);
    }
    // drawing bezier curve
    drawer.draw_bc_lines(curve->get_bc_line_points(), sf::Color::Green, 3.0);
    // drawing convex hull
    // drawer.draw_convex_hull(curve->get_convex_hull_points());
  }
}

