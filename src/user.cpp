#include "../include/user.hpp"
#include <fstream>
#include <ranges>

User::User(sf::RenderWindow& _window) : 
    animation_state(animation_manager),
    drawer(_window), 
    animation_manager(animation_state.frames, animation_state.active_frame),
    ui_manager(_window, input_handler, drawing_settings, current_state, animation_state),
    input_handler(animation_state.active_frame, current_state, 
      actions, animation_state, ui_manager, drawing_settings),
    frames{animation_state.frames},
    active_frame{animation_state.active_frame}
    
{
  input_handler.add_frame(false);
  active_frame = frames->at(0);
}

/*User::User(Frames _frames, unsigned fc, sf::RenderWindow& _window)
  : frames {std::make_shared<Frames>(std::move(_frames))}, 
    drawer(_window),
    animation_manager(frames),
    animation_state(frames),
    ui_manager(_window, input_handler, drawing_settings, current_state, animation_state),
    input_handler(active_frame, current_state, 
      actions, animation_state, ui_manager, drawing_settings)
     {
  active_frame = frames->at(0);
}*/

void User::init_empty() {
  frames->push_back(std::make_shared<Frame>(0)); 
}

void User::handle_input(sf::Event event, InputState& input) {
  input_handler.handle_event(event, input);
}

unsigned User::get_fps() {
  return animation_manager.get_fsp();
}

unsigned update_index(Frames &frames, Frames::iterator it) {
  return (unsigned)std::distance(frames.begin(), it);
}

unsigned User::get_frame_index() {
  return animation_state.get_frame_index() + 1;
}

unsigned User::get_frame_count() {
  return animation_state.get_frame_count();
}

void User::update(InputState& input) {
  //handling input here idk
  input_handler.handle_mouse_movement(input);

  // we dropped of a point
  if(!input.left_mouse_down && active_frame->active_point && 
      active_frame->active_point->started_moving) {
    actions.push_back("moved point");
    std::cout << "-------------\nmoved point\nparent curve id = " 
      << active_frame->active_point->get_parent_id() 
      << "\npoint id = " << active_frame->active_point->get_id()
      << "\npoint pos = " << active_frame->active_point->x << ", " 
      << active_frame->active_point->y << std::endl;
    active_frame->active_point->started_moving = false;
    active_frame->active_point = nullptr;
  }

  // we dropped of a curve
  if(!input.left_mouse_down && active_frame->active_curve && 
      active_frame->active_curve->started_moving) {
    actions.push_back("moved curve");
    std::cout << "-------------\nmoved curve\n" 
      << "id = "<< active_frame->active_curve->get_id() <<std::endl;
    active_frame->active_curve->started_moving = false;
    //active_frame->active_curve = nullptr;
  }
  
  std::cout << "af id = " << active_frame->get_id() << std::endl;
  std::cout << "fs size = " << frames->size() << std::endl;

  if(current_state == State::PlayAnimation) {
    animation_manager.play_animation(input.dt);
  } else {
    drawing_settings.update_settings(input.selected_curve, active_frame->active_curve);
    active_frame = animation_state.get_active_frame();
  }

  for(auto& curve: active_frame->curves) {
    curve->update();
  }
}

void User::save_to_file(std::string path) {
  animation_state.save_to_file(path);
}

void User::load_from_file(std::string path) {
  animation_state.load_from_file(path);
  active_frame->active_curve = nullptr;
  active_frame->active_point = nullptr;
}

// TODO add faded drawing
//

void User::draw_bclines_from_frame(const std::shared_ptr<Frame>& frame,
    sf::RenderWindow *window, sf::Uint8 opacity) {
  for(auto curve: frame->curves) {
    // drawing control points
    if(current_state != State::PlayAnimation && frame->active_curve &&
        frame->active_curve->get_id() == curve->get_id()) {
      bool active = false;
      if(frame->active_curve && 
          curve->get_id() == frame->active_curve->get_id())
        active = true;
      drawer.draw_control_points(curve->get_control_points(), active);
    }
    // drawing bezier curve
    drawer.draw_bc_lines(curve->get_bc_line_points(), 
        curve->get_color(), curve->get_thickness(), opacity);
    // drawing convex hull
    // drawer.draw_convex_hull(curve->get_convex_hull_points());
  }
}

void draw_background_curves_from_frame(
    const std::shared_ptr<Frame>& frame, sf::RenderWindow* window,
    sf::Uint8 opacity, const State& current_state, Drawer& drawer) {
  for(auto curve: frame->curves) {
    drawer.draw_bc_lines_for_background(curve->get_bc_line_points(), 
        curve->get_color(), curve->get_thickness(), opacity);
  }
}
  

void User::draw(sf::RenderWindow *window) {
  for(int i = 0; i < frames->size(); i++) {
    auto frame = frames->at(i);
    if(frame->get_id() == active_frame->get_id()) {
      draw_bclines_from_frame(frame, window, 255);
    } 
    if(current_state == State::PlayAnimation)
      continue;
    if(frames->at((i + 1) % frames->size())->get_id() == active_frame->get_id()) {
      draw_background_curves_from_frame(frame, window, 100,
          current_state, drawer);
    }
    if(frames->at((i + 2) % frames->size())->get_id() == active_frame->get_id()) {
      draw_background_curves_from_frame(frame, window, 50,
          current_state, drawer);
    }
  }

  ui_manager.drawUI();
}

