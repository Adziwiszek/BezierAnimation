#include "../include/user.hpp"

User::User(sf::RenderWindow& _window) : 
    animation_state(animation_manager, _window.getSize()),
    drawer(_window), 
    animation_manager(animation_state.frames, animation_state.active_frame),
    ui_manager(_window, input_handler, drawing_settings, current_state, animation_state),
    input_handler(animation_state.active_frame, animation_state.frames, current_state, 
      actions, animation_state, ui_manager, drawing_settings),
    frames{animation_state.frames},
    active_frame{animation_state.active_frame}
    
{
  input_handler.add_frame(false);
  active_frame = frames->at(0);
}


void User::init_empty() {
  frames->push_back(std::make_shared<Frame>(0)); 
}

void User::handle_input(sf::Event event, InputState& input) {
  input_handler.handle_event(event, input);
}

unsigned User::get_fps() {
  return animation_manager.get_fps();
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
    //actions.push_back("moved point");
    /*std::cout << "-------------\nmoved point\nparent curve id = " 
      << active_frame->active_point->get_parent_id() 
      << "\npoint id = " << active_frame->active_point->get_id()
      << "\npoint pos = " << active_frame->active_point->x << ", " 
      << active_frame->active_point->y << std::endl;*/
    active_frame->active_point->started_moving = false;
    active_frame->active_point = nullptr;
  }

  // we dropped of a curve
  if(!input.left_mouse_down && active_frame->active_curve && 
      active_frame->active_curve->started_moving) {
    //actions.push_back("moved curve");
    /*std::cout << "-------------\nmoved curve\n" 
      << "id = "<< active_frame->active_curve->get_id() <<std::endl;*/
    active_frame->active_curve->started_moving = false;
    //active_frame->active_curve = nullptr;
  }
  
  if(current_state == State::PlayAnimation) {
    animation_manager.play_animation(input.dt);
  } else {
    drawing_settings.update_settings(input.selected_curve, active_frame->active_curve);
    active_frame = animation_state.get_active_frame();
  }

  for(auto& curve: active_frame->curves) {
    curve->update();
  }
  ui_manager.update(input);
}

State User::get_current_state() { return current_state; }

void User::save_to_file(std::string path) {
  animation_state.save_to_file(path);
}

void User::load_from_file(std::string path) {
  animation_state.load_from_file(path);
  active_frame->active_curve = nullptr;
  active_frame->active_point = nullptr;
}

void User::draw(sf::RenderWindow& window, const InputState& input) {
  sf::RenderTexture render_texture;
  if (!render_texture.create(window.getSize().x, window.getSize().y)) {
    std::cerr << "Failed to create render texture" << std::endl;
    return;
  }
  render_texture.clear(sf::Color::Transparent);
  
  sf::RenderTexture background;
  if (!background.create(window.getSize().x, window.getSize().y)) {
    std::cerr << "Failed to create render texture" << std::endl;
    return;
  }
  background.clear(sf::Color::Transparent);

  int nframes = 3;

  for(int i = 0; i < frames->size(); i++) {
    auto frame = frames->at(i);

    if(frame->get_id() == active_frame->get_id()) {
      // drawing active frame
      drawer.draw_frame(render_texture, frame, current_state, 255);
      // drawing background curves
      if(current_state == State::PlayAnimation) continue;
      int end_frame = i == frames->size() - 1 ? 0 : 1;
      for(int j = -nframes; j <= end_frame; j++) {
        if(j == 0) continue;
        auto background_frame =
          frames->at((i + j + frames->size()) % frames->size()); 
        float opacity = 0.9*(float)(- std::abs(j) + nframes)/(nframes); 
        drawer.draw_frame(render_texture, background_frame, 
            current_state, opacity*255);
      }
    } 
  }
  render_texture.display();

  sf::Sprite sprite(render_texture.getTexture());
  sf::Color sprite_col = sprite.getColor();
  sprite.setColor(sprite_col);

  window.draw(sprite);

  ui_manager.drawUI(input);
}

