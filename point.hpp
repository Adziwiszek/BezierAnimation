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

  /**
   * <Constructor for Point class>
   *
   * @param _position initial position of a point
   *
   * */
  Point(Vec2f _position);

  Point operator*(float const);
  Point operator+(Point const&);

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
