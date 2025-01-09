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

void draw_using_quads(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness, sf::RenderWindow& window) {
  sf::VertexArray quad(sf::Quads);

  for (size_t i = 0; i < bc_line_points.size() - 1; ++i) {
    Vec2f p1 = bc_line_points[i]->get_position();
    Vec2f p2 = bc_line_points[i + 1]->get_position();

    Vec2f direction = p2 - p1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    Vec2f unitDir = direction / length;

    // Perpendicular vector for thickness
    Vec2f perp(-unitDir.y, unitDir.x);
    Vec2f offset = perp * (thickness / 2.0f);

    // Define the four corners of the thick line segment
    quad.append(sf::Vertex(p1 - offset, color));
    quad.append(sf::Vertex(p2 - offset, color));
    quad.append(sf::Vertex(p2 + offset, color));
    quad.append(sf::Vertex(p1 + offset, color));
  }
  window.draw(quad);
}


void draw_using_lines(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness, sf::RenderWindow& window) {
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

void draw_using_circles(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness, sf::RenderWindow& window) {

  sf::RenderTexture render_texture;
  render_texture.create(window.getSize().x, window.getSize().y);
  render_texture.clear(sf::Color::Transparent);

  for (size_t i = 0; i < bc_line_points.size() - 1; ++i) {
    Vec2f p1 = bc_line_points[i]->get_position();
    Vec2f p2 = bc_line_points[i + 1]->get_position();

    float t = 0.0f;
    while (t <= 1.0f) {
      Vec2f point = (1 - t) * p1 + t * p2;
      sf::CircleShape circle(thickness / 2.0f);
      circle.setFillColor(color);
      circle.setPosition(point - Vec2f(thickness / 2.0f, thickness / 2.0f));
      render_texture.draw(circle, sf::BlendAlpha);
      t += 0.1f;  // Adjust step size for smoother curves
    }
  }
  render_texture.display();

  sf::Sprite sprite(render_texture.getTexture());
  window.draw(sprite, sf::BlendAlpha);
}

void draw_using_va(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness, sf::RenderWindow& window) {

    sf::VertexArray quads(sf::Quads);

  for (size_t i = 0; i < bc_line_points.size() - 1; ++i) {
    Vec2f p1 = bc_line_points[i]->get_position();
    Vec2f p2 = bc_line_points[i + 1]->get_position();

    // Calculate direction vector and perpendicular offset
    Vec2f direction = p2 - p1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    Vec2f unit_direction = direction / length;
    Vec2f perpendicular(-unit_direction.y, unit_direction.x);

    Vec2f offset = perpendicular * (thickness / 2.0f);

    // Create the quad for this segment
    quads.append(sf::Vertex(p1 - offset, color));
    quads.append(sf::Vertex(p1 + offset, color));
    quads.append(sf::Vertex(p2 + offset, color));
    quads.append(sf::Vertex(p2 - offset, color));
  }

  window.draw(quads);
}

void Drawer::draw_bc_lines(const std::vector<std::shared_ptr<Point>>& bc_line_points,
      sf::Color color, float thickness, sf::Uint8 opacity) {
  if(bc_line_points.size() < 2)
    return;
  //color.a = 255*static_cast<sf::Uint8>(std::clamp(opacity, 0.0f, 1.0f));
  color.a = opacity;
  //draw_using_circles(bc_line_points, color, thickness, window);
  draw_using_circles(bc_line_points, color, thickness, window);
}

void Drawer::draw_bc_lines_for_background(
    const std::vector<std::shared_ptr<Point>>& bc_line_points,
    sf::RenderTexture& render_texture,
    sf::Color color, float thickness, sf::Uint8 opacity
      ) {
  if(bc_line_points.size() < 2)
    return;

  for (size_t i = 0; i < bc_line_points.size() - 1; ++i) {
    Vec2f p1 = bc_line_points[i]->get_position();
    Vec2f p2 = bc_line_points[i + 1]->get_position();

    float t = 0.0f;
    while (t <= 1.0f) {
      Vec2f point = (1 - t) * p1 + t * p2;
      sf::CircleShape circle(thickness / 2.0f);
      circle.setFillColor(color);
      circle.setPosition(point - Vec2f(thickness / 2.0f, thickness / 2.0f));
      render_texture.draw(circle, sf::BlendAlpha);
      t += 0.1f;  // Adjust step size for smoother curves
    }
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
