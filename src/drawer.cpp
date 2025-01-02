#include "../include/drawer.hpp"

#define RADIUS 10.0f

Drawer::Drawer(sf::RenderWindow& _window):
  window {_window} 
{}

void Drawer::draw_point(const Point &p, bool active) {
  sf::CircleShape cp;
  cp.setFillColor(sf::Color::White);
  cp.setOutlineThickness(2.0f);
  cp.setOutlineColor(sf::Color::Red);
  //float rad = p.get_radius();
  float rad = RADIUS;
  cp.setPosition(p.get_position());
  cp.setOrigin({rad, rad});
  cp.setRadius(rad);
  if(active) 
    cp.setFillColor(sf::Color::Yellow);
  window.draw(cp);
}

void Drawer::draw_control_points(const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active) {
  for(const auto& p: control_points) {
    draw_point(*p, active);
  }
}
