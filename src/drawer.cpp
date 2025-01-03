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

void Drawer::draw_bc_lines(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness) {
  if(bc_line_points.empty())
    return;

  for (size_t i = 0; i < bc_line_points.size() - 1; ++i) {
    Point p1(*bc_line_points[i]);
    Point p2(*bc_line_points[i+1]);
    Vec2f direction(p2.get_position() - p1.get_position());
    float length = std::sqrt(direction.x*direction.x + direction.y*direction.y);

    sf::RectangleShape line(Vec2f(length, thickness));
    line.setFillColor(color);
    line.setPosition(p1.get_position());
    line.setRotation(std::atan2(direction.y, direction.x) * 180.f / M_PI);
    window.draw(line);
  }
}

void Drawer::draw_convex_hull(const std::vector<std::shared_ptr<Point>>& ch_points) {
  if(ch_points.empty())
    return;

  for (size_t i = 0; i < ch_points.size(); ++i) {
    // linia od od itego do i+1 wierzcholka otoczki wypuklej
    sf::Vertex line[] = {
      sf::Vertex(sf::Vector2f(ch_points[i]->x, ch_points[i]->y), sf::Color::Blue),
      sf::Vertex(sf::Vector2f(ch_points[(i+1) % ch_points.size()]->x,
            ch_points[(i+1) % ch_points.size()]->y), sf::Color::Blue)
    };
    window.draw(line, 2, sf::Lines);
  }
}
