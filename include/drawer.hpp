#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "point.hpp"

class Drawer {
  sf::RenderWindow &window;

public:
  Drawer(sf::RenderWindow& _window);

  void draw_point(const Point &p, bool active=false);
  void draw_control_points(const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active=false);
};
