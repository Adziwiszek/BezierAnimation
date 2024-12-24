#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/user.hpp"
#include "../include/frame.hpp"

using Vec2f = sf::Vector2f;

Frames load_from_file(std::string path) {
  std::ifstream input(path);
  if(!input) {
    std::cerr << "Error: can't open " << path << std::endl;
    return {};
  }
  
  std::string line;
  std::getline(input, line);
  int n_frames = std::stoi(line);
  Frames frames;
  for(int i = 0; i < n_frames; i++) {
    frames.push_back(std::make_shared<Frame>(i));
  }
  while(std::getline(input, line)) {
    std::istringstream stream(line);
    std::string action, _x, _y, _f_id, _c_id;
    if(stream >> action >> _x >> _y >> _f_id >> _c_id) {
      float x = std::stof(_x);
      float y = std::stof(_y);
      int f_id = std::stoi(_f_id);
      int c_id = std::stoi(_c_id);
      if(action == "ADDC") {
        frames[f_id]->add_curve({x, y}); 
      } else if(action == "ADDP") {
        frames[f_id]->active_curve = frames[f_id]->curves[c_id];
        frames[f_id]->add_point_to_current_curve({x, y}); 
      } 
    } else {
      std::cerr << "file error" << std::endl;
    }
  }
  std::cout << "success!" << std::endl;
  input.close();
  return frames;
}

int main(int argc, char **argv)
{
  Frames f;
  if(argc > 1) {
    if("-h" == std::string(argv[1])) {
      std::cout<<"usage: dupa"<<std::endl;
      return 0;
    } else {
      std::cout<<"loading from file..."<< std::endl;
      f = load_from_file(std::string(argv[1])); 
    }
  } else {
    f.push_back(std::make_shared<Frame>(0)); 
  }
  sf::RenderWindow window(sf::VideoMode(800, 600), "Bezier Animations");
  User user(f, f.size());
  InputState input_state;
  
  sf::Font font;
  if(!font.loadFromFile("Roboto-Black.ttf")) {
    std::cerr << "Error: Could not load font!" << std::endl;
  }
  sf::Text frame_info;
  frame_info.setFont(font);
  frame_info.setCharacterSize(24);
  frame_info.setFillColor(sf::Color::White);
  frame_info.setPosition(0.f, 0.f);

  sf::Clock clock;

  while (window.isOpen())
  {
    sf::Time dt = clock.restart();
    input_state.update_mouse(window);
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // user input
      user.handle_input(event, input_state);
    }

    input_state.update_dt(dt.asSeconds()); 
    // update users points
    user.update(input_state);
    std::string state_text = 
      "frame = "+ std::to_string(user.get_frame_index()) + "/" 
      + std::to_string(user.get_frame_count()) +
      "\nfps = " + std::to_string(user.get_fps());
    frame_info.setString(state_text);

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.draw(frame_info);
    window.display();
  }
  
  return 0;
}


