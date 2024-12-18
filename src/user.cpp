#include "../include/user.hpp"

User::User() {
  //curves.push_back(BCurve()); 
  //active_curve = &curves[0];
  //active_curve = nullptr;
  add_frame();
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

void User::add_frame() {
  frames.push_back(std::make_shared<Frame>(frame_counter++));
  if(frames.size() == 1) {
    f_iter = frames.begin();
    active_frame = *f_iter;
  } else {
    next_frame();
  }
}

void User::next_frame() {
  f_iter++;
  // we are at the end
  if(f_iter >= frames.begin()) {
    f_iter--; 
  } else {
    active_frame = *f_iter;
  }
}

void User::prev_frame() {
  f_iter--;
  // we are at the beggining
  if(f_iter <= frames.begin()) {
    f_iter++; 
  } else {
    active_frame = *f_iter;
  }
}

void User::switch_to_state(State new_state, const std::string& state_name) {
  if (current_state != new_state) {
    std::cout << "Switched to " << state_name << "!\n";
    current_state = new_state;
  }
}

void User::add_new_curve(Vec2f pos) {
  if(active_frame) {
    active_frame->add_curve(pos);
    current_state = State::Normal;
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




