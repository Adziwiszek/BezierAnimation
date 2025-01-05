#include "../include/bcurve.hpp"

#include<iostream>
using std::cout;

BCurve::BCurve(unsigned id) : id { id } {
}

BCurve::BCurve(Vec2f pos, unsigned id) : id { id } {
  spawn_point(pos);
}

BCurve::BCurve(const BCurve& other, unsigned id) : id { id } {
  for(auto const& p: other.points) {
    spawn_point(p->get_position());
  }
}

float orientation(const std::shared_ptr<Point>& p, 
                  const std::shared_ptr<Point>& q, 
                  const std::shared_ptr<Point>& r) {
    return (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
}

float distance(const std::shared_ptr<Point>& p1, const std::shared_ptr<Point>& p2) {
  return std::sqrt(std::pow(p1->x - p2->x, 2) + std::pow(p1->y - p2->y, 2));
}

void BCurve::spawn_point(Vec2f pos) {
  points.push_back(std::make_shared<Point>(pos, id, point_counter++));
}

void BCurve::delete_point_by_id(unsigned id) {
  try {
    points.erase(
      std::remove_if(points.begin(), points.end(),
        [id](const std::shared_ptr<Point>&p) {
          return p->get_id() == id;
        }),
      points.end()
    ); 
  } catch(const std::exception& e) {
    std::cerr << "Error when deleting point in BCurve" << std::endl;
    throw e;
  }
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

size_t BCurve::get_points_count() const {
  return points.size();
}

size_t BCurve::get_bc_points_count() const {
  return bc_points.size();
}

const Points& BCurve::get_convex_hull_points() const {
  return convex_hull;
}

const Points& BCurve::get_control_points() const {
  return points;
}

const Points& BCurve::get_bc_line_points() const {
  return bc_points;
}

size_t BCurve::mouse_over_point(Vec2f mpos) {
  for(size_t i = 0; i < points.size(); i++){ 
    if(points[i]->mouse_in(mpos))
      return i;
  }
  return SIZE_MAX;
}

bool BCurve::mouse_in_curve(Vec2f mpos) {
  for(const auto& p: bc_points) {
    if(p->mouse_in(mpos)) {
      return true;
    }
  }
  return false;
}

bool BCurve::mouse_in_control_point(Vec2f mpos) {
  for(const auto& p: points) {
    if(p->mouse_in(mpos)) {
      return true;
    }
  }
  return false;
}

Vec2f BCurve::linear_bezier_eval(unsigned int n, float t) {
  float h = 1.0;
  float u = 1.0 - t;
  unsigned int n1 = n + 1;
  Vec2f Q(points[0]->x, points[0]->y);

  if(t <= 0.5) {
    u = t / u;
    for(int k = 1; k < n; k++)
    {
      h = h * u * (n1 - k);
      h = h / (k + h);
      float h1 = 1 - h;
      Q.x = h1 * Q.x + h * (points[k]->x);
      Q.y = h1 * Q.y + h * (points[k]->y);
    }
  } else {
    u = u / t;
    for(int k = 1; k < n; k++)
    {
      h = h * (n1 - k);
      h = h / (k * u + h);
      float h1 = 1 - h;
      Q.x = h1 * Q.x + h * (points[k]->x);
      Q.y = h1 * Q.y + h * (points[k]->y);
    }
  }
  return Q;
}

vector<std::shared_ptr<Point>> BCurve::generate_curve_points(int n) {
  vector<std::shared_ptr<Point>> res;
  if(points.size() < 3)
    return res;

  for(int i = 0; i <= n; i++) {
    float t = static_cast<float>(i) / static_cast<float>(n);
    Vec2f p_coords = linear_bezier_eval(points.size(), t);
    res.push_back(std::make_shared<Point>(p_coords, id, i));
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

  return hull;
}

void BCurve::update() {
  convex_hull = graham_scan(points);
  int curve_points = 20 + (int)(points.size() / 2)*7;
  bc_points = generate_curve_points(curve_points);
}

