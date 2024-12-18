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
  void add_curve(Vec2f);
  void add_point_to_current_curve(Vec2f);
  std::shared_ptr<Point> get_active_point(Vec2f);
  std::shared_ptr<BCurve> get_active_curve(Vec2f);
};
