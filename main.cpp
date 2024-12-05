#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "point.hpp"

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

// declarations
class Point;
std::vector<Point> graham_scan(std::vector<Point> points);
float bernstein_poly(int n, int k, float x);
Point bezier_point(float t, vector<Point> const &chull);


class BCurve {
public:
  void spawn_point(Vec2f pos) {
    points.push_back(Point(pos));
  }

  // De Casteljau algorithm for Bézier curve point calculation
  static Point deCasteljau(std::vector<Point> controlPoints, float t) {
    // Create a copy of control points to work with
    std::vector<Point> points = controlPoints;
    for (size_t r = 1; r < controlPoints.size(); ++r) {
      for (size_t i = 0; i < controlPoints.size() - r; ++i) {
        // Linear interpolation between adjacent points
        points[i] = points[i] * (1 - t) + points[i + 1] * t;
      }
    }

    // The final point is the point on the Bézier curve
    return points[0];
  }

/*function eval_horner(t) { 
  u = 1 - t; 
  bc = 1; 
  tn = 1; 
  tmp = controlpoints[0] * u; 
  for(int i = 1; i < n; i++) { 
    tn *= t; 
    bc *= (n-i+1)/i; 
    tmp = (tmp + tn * bc * controlpoints) * u; 
  } 
  return (tmp + tn * t * controlpoints[n]); 
}*/
  Point horner_point(float t) {
    float u { 1 - t };
    float bc { 1 };
    float tn { 1 };
    Point tmp = points[0];
    int n = points.size();
    for(int i = 1; i < n; i++) {
      tn *= t;
      bc *= ((float)(n - i + 1))/i;
      tmp = (tmp + points[i] * tn * t) * u;
    }
    return tmp + points[n - 1] * t * tn;
  }

  void draw_points(sf::RenderWindow *window) {
    for(auto p: points) {
      p.draw(window);
    }
  }

  Point& get_point(size_t index) {
    // TODO: handle wrong index
    // if(index >= points.size()) 
    return points[index];
  }

  size_t points_count() const {
    return points.size();
  }

  void draw_convex_hull(sf::RenderWindow *window) {
    // If convex hull is to small we dont draw it
    if(convex_hull.size() < 3) 
      return;

    for (size_t i = 0; i < convex_hull.size(); ++i) {
      // linia od od itego do i+1 wierzcholka otoczki wypuklej
      sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(convex_hull[i].x, convex_hull[i].y), sf::Color::Blue),
        sf::Vertex(sf::Vector2f(convex_hull[(i+1) % convex_hull.size()].x,
              convex_hull[(i+1) % convex_hull.size()].y), sf::Color::Blue)
      };
      window->draw(line, 2, sf::Lines);
    }
  }

  void draw_bezier_lines(sf::RenderWindow *window) {
    if(convex_hull.size() < 3) 
      return;

    for (size_t i = 0; i < bezier_points.size() - 1; ++i) {
      //std::cout << "i = " << i << std::endl;
      //std::cout << bezier_points[i].x << " " << bezier_points[i].y << std::endl;
      // linie do zrobienia iluzji krzywej beziera
      sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(bezier_points[i].x, bezier_points[i].y), sf::Color::Green),
        sf::Vertex(sf::Vector2f(bezier_points[i + 1].x, bezier_points[i + 1].y), sf::Color::Green)
      };
      window->draw(line, 2, sf::Lines);
    }
  }

  vector<Point> generate_curve_points(int n) {
    vector<Point> res;
    if(points.size() < 3)
      return res;

    for(int i = 0; i <= n; i++) {
      //std::cout << i << std::endl;
      float t = static_cast<float>(i) / static_cast<float>(n);
      res.push_back(deCasteljau(points, t));
      //res.push_back(horner_point(t));
    }
    return res;
  }

  size_t mouse_over_point(Vec2f mpos) {
    for(size_t i = 0; i < points.size(); i++){ 
      if(points[i].mouse_in(mpos))
        return i;
    }
    return SIZE_MAX;
  }

  void update() {
    convex_hull = graham_scan(points);
    int curve_points = 20 + (int)(points.size() / 2)*7;
    bezier_points = generate_curve_points(curve_points);
  }

  void undo_last_point() {
    if(points.size() > 0) {
      points.pop_back();
    }
  }

private:
  vector<Point> points;
  vector<Point> convex_hull;
  vector<Point> bezier_points;
};

class User {
private:
  BCurve* active_curve;
  Point* active_point;
  vector<BCurve> curves;

  bool lmb_pressed { false };
  bool z_pressed { false };

public:
  User() {
    curves.push_back(BCurve()); 
    active_curve = &curves[0];
  }

  void handle_input(sf::Event event, Vec2f mpos) {
    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        size_t point_index = active_curve->mouse_over_point(mpos);

        if (point_index != SIZE_MAX) {
          active_point = &(active_curve->get_point(point_index));
        } else if (!lmb_pressed) {
          // adds a new point and sets it as an active point
          active_curve->spawn_point(mpos);
          active_point = &(active_curve->get_point(active_curve->points_count() - 1));
        }
        /*// we check if we found an active point
        if(!lmb_pressed && active_point == NULL) {
          active_curve->spawn_point(mpos);
        }*/
        lmb_pressed = true;
      }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        active_point = NULL;
        lmb_pressed = false;
      }
    }

    if(event.type == sf::Event::KeyPressed) {
      if(event.key.control && event.key.scancode == sf::Keyboard::Scan::Z && !z_pressed) {
        active_curve->undo_last_point();
        z_pressed = true;
      }
    }
    if(event.type == sf::Event::KeyReleased) {
      if(event.key.scancode == sf::Keyboard::Scan::Z) {
        z_pressed = false;
      }
    }
  }

  void update(Vec2f mpos) {
    if(lmb_pressed) {
      if(active_point != NULL) {
        active_point->update_position(mpos);
      }
    }

    for(auto& curve: curves) {
      curve.update();
    }
  }

  void draw_curve_points(sf::RenderWindow *window) {
    for(auto curve: curves) {
      curve.draw_points(window);
    }
  }

  void draw_convex_hull(sf::RenderWindow *window) {
    for(auto curve: curves) {
      curve.draw_convex_hull(window);
    }
  }

  void draw_bezier_curve(sf::RenderWindow *window) {
    for(auto curve: curves) {
      curve.draw_bezier_lines(window);
    }
  }
  
  void draw(sf::RenderWindow *window) {
    draw_curve_points(window);
    draw_convex_hull(window);
    draw_bezier_curve(window);
  }
};

float orientation(const Point& p, const Point& q, const Point& r) {
    return (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
}

float distance(const Point& p1, const Point& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

std::vector<Point> graham_scan(std::vector<Point> points) {
    //  Jeśli mamy mniej niż 3 punkty, nie możemy utworzyć otoczki
    if (points.size() < 3) return points;

    //  Znajdź punkt o najmniejszej współrzędnej y 
    // (w przypadku równych y - punkt o najmniejszym x)
    auto minYPoint = std::min_element(points.begin(), points.end(), 
        [](const Point& a, const Point& b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
        });

    //  Przesuń punkt o najmniejszej współrzędnej y na początek wektora
    std::swap(*minYPoint, points[0]);
    Point& pivot = points[0];

    //  Sortuj punkty względem kąta z punktem bazowym
    std::sort(points.begin() + 1, points.end(), 
        [&pivot](const Point& a, const Point& b) {
            float orientationVal = orientation(pivot, a, b);
            
            // Jeśli punkty mają ten sam kąt, wybierz bliższy
            if (orientationVal == 0) {
                return distance(pivot, a) < distance(pivot, b);
            }
            
            // Sortuj względem kąta (przeciwnie do ruchu wskazówek zegara)
            return orientationVal > 0;
        });

    std::vector<Point> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);

    for (size_t i = 2; i < points.size(); ++i) {
        // Usuwaj punkty z otoczki, które tworzą skręt w prawo
        while (hull.size() > 1) {
            Point top = hull.back();
            hull.pop_back();
            
            if (orientation(hull.back(), top, points[i]) > 0) {
                hull.push_back(top);
                break;
            }
        }
        hull.push_back(points[i]);
    }

    //std::cout << "hull size = " << hull.size() << std::endl;
    return hull;
}

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  User user {};

  Vec2f mpos { 0.0f, 0.0f };
  while (window.isOpen())
  {
    mpos = { static_cast<float>(sf::Mouse::getPosition(window).x), 
             static_cast<float>(sf::Mouse::getPosition(window).y)};

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // user input
      user.handle_input(event, mpos);
    }

    // update users points
    user.update(mpos);

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.display();
  }

  return 0;
}


