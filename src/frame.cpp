#include "../include/frame.hpp"

Frame::Frame(unsigned _id): id {_id}
{}

void Frame::add_curve(Vec2f pos) {
  curves.push_back(std::make_shared<BCurve>(pos, curve_counter++));
  active_curve = curves[curves.size() - 1];
}

void Frame::add_point_to_current_curve(Vec2f pos) {
  if(active_curve) {
    active_curve->spawn_point(pos);
  }
}

std::shared_ptr<BCurve> Frame::get_active_curve(Vec2f mpos) {
  for(const auto &curve: curves) {
    if(curve->get_size() > 0 && curve->get_size() < 3
        || curve->mouse_in_control_point(mpos)) {
      return curve; 
    }
    if(curve->mouse_in(mpos)) {
      return curve;
    }
  }
  return nullptr; 
}

std::shared_ptr<Point> Frame::get_active_point(Vec2f mpos) {
  for(const auto &curve: curves) {
    for(const auto &point: curve->points) {
      if(point->mouse_in(mpos)) {
        return point;
      }
    }
  }
  return nullptr;
}
