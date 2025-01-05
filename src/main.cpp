#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/user.hpp"
#include "../include/frame.hpp"

using Vec2f = sf::Vector2f;

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Bezier Animations");
  User user(window);
  InputState input_state;

  if(argc > 1) {
    if("-h" == std::string(argv[1])) {
      std::cout<<"usage: dupa"<<std::endl;
      return 0;
    } else {
      std::cout<<"loading from file..."<< std::endl;
      user.load_from_file(std::string(argv[1]));
    }
  }   

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

  // help text 
  std::string help_text = 
    "m - move state\ng - add curve state\nh - add point state \
    \nn - normal state\ns - save to file\na - play animation \
    \nf - new frame\ne - next frame\nq - previous frame\nup/down - change fps";
  std::cout << help_text << std::endl;

  /* MAIN LOOP
   * 1) get user input
   * 2) check if they clicked on a point or a curve (set appropriate vars)
   * 3) do stuff with the input (move, add, delete, etc)
   * 4) update world
   * */
  while (window.isOpen())
  {
    sf::Time dt = clock.restart();
    input_state.update_mouse(window);
    input_state.update_dt(dt.asSeconds()); 

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // user input
      user.handle_input(event, input_state);
    }

    // update user 
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
  
  for(const auto& a: user.actions) {
    std::cout << a << std::endl;
  }

  return 0;
}


