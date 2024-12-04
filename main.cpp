#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

using Vec2f = sf::Vector2f;
using std::unique_ptr, std::vector;

class Point {
protected:
  sf::RectangleShape representation;
  Vec2f position;
  Vec2f size;

public:
  Point(Vec2f _position)
    : position { _position }
  {
    size = Vec2f(10.0f, 10.0f);
    representation = sf::RectangleShape(size);
    representation.setPosition(position);
    representation.setFillColor(sf::Color::Blue);
    representation.setOutlineColor(sf::Color::Red);
    representation.setOutlineThickness(2.0f);
  }
  
  bool mouse_in(Vec2f mpos) {
    if(mpos.x >= position.x && mpos.x <= position.x + size.x &&
       mpos.y >= position.y && mpos.y <= position.y + size.y)
      return true;
    return false;
  }

  void update_position(Vec2f pos) {
    position = pos;
  }

  void draw(sf::RenderWindow *window) {
    representation.setPosition(position);
    window->draw(representation);
  }
};

class BCurve {
public:
  void spawn_point(Vec2f pos) {
    points.push_back(Point(pos));
  }

  void draw_points(sf::RenderWindow *window) {
    for(auto p: points) {
      p.draw(window);
    }
  }

  Point* mouse_over_point(Vec2f mpos) {
    for(size_t i = 0; i < points.size(); i++){ 
      Point* p = &points[i];
      if(p->mouse_in(mpos)) {
        return p;
      }
    }
    return NULL;
  }

  void undo_last_point() {
    if(points.size() > 0) {
      points.pop_back();
    }
  }

private:
  vector<Point> points;
};

class User {
private:
  BCurve* active_curve;
  Point* active_point;
  vector<BCurve> curves;

  bool lmb_pressed { false };
  bool z_pressed { false };

public:
  User() {
    curves.push_back(BCurve()); 
    active_curve = &curves[0];
  }

  void handle_input(sf::Event event, Vec2f mpos) {
    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        active_point = active_curve->mouse_over_point(mpos);
        if(active_point == NULL) { // we are not mousing over a point
          if(!lmb_pressed) { // if we are not mousing over we add a point
            active_curve->spawn_point(mpos);
          }
        }         
        lmb_pressed = true;
      }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        active_point = NULL;
        lmb_pressed = false;
      }
    }

    if(event.type == sf::Event::KeyPressed) {
      if(event.key.control && event.key.scancode == sf::Keyboard::Scan::Z && !z_pressed) {
        active_curve->undo_last_point();
        z_pressed = true;
      }
    }
    if(event.type == sf::Event::KeyReleased) {
      if(event.key.scancode == sf::Keyboard::Scan::Z) {
        z_pressed = false;
      }
    }
  }

  void update(Vec2f mpos) {
    if(lmb_pressed) {
      if(active_point != NULL) {
        active_point->update_position(mpos);
      }
    }
  }

  void drawCurvePoints(sf::RenderWindow *window) {
    for(auto curve: curves) {
      curve.draw_points(window);
    }
  }
  
};

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
    user.drawCurvePoints(&window);
    window.display();
  }

  return 0;
}
