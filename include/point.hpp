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
  unsigned parent_id;
  unsigned id;

public:
  float x; 
  float y;

  /**
   * <Constructor for Point class>
   *
   * @param _position initial position of a point
   *
   * */
  Point(Vec2f, unsigned, unsigned);
  Point(const Point&);

  Point operator*(float const);
  Point operator+(Point const&);

  unsigned get_parent_id();
  unsigned get_id();
  void set_color(sf::Color);

  /**
   * <computes distance from given position to this point>
   *
   * @param pos position that we compute distance from
   * */
  float dist_from_point(Vec2f pos);  
  bool mouse_in(Vec2f mpos);
  void update_position(Vec2f pos);

  void draw(sf::RenderWindow *window);
};
