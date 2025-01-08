#include "../include/frame.hpp"
#include <iostream>

Frame::Frame(unsigned _id): id {_id}
{}

Frame::Frame(const Frame& other, unsigned _id) 
  : curve_counter(other.curve_counter), id {_id}
{
  // deep copy of others curves
  for (const auto& curve : other.curves) {
    auto new_curve = BCurve(*curve, curve_counter++);
    curves.push_back(std::make_shared<BCurve>(new_curve)); 
  }
}

Frame::Frame(const std::shared_ptr<Frame>& other, unsigned _id):
  curve_counter{other->curve_counter}, id{_id} {
  // deep copy of others curves
  for (const auto& curve : other->curves) {
    auto new_curve = BCurve(*curve, curve_counter++);
    curves.push_back(std::make_shared<BCurve>(new_curve)); 
  }
}

unsigned Frame::get_id() {
  return id;
}

void Frame::add_curve(Vec2f pos, float thick, sf::Color col) {
  std::cout << "color(rgb) = " << (int)col.r << " " << (int)col.g 
    << " " << (int)col.b << std::endl;
  std::cout << "thickness  = " << thick << std::endl;
  //std::cout << "color(rgb) = " << col.r << " " << col.g << " " << col.b << std::endl;
  
  if(col == sf::Color::Red)
    std::cout << "color red\n";
  curves.push_back(std::make_shared<BCurve>(pos, curve_counter++, thick, col));
  active_curve = curves[curves.size() - 1];
}

void Frame::add_point_to_current_curve(Vec2f pos) {
  if(active_curve) {
    active_curve->spawn_point(pos);
  }
}

void Frame::delete_curve_by_id(unsigned id) {
  try {
    curves.erase(
      std::remove_if(curves.begin(), curves.end(),
        [id](const std::shared_ptr<BCurve>&c) {
          return c->get_id() == id;
        }),
      curves.end()
    ); 
  } catch(const std::exception& e) {
    std::cout << "Error when trying to delete curve: " << e.what() << std::endl;
  }
}

void Frame::delete_point_from_current_curve(unsigned id) {
  try {
    curves.erase(
      std::remove_if(curves.begin(), curves.end(),
        [id](const std::shared_ptr<BCurve>&c) {
          return c->get_id() == id;
        }),
      curves.end()
    ); 
  } catch(const std::exception& e) {
    std::cerr << "Error when trying to delete curve in Frame: " << std::endl;
    throw e;
  }
}

std::shared_ptr<BCurve> Frame::get_active_curve(Vec2f mpos) {
  for(const auto &curve: curves) {
    if(curve->get_points_count() > 0 && curve->get_points_count() < 3
        || curve->mouse_in_control_point(mpos)) {
      return curve; 
    }
    if(curve->mouse_in_curve(mpos)) {
      return curve;
    }
  }
  return nullptr; 
}

std::shared_ptr<Point> Frame::get_active_point(Vec2f mpos) {
  for(const auto &curve: curves) {
    for(const auto &point: curve->get_control_points()) {
      if(point->mouse_in(mpos)) {
        return point;
      }
    }
  }
  return nullptr;
}
