#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "point.hpp"

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

class BCurve {
public:
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
  Point& get_point(size_t index); 
  /*
   * returns how many reference points there are
   * */
  size_t points_count() const;
  /*
   * deCasteljau algorithm for computing a point on this Bezier curve
   * */
  /*
   * returns index of a point that the mouse is currently on top of
   * if mouse is not on top of any point it returns SIZE_MAX
   * */
  size_t mouse_over_point(Vec2f mpos); 
  Point deCasteljau(float t); 
  // working on this
  Point horner_point(float t); 
  /*
   * generates the actual points on the Bezier curve
   * they are used only for drawing
   * */
  vector<Point> generate_curve_points(int n);
  /*
   * updates state of this curve, calculates convex hull
   * */
  void update(); 

  /*
   * methods for drawing
   * */
  void draw_points(sf::RenderWindow *window);
  void draw_convex_hull(sf::RenderWindow *window); 
  void draw_bezier_lines(sf::RenderWindow *window); 

  std::vector<Point> graham_scan(std::vector<Point> points);
private:
  vector<Point> points;
  vector<Point> convex_hull;
  vector<Point> bezier_points;
};

