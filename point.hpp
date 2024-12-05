#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

class Point {
protected:
  sf::CircleShape representation;
  Vec2f position;
  float radius;

public:
  float x;
  float y;

  Point(Vec2f _position);

  Point operator*(float const t);
  Point operator+(Point const& p);
  // linear interpolation
  static Point lerp(const Point& p1, const Point& p2, float t);

  float dist_from_point(Vec2f pos);  
  bool mouse_in(Vec2f mpos);
  void update_position(Vec2f pos);

  void draw(sf::RenderWindow *window);
};
