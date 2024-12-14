#include "../include/user.hpp"

User::User() {
  //curves.push_back(BCurve()); 
  //active_curve = &curves[0];
  active_curve = nullptr;
}

void User::handle_mouse_pressed(const InputState& input) {
  if (!input.left_mouse_down) return;

  if (current_state == State::AddCurve) {
    add_new_curve(input.mouse_position);
  } else if (current_state == State::AddPoint && active_curve) {
    add_point_to_current_curve(input.mouse_position);
  } else if (current_state == State::Normal) {
    auto [curve, point] = get_active_point_curve(input.mouse_position);
    active_curve = curve;
    active_point = point;
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
      switch_to_state(State::MoveCurve, "MoveCurve");
      break;
    case sf::Keyboard::Key::H:
      switch_to_state(State::AddPoint, "AddPoint");
      break;
    case sf::Keyboard::Key::Z:
      if(active_curve && input.ctrl_pressed) {
        active_curve->undo_last_point();
      }
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
      active_point = nullptr;
      input.left_mouse_down = false;
    }
  }

  /*if (event.type == sf::Event::MouseMoved) {
    Vec2f new_pos = {(float)event.mouseMove.x, (float)event.mouseMove.y};
    Vec2f new_delta = new_pos - input.mouse_position;
    if(new_delta == input.mouse_delta) {
      input.mouse_delta = {0.0f, 0.0f};
    } else {
      input.mouse_delta = new_delta;
      input.mouse_position = new_pos;
    }
  }*/

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

std::tuple<std::shared_ptr<BCurve>, std::shared_ptr<Point>> 
  User::get_active_point_curve(Vec2f mpos) {
  for(const auto &curve: curves) {
    for(const auto &point: curve->points) {
      if(point->mouse_in(mpos)) {
        return {curve, point};
      }
    }
  }
  return {nullptr, nullptr};
}

void User::switch_to_state(State new_state, const std::string& state_name) {
  if (current_state != new_state) {
    std::cout << "Switched to " << state_name << "!\n";
    current_state = new_state;
  }
}

void User::add_new_curve(Vec2f pos) {
  unsigned new_id = curves.size();
  curves.push_back(std::make_shared<BCurve>(pos, new_id));
  active_curve = curves[new_id];
  current_state = State::Normal;
}

void User::add_point_to_current_curve(Vec2f pos) {
  active_curve->spawn_point(pos);
  active_point = (active_curve->get_point(active_curve->points_count() - 1));
}

void User::update(const InputState& input) {
  if(input.left_mouse_down) {
    if(active_point && current_state == State::Normal) {
      active_point->update_position(input.mouse_position);
    } else if(active_curve && current_state == State::MoveCurve) {
      for(const auto &point: active_curve->points) {
        if(!point) continue;
        Vec2f new_pos { point->x + input.mouse_delta.x,
                      point->y + input.mouse_delta.y };
        point->update_position(new_pos); 
      }
    }
  }

  for(auto& curve: curves) {
    curve->update();
  }
}

void User::draw_curve_points(sf::RenderWindow *window) {
  for(auto curve: curves) {
    bool active = false;
    if(active_curve && curve->get_id() == active_curve->get_id())
      active = true;
    curve->draw_points(window, active);
  }
}

void User::draw_convex_hull(sf::RenderWindow *window) {
  for(auto curve: curves) {
    curve->draw_convex_hull(window);
  }
}

void User::draw_bezier_curve(sf::RenderWindow *window) {
  for(auto curve: curves) {
    curve->draw_bezier_lines(window);
  }
}

void User::draw(sf::RenderWindow *window) {
  //if(current_state != State::MoveCurve)
  draw_curve_points(window);
  //draw_convex_hull(window);
  draw_bezier_curve(window);
}




