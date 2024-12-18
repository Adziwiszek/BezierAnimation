#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

#include "../include/user.hpp"

using Vec2f = sf::Vector2f;

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Bezier Animations");
  User user {};
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

  while (window.isOpen())
  {
    input_state.update_mouse(window);
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // user input
      user.handle_input(event, input_state);
    }

    // update users points
    user.update(input_state);
    frame_info.setString("frame = "+ std::to_string(user.get_frame_index()) + "/" + std::to_string(user.get_frame_count()));

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.draw(frame_info);
    window.display();
  }

  return 0;
}


