#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "point.hpp"

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;
using Points = std::vector<std::shared_ptr<Point>>;

class BCurve {
private: 
  unsigned id;                  // unique id of this curve
  unsigned point_counter {0};   // count of how many points have been placed

  float thickness{3.0f};
  sf::Color color{sf::Color::Green};

  vector<std::shared_ptr<Point>> convex_hull;
  vector<std::shared_ptr<Point>> bc_points;
  vector<std::shared_ptr<Point>> points;

  std::vector<std::shared_ptr<Point>> 
    graham_scan(std::vector<std::shared_ptr<Point>> points);
public:
  bool started_moving {false};

  BCurve(unsigned id, float thick, sf::Color col);            // basic constructor
  BCurve(Vec2f pos, unsigned id, float thick, sf::Color col); // immediately spawns point
  BCurve(const BCurve& other, unsigned id);                   // copy constructor

  unsigned get_id();
  size_t get_points_count() const;
  size_t get_bc_points_count() const;
  float get_thickness() const;
  sf::Color get_color() const;
  const Points& get_convex_hull_points() const; 
  const Points& get_control_points() const;
  const Points& get_bc_line_points() const;

  void set_thickness(float);
  void set_color(sf::Color);

  void spawn_point(Vec2f pos);
  void delete_point_by_id(unsigned id);
  void undo_last_point(); 

  std::shared_ptr<Point> get_point(size_t index); 
  size_t mouse_over_point(Vec2f mpos); 

  bool mouse_in_curve(Vec2f);
  bool mouse_in_control_point(Vec2f);

  Vec2f linear_bezier_eval(unsigned int n, float t);
  vector<std::shared_ptr<Point>> generate_curve_points(int n);
  void update(); 
};

