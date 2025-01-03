#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "point.hpp"

class Drawer {
  sf::RenderWindow &window;

public:
  Drawer(sf::RenderWindow& _window);

  void draw_point(const Point &p, bool active=false);
  void draw_control_points(const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active=false);
  void draw_bc_lines(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color=sf::Color::Green, float thickness=1.0);
  void draw_convex_hull(const std::vector<std::shared_ptr<Point>>& ch_points);
};
