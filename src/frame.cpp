#include "../include/frame.hpp"

Frame::Frame(unsigned _id): id {_id}
{}

void Frame::add_curve(Vec2f pos) {
  curves.push_back(std::make_shared<BCurve>(pos, curve_counter++));
}

void Frame::add_point_to_current_curve(Vec2f pos) {
  if(active_curve) {
    active_curve->spawn_point(pos);
  }
}

std::tuple<std::shared_ptr<BCurve>, std::shared_ptr<Point>> 
  Frame::get_active_point_curve(Vec2f mpos) {
  for(const auto &curve: curves) {
    for(const auto &point: curve->points) {
      if(point->mouse_in(mpos)) {
        return {curve, point};
      }
    }
  }
  return {nullptr, nullptr};
}
