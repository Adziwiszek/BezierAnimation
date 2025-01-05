#include "../include/inputHandler.hpp"

using std::cout;

InputHandler::InputHandler(std::vector<std::shared_ptr<Frame>>& frames,
                         std::shared_ptr<Frame>& active_frame,
                         State& current_state,
                         unsigned& frame_counter,
                         unsigned& frame_index,
                         std::vector<std::string>& actions)
  : frames(frames), active_frame(active_frame), current_state(current_state),
    frame_counter(frame_counter), frame_index(frame_index), actions(actions) {}


void InputHandler::handle_mouse_pressed(const InputState& input) {
  if(!input.left_mouse_down)
    return;
  switch(current_state) {
    case State::AddCurve:
      add_new_curve(input.mouse_position);
      //actions.push_back(add curve pos)
      break;

    case State::AddPoint:
      add_point_to_active_curve(input.mouse_position);
      //actions.push_back(add curve pos)
      break;

    case State::Normal:
    case State::Move:
    case State::Delete:
      active_frame->active_point = active_frame->get_active_point(input.mouse_position);
      active_frame->active_curve = active_frame->get_active_curve(input.mouse_position);
      
      if (current_state == State::Delete) {
          if (active_frame->active_point) {
              handle_point_deletion();
          } else if (active_frame->active_curve) {
              handle_curve_deletion();
          }
      }
      break; 

    case State::PlayAnimation:
      break;
  }
}


void InputHandler::add_new_curve(Vec2f pos) {
  if(!active_frame)
    return;

  active_frame->add_curve(pos);
  switch_to_state(State::AddPoint, "AddPoint");
}
void InputHandler::add_point_to_active_curve(  Vec2f pos) {
  if(!active_frame->active_curve)
    return;

  active_frame->active_curve->spawn_point(pos);
}
void InputHandler::handle_point_deletion() {
  if(!active_frame->active_point)
    return;

  try {
    unsigned p_id = active_frame->active_point->get_id();
    active_frame->active_curve->delete_point_by_id(p_id);
  } catch(const std::exception& e) {
    std::cerr << "Error when deleting point in InputHandler: " << e.what() << std::endl;
  }
}
void InputHandler::handle_curve_deletion() {
  if(!active_frame->active_curve)
    return;

  try {
    unsigned c_id = active_frame->active_curve->get_id();
    active_frame->delete_curve_by_id(c_id);
  } catch(const std::exception& e) {
    std::cerr << "Error when deleting curve in InputHandler: " << e.what() << std::endl;
  }
}
void InputHandler::switch_to_state(State new_state, const std::string& state_name) {
  if (current_state == new_state)
    return;

  std::cout << "Switched to " << state_name << "!\n";
  current_state = new_state;
  if(current_state == PlayAnimation) {
    active_frame = frames[frame_index];
  } else {
    //animation_frame_index = 0;
  }
}

void InputHandler::move_active_point(Vec2f pos) {
  if(!active_frame->active_point) 
    return;

  active_frame->active_point->update_position(pos);
  active_frame->active_point->started_moving = true;
}

void InputHandler::move_active_curver(Vec2f pos) { 
  // we don't want to move the curve if we are clicking on a point
  if(active_frame->active_point || !active_frame->active_curve) 
    return;

  active_frame->active_curve->started_moving = true;
  for(const auto &point: active_frame->active_curve->get_control_points()) {
    if(!point) continue;
    Vec2f new_pos { point->x + pos.x,
                  point->y + pos.y };
    point->update_position(new_pos); 
  }
}

void InputHandler::handle_mouse_movement(const InputState& input) {
  if(input.mouse_delta == Vec2f{0, 0})
    return;

  // right now all actions that are done by movement require lmb to be pressed
  if(!input.left_mouse_down)
    return;

  if(current_state == State::Move) {
    move_active_point(input.mouse_position);
    move_active_curver(input.mouse_delta);
  }
}

void InputHandler::add_frame(bool copy_frame) {
  std::shared_ptr<Frame> new_frame;
  if(copy_frame) {
    new_frame = std::make_shared<Frame>(*frames[frame_index], frame_counter++);
  } else { 
    new_frame = std::make_shared<Frame>(frame_counter++);
  }
  // if frame we are adding is first we do special stuff
  if(frames.empty() || frame_index == frames.size()-1) {
    frames.push_back(new_frame);
  } else {
    frames.insert(frames.begin() + frame_index + 1, new_frame);
  }
}
void InputHandler::next_frame() {
  if(frame_index < frames.size() - 1) { 
    active_frame = frames[++frame_index];
  }
}
void InputHandler::prev_frame() {
  if(frame_index > 0) { 
    active_frame = frames[--frame_index];
  }
}

void InputHandler::handle_key_pressed(sf::Keyboard::Key key, const InputState& input) {
  switch (key) {
    case sf::Keyboard::Key::G:
      switch_to_state(State::AddCurve, "AddCurve");
      break;
    case sf::Keyboard::Key::N:
      switch_to_state(State::Normal, "Normal");
      break;
    case sf::Keyboard::Key::M:
      switch_to_state(State::Move, "Move");
      break;
    case sf::Keyboard::Key::H:
      switch_to_state(State::AddPoint, "AddPoint");
      break;
    case sf::Keyboard::Key::A:
      switch_to_state(State::PlayAnimation, "PlayAnimation");
      break;
    case sf::Keyboard::Key::S:
      //save_to_file("test.txt");
      break;
    case sf::Keyboard::Key::Up:
      //fps += 1;
      break;
    case sf::Keyboard::Key::Down:
      //fps -= 1;
      break;
    case sf::Keyboard::Key::F:
      add_frame(true);
      next_frame();
      actions.push_back("added frame");
      break;
    case sf::Keyboard::Key::E:
      actions.push_back("next frame");
      next_frame();
      break;
    case sf::Keyboard::Key::Q:
      actions.push_back("previous frame");
      prev_frame();
      break;
    case sf::Keyboard::Key::D:
      switch_to_state(State::Delete, "Delete point");
      break;
    case sf::Keyboard::Key::Z:
      /*if(active_curve && input.ctrl_pressed) {
        active_curve->undo_last_point();
      }*/
      break;
    default:
      break;
  }
}
