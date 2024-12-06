#include <SFML/Graphics.hpp>
#include <cmath>

#include "../include/point.hpp"
#include "../include/bcurve.hpp"
#include "../include/user.hpp"

using Vec2f = sf::Vector2f;

int main(int argc, char **argv)
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  User user {};

  Vec2f mpos { 0.0f, 0.0f };
  Vec2f last_mpos { 0.0f, 0.0f };
  while (window.isOpen())
  {
    last_mpos = mpos;
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
    user.update(mpos, {mpos - last_mpos});

    window.clear(sf::Color::Black);
    user.draw(&window);
    window.display();
  }

  return 0;
}


