#include "../include/user.hpp"

User::User() {
  //curves.push_back(BCurve()); 
  //active_curve = &curves[0];
  //active_curve = nullptr;
  add_frame();
  active_frame = frames[0];
  frame_index = 0;
}

void User::handle_mouse_pressed(const InputState& input) {
  if (!input.left_mouse_down) return;

  if(current_state == State::AddCurve) {
    add_new_curve(input.mouse_position);
  } else if(current_state == State::AddPoint && active_frame->active_curve) {
    add_point_to_current_curve(input.mouse_position);
  } else if(current_state == State::Normal ||
      current_state == State::Move) {
    /*auto [curve, point] = active_frame->
      get_active_point_curve(input.mouse_position);*/
    //active_frame->active_curve = curve;
    active_frame->active_point = 
      active_frame->get_active_point(input.mouse_position);
    active_frame->active_curve = 
      active_frame->get_active_curve(input.mouse_position);
  } 
}

void User::handle_key_pressed(sf::Keyboard::Key key, const InputState& input) {
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
    case sf::Keyboard::Key::Up:
      fps += 1;
      break;
    case sf::Keyboard::Key::Down:
      fps -= 1;
      break;
    case sf::Keyboard::Key::F:
      add_frame();
      next_frame();
      break;
    case sf::Keyboard::Key::E:
      next_frame();
      break;
    case sf::Keyboard::Key::Q:
      prev_frame();
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
      handle_mouse_pressed(input);
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

void User::add_frame() {
  // if frame we are adding is first we do special stuff
  if(frames.empty() || frame_index == frames.size()-1) {
    frames.push_back(std::make_shared<Frame>(frame_counter++));
  } else {
    frames.insert(frames.begin() + frame_index + 1, std::make_shared<Frame>(frame_counter++));
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
  if(input.left_mouse_down) {
    if(active_frame->active_point && current_state == State::Normal) {
    } else if(current_state == State::Move && input.mouse_delta != Vec2f{0,0}) {
      if(active_frame->active_point) {
        active_frame->active_point->update_position(input.mouse_position);
        active_frame->active_point->started_moving = true;
      } else if(active_frame->active_curve) {
        active_frame->active_curve->started_moving = true;
        for(const auto &point: active_frame->active_curve->points) {
          if(!point) continue;
          Vec2f new_pos { point->x + input.mouse_delta.x,
                        point->y + input.mouse_delta.y };
          point->update_position(new_pos); 
        }
      }
    }
  }
  // we dropped of a point
  if(!input.left_mouse_down && active_frame->active_point && 
      active_frame->active_point->started_moving) {
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
    std::cout << "-------------\nmoved curve\n" 
      << "id = "<< active_frame->active_curve->get_id() <<std::endl;
    active_frame->active_curve->started_moving = false;
    active_frame->active_curve = nullptr;
  }

  if(current_state == State::PlayAnimation) {
    current_time_between_frames += input.dt;
    std::cout<<"ctbf = "<< current_time_between_frames << std::endl;
    float time_to_switch = 1.0f / (float)fps;
    std::cout <<"tts = " <<time_to_switch <<std::endl;
    if(current_time_between_frames >= time_to_switch) {
      animation_frame_index = (animation_frame_index + 1) % frames.size();
      current_time_between_frames = 0.0;
    }
    std::cout<<"anim f id = " << animation_frame_index << std::endl;
    active_frame = frames[animation_frame_index];
  }

  for(auto& curve: active_frame->curves) {
    curve->update();
  }
}

void User::draw_curve_points(sf::RenderWindow *window) {
  for(auto curve: active_frame->curves) {
    bool active = false;
    if(active_frame->active_curve && 
        curve->get_id() == active_frame->active_curve->get_id())
      active = true;
    curve->draw_points(window, active);
  }
}

void User::draw_convex_hull(sf::RenderWindow *window) {
  for(auto curve: active_frame->curves) {
    curve->draw_convex_hull(window);
  }
}

void User::draw_bezier_curve(sf::RenderWindow *window) {
  for(auto curve: active_frame->curves) {
    curve->draw_bezier_lines(window);
  }
}

void User::draw(sf::RenderWindow *window) {
  //if(current_state != State::MoveCurve)
  draw_curve_points(window);
  //draw_convex_hull(window);
  draw_bezier_curve(window);
}




