#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "point.hpp"
#include "bcurve.hpp"

class User {
private:
  enum State {
    Normal, 
    MoveCurve,
    AddCurve,
    AddPoint
  };

  std::shared_ptr<BCurve> active_curve;
  std::shared_ptr<Point> active_point;
  vector<std::shared_ptr<BCurve>> curves;

  State current_state { Normal };

  bool lmb_pressed { false };
  bool z_pressed { false };


public:
  User(); 
  void handle_input(sf::Event event, Vec2f mpos); 
  void update(Vec2f mpos); 

  void add_new_curve(Vec2f pos);

  void draw_curve_points(sf::RenderWindow *window); 
  void draw_convex_hull(sf::RenderWindow *window);
  void draw_bezier_curve(sf::RenderWindow *window);
  void draw(sf::RenderWindow *window);
};
