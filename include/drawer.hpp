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

  void draw_line_segment(sf::RenderTarget& target, Vec2f start, Vec2f end,
      sf::Color col=sf::Color::Green, float thick=20.0);
  void draw_using_line_segments(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::RenderTarget& target, sf::Color col=sf::Color::Green, float thick=20.0);
  void draw_frame(sf::RenderTarget& target, const std::shared_ptr<Frame>& frame,
      const State& curr_state, sf::Uint8 opacity=255);

  void draw_point(sf::RenderTarget& target, const Point &p, bool active=false);
  void draw_control_points(sf::RenderTarget& target, 
      const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active=false);
  void draw_bc_lines(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color=sf::Color::Green, float thickness=1.0, sf::Uint8 opacity=255);
  void draw_bc_lines_for_background(
      const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::RenderTexture& render_texture,
      sf::Color color=sf::Color::Green, float thickness=1.0, sf::Uint8 opacity=255);
  void draw_convex_hull(const std::vector<std::shared_ptr<Point>>& ch_points);
};
