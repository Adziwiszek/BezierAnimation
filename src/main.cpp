#include <SFML/Graphics.hpp>
#include <cmath>

#include "../include/user.hpp"

using Vec2f = sf::Vector2f;

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  User user {};
  InputState input_state;

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

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.display();
  }

  return 0;
}


