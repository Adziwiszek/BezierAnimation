#include "../include/drawer.hpp"
#include <iostream>

#define RADIUS 10.0f

Drawer::Drawer(sf::RenderWindow& _window):
  window {_window} 
{}

float vec_len(Vec2f vec) {
  return std::sqrt(vec.x*vec.x + vec.y*vec.y);
}

Vec2f normalize(Vec2f vec) {
  float len = vec_len(vec);
  return {vec.x/len, vec.y/len};
}

void Drawer::draw_line_segment(sf::RenderTarget& target, Vec2f start, Vec2f end,
      sf::Color col, float thick) {
  sf::VertexArray va{sf::TriangleStrip, 4};
  sf::CircleShape endpoint_circle;
  float radius = thick/2;
  endpoint_circle.setOrigin(radius, radius);
  endpoint_circle.setRadius(radius);
  endpoint_circle.setFillColor(col);
  //normalizing vector from start to end
  Vec2f se_vec = normalize(end - start);
  Vec2f perp_se_vec = {-se_vec.y, se_vec.x};

  va[0].position = start + perp_se_vec*radius;
  va[1].position = start - perp_se_vec*radius;
  va[3].position = end - perp_se_vec*radius;
  va[2].position = end + perp_se_vec*radius;
  for(int i = 0; i < 4; i++) 
    va[i].color = col;

  //drawing line
  target.draw(va);

  if(col.a != 255) return;
  //drawing first endpoint
  endpoint_circle.setPosition(start);
  target.draw(endpoint_circle);
  //drawing second endpoint
  endpoint_circle.setPosition(end);
  target.draw(endpoint_circle);
}

void Drawer::draw_using_line_segments(
    const std::vector<std::shared_ptr<Point>>& bc_line_points,
    sf::RenderTarget& target, sf::Color col, float thick) {
  if(bc_line_points.size() < 2) return;

  for(size_t i = 0; i < bc_line_points.size() - 1; i++) {
    Vec2f start = bc_line_points[i]->get_position();
    Vec2f end = bc_line_points[i + 1]->get_position();
    draw_line_segment(target, start, end, col, thick);    
  }
}

void Drawer::draw_frame(sf::RenderTarget& target, const std::shared_ptr<Frame>& frame,
    const State& curr_state, sf::Uint8 opacity) {
  for(const auto& curve: frame->curves) {
    sf::Color curve_col = curve->get_color(); 
    curve_col.a = opacity;
    // drawing curve lines
    draw_using_line_segments(curve->get_bc_line_points(), target, curve_col, 
        curve->get_thickness());
    // drawing curve control points
    if(curr_state != State::PlayAnimation && frame->active_curve &&
        frame->active_curve->get_id() == curve->get_id() &&
        !(curve->started_moving || curve->point_moving)) {
      draw_control_points(target, curve->get_control_points(), true, opacity);
    }
  }
}

void Drawer::draw_point(sf::RenderTarget& target, const Point &p, bool active, sf::Uint8 opacity) {
  sf::CircleShape cp;
  auto col = sf::Color::White;
  col.a = opacity;
  cp.setFillColor(col);
  cp.setOutlineThickness(2.0f);
  cp.setOutlineColor(sf::Color::Red);
  //float rad = p.get_radius();
  float rad = RADIUS;
  cp.setPosition(p.get_position());
  cp.setOrigin({rad, rad});
  cp.setRadius(rad);
  if(active) 
    cp.setFillColor(sf::Color::Yellow);
  target.draw(cp);
}

void Drawer::draw_control_points(sf::RenderTarget& target,
  const std::vector<std::shared_ptr<Point>>& control_points, 
      bool active, sf::Uint8 opacity) {
  for(const auto& p: control_points) {
    draw_point(target, *p, active, opacity);
  }
}

/*void Drawer::draw_convex_hull(const std::vector<std::shared_ptr<Point>>& ch_points) {
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
}*/
