#include "../include/bcurve.hpp"

BCurve::BCurve(unsigned id) : id { id } {
}

BCurve::BCurve(Vec2f pos, unsigned id) : id { id } {
  spawn_point(pos);
}

float orientation(const std::shared_ptr<Point>& p, const std::shared_ptr<Point>& q, const std::shared_ptr<Point>& r) {
    return (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
}

float distance(const std::shared_ptr<Point>& p1, const std::shared_ptr<Point>& p2) {
    return std::sqrt(std::pow(p1->x - p2->x, 2) + std::pow(p1->y - p2->y, 2));
}

void BCurve::spawn_point(Vec2f pos) {
  std::cout<<"asdasd\n";
  unsigned new_id = points.size();
  std::cout<<"================\n";
  points.push_back(std::make_shared<Point>(pos, id, new_id));
}

void BCurve::undo_last_point() {
  if(points.size() > 0) {
    points.pop_back();
  }
}

unsigned BCurve::get_id() {
  return id;
}

std::shared_ptr<Point> BCurve::get_point(size_t index) {
  // TODO: handle wrong index
  // if(index >= points.size()) 
  return points[index];
}

size_t BCurve::points_count() const {
  return points.size();
}

size_t BCurve::mouse_over_point(Vec2f mpos) {
  for(size_t i = 0; i < points.size(); i++){ 
    if(points[i]->mouse_in(mpos))
      return i;
  }
  return SIZE_MAX;
}

std::shared_ptr<Point> BCurve::deCasteljau(float t) {
  // Create a copy of control points to work with
  std::vector<std::shared_ptr<Point>> controlPoints = points;
  for (size_t r = 1; r < controlPoints.size(); ++r) {
    for (size_t i = 0; i < controlPoints.size() - r; ++i) {
      // Linear interpolation between adjacent points
      controlPoints[i] = std::make_shared<Point>(
          *controlPoints[i] * (1 - t) + *controlPoints[i + 1] * t);
    }
  }

  // The final point is the point on the Bézier curve
  return controlPoints[0];
}

Point BCurve:: horner_point(float t) {
  float u { 1 - t };
  float bc { 1 };
  float tn { 1 };
  std::shared_ptr<Point> tmp = std::make_shared<Point>(*points[0]);
  int n = points.size();
  for(int i = 1; i < n; i++) {
    tn *= t;
    bc *= ((float)(n - i + 1))/i;
    tmp = std::make_shared<Point>((*tmp + *points[i] * tn * t) * u);
  }
  return *tmp + *points[n - 1] * t * tn;
}

vector<std::shared_ptr<Point>> BCurve::generate_curve_points(int n) {
  vector<std::shared_ptr<Point>> res;
  if(points.size() < 3)
    return res;

  for(int i = 0; i <= n; i++) {
    //std::cout << i << std::endl;
    float t = static_cast<float>(i) / static_cast<float>(n);
    res.push_back(deCasteljau(t));
    //res.push_back(horner_point(t));
  }
  return res;
}

std::vector<std::shared_ptr<Point>> BCurve::graham_scan(std::vector<std::shared_ptr<Point>> points) {
  //  Jeśli mamy mniej niż 3 punkty, nie możemy utworzyć otoczki
  if (points.size() < 3) return points;

  //  Znajdź punkt o najmniejszej współrzędnej y 
  // (w przypadku równych y - punkt o najmniejszym x)
  auto minYPoint = std::min_element(points.begin(), points.end(), 
      [](const std::shared_ptr<Point>& a, const std::shared_ptr<Point>& b) {
      return a->y < b->y || (a->y == b->y && a->x < b->x);
      });

  //  Przesuń punkt o najmniejszej współrzędnej y na początek wektora
  std::swap(*minYPoint, points[0]);
  std::shared_ptr<Point>& pivot = points[0];

  //  Sortuj punkty względem kąta z punktem bazowym
  std::sort(points.begin() + 1, points.end(), 
      [&pivot](const std::shared_ptr<Point>& a, const std::shared_ptr<Point>& b) {
      float orientationVal = orientation(pivot, a, b);

      // Jeśli punkty mają ten sam kąt, wybierz bliższy
      if (orientationVal == 0) {
      return distance(pivot, a) < distance(pivot, b);
      }

      // Sortuj względem kąta (przeciwnie do ruchu wskazówek zegara)
      return orientationVal > 0;
      });

  std::vector<std::shared_ptr<Point>> hull;
  hull.push_back(points[0]);
  hull.push_back(points[1]);

  for (size_t i = 2; i < points.size(); ++i) {
    // Usuwaj punkty z otoczki, które tworzą skręt w prawo
    while (hull.size() > 1) {
      std::shared_ptr<Point> top = hull.back();
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

void BCurve::update() {
  convex_hull = graham_scan(points);
  int curve_points = 20 + (int)(points.size() / 2)*7;
  bezier_points = generate_curve_points(curve_points);
}

void BCurve::draw_points(sf::RenderWindow *window, bool active) {
  for(auto p: points) {
    if(active) 
      p->set_color(sf::Color::Yellow);
    else 
      p->set_color(sf::Color::White);
    p->draw(window);
  }
}

void BCurve::draw_convex_hull(sf::RenderWindow *window) {
  // If convex hull is too small we dont draw it
  if(convex_hull.size() < 3) 
    return;

  for (size_t i = 0; i < convex_hull.size(); ++i) {
    // linia od od itego do i+1 wierzcholka otoczki wypuklej
    sf::Vertex line[] = {
      sf::Vertex(sf::Vector2f(convex_hull[i]->x, convex_hull[i]->y), sf::Color::Blue),
      sf::Vertex(sf::Vector2f(convex_hull[(i+1) % convex_hull.size()]->x,
            convex_hull[(i+1) % convex_hull.size()]->y), sf::Color::Blue)
    };
    window->draw(line, 2, sf::Lines);
  }
}

void BCurve::draw_bezier_lines(sf::RenderWindow *window) {
  if(convex_hull.size() < 3) 
    return;

  for (size_t i = 0; i < bezier_points.size() - 1; ++i) {
    // linie do zrobienia iluzji krzywej beziera
    sf::Vertex line[] = {
      sf::Vertex(sf::Vector2f(bezier_points[i]->x, bezier_points[i]->y), sf::Color::Green),
      sf::Vertex(sf::Vector2f(bezier_points[i + 1]->x, bezier_points[i + 1]->y), sf::Color::Green)
    };
    window->draw(line, 2, sf::Lines);
  }
}


