#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "point.hpp"
#include "bcurve.hpp"
#include "user.hpp"

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

;

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  User user {};

  Vec2f mpos { 0.0f, 0.0f };
  while (window.isOpen())
  {
    mpos = { static_cast<float>(sf::Mouse::getPosition(window).x), 
      static_cast<float>(sf::Mouse::getPosition(window).y)};

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // user input
      user.handle_input(event, mpos);
    }

    // update users points
    user.update(mpos);

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.display();
  }

  return 0;
}


