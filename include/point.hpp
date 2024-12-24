#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

class Point {
protected:
  Vec2f position;
  float radius;
  unsigned parent_id;
  unsigned id;

public:
  bool started_moving = false;
  const float& x = position.x;
  const float& y = position.y;

  Point(Vec2f, unsigned, unsigned);
  Point(const Point&);

  Point operator*(float const) const;
  Point operator+(Point const&) const;

  unsigned get_parent_id() const;
  unsigned get_id() const;
  Vec2f get_position() const;
  float get_radius() const;

  float dist_from_point(Vec2f pos) const;  
  float dist_from_point(const Point& other) const;  
  bool mouse_in(Vec2f mpos) const;
  void update_position(Vec2f pos);
};
