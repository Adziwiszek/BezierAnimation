#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "point.hpp"

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

class BCurve {
private:
  unsigned id;
  unsigned point_counter {0};

  vector<std::shared_ptr<Point>> convex_hull;
  vector<std::shared_ptr<Point>> bezier_points;
public:
  bool started_moving {false};
  vector<std::shared_ptr<Point>> points;

  BCurve(unsigned id);
  BCurve(Vec2f pos, unsigned id);
  BCurve(const BCurve& other, unsigned id);

  unsigned get_id();
  unsigned get_size();
  void spawn_point(Vec2f pos);
  void undo_last_point(); 
  std::shared_ptr<Point> get_point(size_t index); 
  size_t points_count() const;
  std::shared_ptr<Point> deCasteljau(float t); 
  size_t mouse_over_point(Vec2f mpos); 

  bool mouse_in(Vec2f);
  bool mouse_in_control_point(Vec2f);
  // working on this
  Point horner_point(float t); 
  vector<std::shared_ptr<Point>> generate_curve_points(int n);
  void update(); 

  void draw_points(sf::RenderWindow *window, bool active);
  void draw_convex_hull(sf::RenderWindow *window); 
  void draw_bezier_lines(sf::RenderWindow *window); 

  std::vector<std::shared_ptr<Point>> 
    graham_scan(std::vector<std::shared_ptr<Point>> points);
};

