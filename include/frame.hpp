#pragma once

#include <SFML/Graphics.hpp>
#include "bcurve.hpp"
#include <vector>
#include <memory>

using Curves = std::vector<std::shared_ptr<BCurve>>;

class Frame {
private:
  unsigned id;
  unsigned curve_counter {0};
public:
  Curves curves;
  std::shared_ptr<BCurve> active_curve;
  std::shared_ptr<Point> active_point;

  Frame(unsigned);
  Frame(const Frame&, unsigned);
  Frame(const std::shared_ptr<Frame>&, unsigned);

  unsigned get_id();

  void add_curve(Vec2f, float, sf::Color);
  void add_point_to_current_curve(Vec2f);
  void delete_curve_by_id(unsigned);
  void delete_point_from_current_curve(unsigned);
  std::shared_ptr<Point> get_active_point(Vec2f);
  std::shared_ptr<BCurve> get_active_curve(Vec2f);
};

using Frames = std::vector<std::shared_ptr<Frame>>;
