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
  vector<std::shared_ptr<Point>> points;
  BCurve(unsigned id);
  BCurve(Vec2f pos, unsigned id);

  unsigned get_id();
  unsigned get_size();
  /*
   * adds a new reference point
   * */
  void spawn_point(Vec2f pos);
  /*
   * removes last added point
   * */
  void undo_last_point(); 
  /*
   * returns reference point with a given index
   * */
  std::shared_ptr<Point> get_point(size_t index); 
  /*
   * returns how many reference points there are
   * */
  size_t points_count() const;
  /*
   * deCasteljau algorithm for computing a point on this Bezier curve
   * */
  std::shared_ptr<Point> deCasteljau(float t); 
  /*
   * returns index of a point that the mouse is currently on top of
   * if mouse is not on top of any point it returns SIZE_MAX
   * */
  size_t mouse_over_point(Vec2f mpos); 

  bool mouse_in(Vec2f);
  bool mouse_in_control_point(Vec2f);
  // working on this
  Point horner_point(float t); 
  /*
   * generates the actual points on the Bezier curve
   * they are used only for drawing
   * */
  vector<std::shared_ptr<Point>> generate_curve_points(int n);
  /*
   * updates state of this curve, calcurtlates convex hull
   * */
  void update(); 

  /*
   * methods for drawing
   * */
  void draw_points(sf::RenderWindow *window, bool active);
  void draw_convex_hull(sf::RenderWindow *window); 
  void draw_bezier_lines(sf::RenderWindow *window); 

  /*
   * Used for calculating convex hull
   * */
  std::vector<std::shared_ptr<Point>> 
    graham_scan(std::vector<std::shared_ptr<Point>> points);
};

