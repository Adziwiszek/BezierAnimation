#include "../include/user.hpp"
#include <fstream>
#include <ranges>

User::User(sf::RenderWindow& _window) : 
    input_handler(frames, active_frame, current_state, 
    frame_counter, frame_index, actions), drawer(_window) {
  //curves.push_back(BCurve()); 
  //active_curve = &curves[0];
  //active_curve = nullptr;
  add_frame(false);
  active_frame = frames[0];
  frame_index = 0;
}

User::User(Frames frames, unsigned fc, sf::RenderWindow& _window)
  : frames {frames}, frame_counter {fc}, drawer(_window),
    input_handler(frames, active_frame, current_state, 
    frame_counter, frame_index, actions){
  active_frame = frames[0];
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
  output << std::to_string(frames.size()) << std::endl;
  for(const auto& frame: frames) {
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

void User::handle_key_pressed(sf::Keyboard::Key key, const InputState& input) {
  // TODO: fix this switch hell
  switch (key) {
    case sf::Keyboard::Key::G:
      switch_to_state(State::AddCurve, "AddCurve");
      break;
    case sf::Keyboard::Key::N:
      switch_to_state(State::Normal, "Normal");
      break;
    case sf::Keyboard::Key::M:
      switch_to_state(State::Move, "Move");
      break;
    case sf::Keyboard::Key::H:
      switch_to_state(State::AddPoint, "AddPoint");
      break;
    case sf::Keyboard::Key::A:
      switch_to_state(State::PlayAnimation, "PlayAnimation");
      break;
    case sf::Keyboard::Key::S:
      save_to_file("test.txt");
      break;
    case sf::Keyboard::Key::Up:
      fps += 1;
      break;
    case sf::Keyboard::Key::Down:
      fps -= 1;
      break;
    case sf::Keyboard::Key::F:
      add_frame(true);
      next_frame();
      actions.push_back("added frame");
      break;
    case sf::Keyboard::Key::E:
      actions.push_back("next frame");
      next_frame();
      break;
    case sf::Keyboard::Key::Q:
      actions.push_back("previous frame");
      prev_frame();
      break;
    case sf::Keyboard::Key::D:
      switch_to_state(State::Delete, "Delete point");
      break;
    case sf::Keyboard::Key::Z:
      /*if(active_curve && input.ctrl_pressed) {
        active_curve->undo_last_point();
      }*/
      break;
    default:
      break;
  }
}

void User::handle_input(sf::Event event, InputState& input) {
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      input.left_mouse_down = true;
      input.mouse_position = {(float)event.mouseButton.x, (float)event.mouseButton.y};
      //handle_mouse_pressed(input);
      input_handler.handle_mouse_pressed(input);
    }
  }
  if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      //active_point = nullptr;
      input.left_mouse_down = false;
    }
  }

  if(event.type == sf::Event::KeyPressed) {
    input.update_key(event.key.code, true);
    handle_key_pressed(event.key.code, input);
  }
  if(event.type == sf::Event::KeyReleased) {
    if(event.key.scancode == sf::Keyboard::Scan::Z) {
      input.update_key(event.key.code, false);
    }
  }
}

unsigned User::get_fps() {
  return fps;
}

unsigned update_index(Frames &frames, Frames::iterator it) {
  return (unsigned)std::distance(frames.begin(), it);
}

unsigned User::get_frame_index() {
  return frame_index + 1;
}

unsigned User::get_frame_count() {
  return frames.size();
}

void User::add_frame(bool copy_frame) {
  std::shared_ptr<Frame> new_frame;
  if(copy_frame) {
    new_frame = std::make_shared<Frame>(*frames[frame_index], frame_counter++);
  } else { 
    new_frame = std::make_shared<Frame>(frame_counter++);
  }
  // if frame we are adding is first we do special stuff
  if(frames.empty() || frame_index == frames.size()-1) {
    frames.push_back(new_frame);
  } else {
    frames.insert(frames.begin() + frame_index + 1, new_frame);
  }
}

void User::next_frame() {
  if(frame_index < frames.size() - 1) { 
    active_frame = frames[++frame_index];
  }
}

void User::prev_frame() {
  if(frame_index > 0) { 
    active_frame = frames[--frame_index];
  }
}

void User::switch_to_state(State new_state, const std::string& state_name="some state") {
  if (current_state != new_state) {
    std::cout << "Switched to " << state_name << "!\n";
    current_state = new_state;
    if(current_state != PlayAnimation) {
      active_frame = frames[frame_index];
    } else {
      animation_frame_index = 0;
    }
  }
}

void User::add_new_curve(Vec2f pos) {
  if(active_frame) {
    active_frame->add_curve(pos);
    std::cout << "DUPA1\n";
    current_state = State::AddPoint;
  }
}

void User::add_point_to_current_curve(Vec2f pos) {
  if(active_frame) {
    active_frame->add_point_to_current_curve(pos);
    /*std::cout <<
      "spawned point for curve with id = " << active_curve->get_id() << std::endl;
    */
  }
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
    current_time_between_frames += input.dt;
    float time_to_switch = 1.0f / (float)fps;
    if(current_time_between_frames >= time_to_switch) {
      animation_frame_index = (animation_frame_index + 1) % frames.size();
      current_time_between_frames = 0.0;
    }
    active_frame = frames[animation_frame_index];
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




