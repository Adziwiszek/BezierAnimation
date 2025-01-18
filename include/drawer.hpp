#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "point.hpp"
#include "frame.hpp"
#include "userUtils.hpp"

class Drawer {
  sf::RenderWindow &window;

public:
  Drawer(sf::RenderWindow& _window);

  static void draw_line_segment(sf::RenderTarget& target, Vec2f start, Vec2f end,
      sf::Color col=sf::Color::Green, float thick=20.0);
  static void draw_using_line_segments(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::RenderTarget& target, sf::Color col=sf::Color::Green, float thick=20.0);
  static void draw_frame(sf::RenderTarget& target, const std::shared_ptr<Frame>& frame,
      const State& curr_state, sf::Uint8 opacity=255);

  static void draw_point(sf::RenderTarget& target, const Point &p, bool active=false, sf::Uint8 opacity=255);
  static void draw_control_points(sf::RenderTarget& target, 
      const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active=false, sf::Uint8 opacity=255);
};
