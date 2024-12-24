#include "../include/point.hpp"

constexpr float RADIUS = 10.0f;

Point::Point(Vec2f _position, unsigned _p_id, unsigned _id)
  : position { _position }, parent_id { _p_id }, id { _id }
{
  radius = RADIUS;
}

Point::Point(const Point &other)
  : radius { other.radius }, position { other.position }, 
  parent_id { other.parent_id }, id { other.id }
{
}

Point Point::operator*(float const scalar) const {
  return Point( { position.x * scalar, position.y * scalar}, parent_id, id );
}

Point Point::operator+(Point const& other) const {
  return Point( {position.x + other.x, 
                 position.y + other.y},
                 parent_id, id );
}

unsigned Point::get_parent_id() const {
  return parent_id;
}

unsigned Point::get_id() const {
  return id;
}

Vec2f Point::get_position() const {
  return position;
}

float Point::get_radius() const {
  return radius;
}

float Point::dist_from_point(Vec2f other) const {
  return std::sqrt(std::pow(position.x - other.x, 2) + 
                  std::pow(position.y - other.y, 2));
}

float Point::dist_from_point(const Point& other) const {
  return dist_from_point(other.get_position());
}

bool Point::mouse_in(Vec2f mpos) const {
  return dist_from_point(mpos) < radius;
}

void Point::update_position(Vec2f pos) {
  position = pos;
}

