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
  BCurve* active_curve;
  Point* active_point;
  vector<BCurve> curves;

  bool lmb_pressed { false };
  bool z_pressed { false };

public:
  User(); 
  void handle_input(sf::Event event, Vec2f mpos); 

  void update(Vec2f mpos); 
  void draw_curve_points(sf::RenderWindow *window); 
  void draw_convex_hull(sf::RenderWindow *window);
  void draw_bezier_curve(sf::RenderWindow *window);
  void draw(sf::RenderWindow *window);
};
