#include "../include/user.hpp"

User::User() {
  curves.push_back(BCurve()); 
  active_curve = &curves[0];
}

void User::handle_input(sf::Event event, Vec2f mpos) {
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      size_t point_index = active_curve->mouse_over_point(mpos);

      if (point_index != SIZE_MAX) {
        active_point = &(active_curve->get_point(point_index));
      } else if (!lmb_pressed) {
        // adds a new point and sets it as an active point
        active_curve->spawn_point(mpos);
        active_point = &(active_curve->get_point(active_curve->points_count() - 1));
      }
      /*// we check if we found an active point
        if(!lmb_pressed && active_point == NULL) {
        active_curve->spawn_point(mpos);
        }*/
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
    if(event.key.control && event.key.scancode == sf::Keyboard::Scan::Z && !z_pressed) {
      active_curve->undo_last_point();
      z_pressed = true;
    }
  }
  if(event.type == sf::Event::KeyReleased) {
    if(event.key.scancode == sf::Keyboard::Scan::Z) {
      z_pressed = false;
    }
  }
}

void User::update(Vec2f mpos) {
  if(lmb_pressed) {
    if(active_point != NULL) {
      active_point->update_position(mpos);
    }
  }

  for(auto& curve: curves) {
    curve.update();
  }
}

void User::draw_curve_points(sf::RenderWindow *window) {
  for(auto curve: curves) {
    curve.draw_points(window);
  }
}

void User::draw_convex_hull(sf::RenderWindow *window) {
  for(auto curve: curves) {
    curve.draw_convex_hull(window);
  }
}

void User::draw_bezier_curve(sf::RenderWindow *window) {
  for(auto curve: curves) {
    curve.draw_bezier_lines(window);
  }
}

void User::draw(sf::RenderWindow *window) {
  draw_curve_points(window);
  draw_convex_hull(window);
  draw_bezier_curve(window);
}




