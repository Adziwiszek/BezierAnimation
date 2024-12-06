#include "../include/point.hpp"

Point::Point(Vec2f _position)
  : position { _position }, x { _position.x }, y { _position.y }
{
  radius = 10.0f;
  representation = sf::CircleShape(radius);
  representation.setOrigin({10.0f, 10.0f});
  representation.setPosition(position);
  //representation.setFillColor(sf::Color::Blue);
  representation.setOutlineColor(sf::Color::Red);
  representation.setOutlineThickness(1.0f);
}

Point Point::operator*(float const scalar) {
  return Point( { x * scalar, y * scalar} );
}

Point Point::operator+(Point const& other) {
  return Point( {x + other.x, y + other.y} );
}

float Point::dist_from_point(Vec2f pos) {
  return std::sqrt(std::pow(x - pos.x, 2) + std::pow(y - pos.y, 2));
}

bool Point::mouse_in(Vec2f mpos) {
  if(dist_from_point(mpos) < radius) {
    return true;
  }
  return false;
}

void Point::update_position(Vec2f pos) {
  position = pos;
  x = pos.x;
  y = pos.y;
}

void Point::draw(sf::RenderWindow *window) {
  representation.setPosition(position);
  window->draw(representation);
}

