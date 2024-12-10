#include "../include/user.hpp"

User::User() {
  //curves.push_back(BCurve()); 
  //active_curve = &curves[0];
  active_curve = nullptr;
}

void User::handle_input(sf::Event event, Vec2f mpos) {
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      lmb_pressed = true;
    }

  }
  if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      active_point = NULL;
      lmb_pressed = false;
    }
  }

  if(event.type == sf::Event::KeyPressed) {
    if(event.key.control && event.key.scancode == sf::Keyboard::Scan::Z && !z_pressed
      && active_curve) {
      active_curve->undo_last_point();
      z_pressed = true;
    }

    if(event.key.scancode == sf::Keyboard::Scan::G 
        && current_state != State::AddCurve) {
      std::cout << "Switched to AddCurve!\n";
      current_state = State::AddCurve;
    }
    if(event.key.scancode == sf::Keyboard::Scan::N
        && current_state != State::Normal) {
      std::cout << "Switched to Normal!\n";
      current_state = State::Normal;
    }
    if(event.key.scancode == sf::Keyboard::Scan::M
        && current_state != State::MoveCurve) {
      std::cout << "Switched to MoveCurve!\n";
      current_state = State::MoveCurve;
    }
    if(event.key.scancode == sf::Keyboard::Scan::H
        && current_state != State::AddPoint) {
      std::cout << "Switched to AddPoint!\n";
      current_state = State::AddPoint;
    }
  }
  if(event.type == sf::Event::KeyReleased) {
    if(event.key.scancode == sf::Keyboard::Scan::Z) {
      z_pressed = false;
    }
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
  lmb_pressed = false;
  //current_state = State::Normal;
}

void User::update(Vec2f mpos, Vec2f delta_mpos) {
  if(lmb_pressed 
      && (current_state == State::Normal
      || current_state == State::MoveCurve)) {
    for(const auto &curve: curves) {
      for(const auto &point: curve->points) {
        if(point->mouse_in(mpos)) {
          std::cout<<"selected a point\n";
          active_point = point;
          active_curve = curves[point->get_parent_id()];
        }
      }
    }

    if(active_point && current_state == State::Normal) {
      active_point->update_position(mpos);
    }
  }

  if(lmb_pressed && current_state == State::AddPoint) {
    add_point_to_current_curve(mpos);
  }

  if(lmb_pressed && current_state == State::MoveCurve 
      && active_curve && active_curve->points.size() > 0) {
    for(const auto &point: active_curve->points) {
      if(!point) continue;
      Vec2f new_pos { point->x + delta_mpos.x,
                      point->y + delta_mpos.y }; 
      point->update_position(new_pos); 
    }
  }

  if(current_state == State::AddCurve && lmb_pressed) {
    add_new_curve(mpos);   
  }

  for(auto& curve: curves) {
    curve->update();
  }
}

void User::draw_curve_points(sf::RenderWindow *window) {
  for(auto curve: curves) {
    bool active = false;
    if(curve->get_id() == active_curve->get_id())
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
  draw_curve_points(window);
  //draw_convex_hull(window);
  draw_bezier_curve(window);
}




