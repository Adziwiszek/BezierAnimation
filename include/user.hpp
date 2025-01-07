#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "frame.hpp"
#include "drawer.hpp"
#include "inputHandler.hpp"
#include "userUtils.hpp"
#include "animationManager.hpp"

namespace UI {
  enum Orientation {
    Vertical,
    Horizontal
  };
  struct Padding {
    float west;
    float east;
    float north;
    float south;
  };

  class Element {
  protected:
  public: 
    sf::RectangleShape background; 
    Vec2f position;
    Vec2f size;
    Padding padding; 
    float min_width{-1};
    float min_height{-1};
    bool stretch_height{false};
    bool stretch_width{false};

    virtual void draw(sf::RenderWindow& _window) = 0;
    virtual Vec2f calculate_size() = 0;
    void set_position(Vec2f _pos) {
      position = _pos;
      background.setPosition(position);
    }
    void set_size(Vec2f _size) {
      size = _size;
      background.setSize(size);
    }
    void set_color(sf::Color color) {
      background.setFillColor(color);
    }
  };

  class ImageElement : public Element {
    sf::Sprite sprite;
  public:
    ImageElement() {
      set_size({40.0, 40.0});
    }
    void draw(sf::RenderWindow& _window) override {
      _window.draw(background);
      if(sprite.getTexture())
        _window.draw(sprite);
    }
    Vec2f calculate_size() override {return size;}
    void set_texture(const sf::Texture* texture) {
      if(texture) {
        sprite.setTexture(*texture); 
        update_sprite_size();
      }
    }
    void set_position(Vec2f _pos) {
        Element::set_position(_pos);
        sprite.setPosition(position);
    }
    void set_size(Vec2f _size) {
        Element::set_size(_size);
        update_sprite_size();
    }
    void update_sprite_size() {
      if (sprite.getTexture()) {
        const sf::Vector2u textureSize = sprite.getTexture()->getSize();
        const Vec2f scale{
            size.x / static_cast<float>(textureSize.x),
            size.y / static_cast<float>(textureSize.y)
        };
        sprite.setScale(scale.x, scale.y);
      }
    }
  };

  class Container : public Element {
    Orientation orientation;
    std::vector<std::unique_ptr<Element>> children;
    Vec2f spacing{0.0f, 20.0f};
    bool auto_size_x{true};
    bool auto_size_y{true};
    Vec2f max_children_size{0.0, 0.0};
  public:
    void set_padding(Padding _pad) {padding = _pad;}
    void add_elem(std::unique_ptr<Element> elem) {
      if(elem->size.x > max_children_size.x)
        max_children_size.x = elem->size.x;

      children.push_back(std::move(elem));
    }
    void set_orientation(Orientation _or) {
      orientation = _or;
    }
    Vec2f calculate_size() {
      Vec2f my_size{0.0, 0.0};
      my_size += Vec2f{padding.north, padding.west} + Vec2f{padding.east, padding.south};
      float max_size_x{0.0};
      float max_size_y{0.0};
      for(auto& elem: children) {
        Vec2f elem_size = elem->calculate_size();
        max_size_x = std::max(max_size_x, elem_size.x);
        max_size_y = std::max(max_size_y, elem_size.y);
        if(orientation == UI::Vertical) {
          my_size.y += elem_size.y + spacing.y;
        } else if(orientation == UI::Horizontal) {
          my_size.x += elem_size.x + spacing.x;
        }
      }
      if(orientation == UI::Vertical) {
        my_size.y -= spacing.y;
        my_size.x = max_size_x + padding.west + padding.east;
      } else if(orientation == UI::Horizontal) {
        my_size.x -= spacing.x;
        my_size.y = max_size_y + padding.north + padding.south;
      }
      return my_size;
    }
    void draw(sf::RenderWindow& _window) {
      Vec2f final_size {calculate_size()};

      if(stretch_height) {
        final_size.y = _window.getSize().y;
      }
      set_size(final_size);
      _window.draw(background);

      //Vec2f pos{position.x + padding.west, position.y + padding.north};
      Vec2f pos = position + Vec2f{padding.west, padding.north};
      for(auto& elem: children) {
        elem->set_position(pos);
        elem->draw(_window);
        switch(orientation) {
          case UI::Orientation::Horizontal:
            pos.x += spacing.x + elem->size.x;
            break;
          case UI::Orientation::Vertical:
            pos.y += spacing.y + elem->size.y;
            break;
        }
      }
    }
  };

  class Manager {
    std::vector<std::unique_ptr<Element>> elements;
    std::unordered_map<std::string, sf::Texture> textures;
    sf::RenderWindow& window;
  public:
    Manager(sf::RenderWindow& _window): window{_window} {
      auto testcont = std::make_unique<Container>();
      testcont->set_position({0.0, 0.0});
      testcont->set_color(sf::Color::Cyan);
      testcont->set_padding(Padding{5.0, 5.0, 5.0, 5.0});
      testcont->set_orientation(Orientation::Vertical);
      testcont->add_elem(std::make_unique<ImageElement>());
      testcont->add_elem(std::make_unique<ImageElement>());
      testcont->add_elem(std::make_unique<ImageElement>());
      testcont->add_elem(std::make_unique<ImageElement>());

      auto testcont2 = std::make_unique<Container>();
      testcont2->set_position({0.0, 0.0});
      testcont2->set_color(sf::Color::Green);
      testcont2->set_padding(Padding{5.0, 5.0, 5.0, 5.0});
      testcont2->set_orientation(Orientation::Vertical);
      testcont2->add_elem(std::make_unique<ImageElement>());
      testcont2->add_elem(std::make_unique<ImageElement>());
      testcont2->add_elem(std::make_unique<ImageElement>());
      testcont2->add_elem(std::make_unique<ImageElement>());

      auto testcont3 = std::make_unique<Container>(); 
      testcont3->set_position({0.0, 0.0});
      testcont3->set_orientation(Orientation::Horizontal);
      testcont3->stretch_height = true;
      testcont3->set_color(sf::Color::Red);
      testcont3->set_padding(Padding{0.0, 0.0, 0.0, 0.0});
      testcont3->add_elem(std::move(testcont));
      testcont3->add_elem(std::move(testcont2));
      
      elements.push_back(std::move(testcont3));
    }
    void drawUI() {
      Vec2f window_size{window.getSize()};
      for(auto& elem: elements) {
        elem->draw(window);
      }
    }
    bool load_texture(const std::string& path) {
      if(textures.find(path) != textures.end()) {
          return true; 
      }

      sf::Texture texture;
      if (!texture.loadFromFile(path)) {
          std::cerr << "Failed to load texture: " << path << std::endl;
          return false;
      }

      textures[path] = std::move(texture);
      return true;
    } 
    const sf::Texture* get_texture(const std::string& path) const {
      auto it = textures.find(path);
      if (it != textures.end()) {
          return &it->second;
      }
      return nullptr;
    }

  };
};


class User {
private:
  // frames
  std::shared_ptr<Frames> frames; 
  std::shared_ptr<Frame> active_frame;

  State current_state { Normal };

  Drawer drawer;
  InputHandler input_handler;
  AnimationManager animation_manager;
  AnimationState animation_state;
  UI::Manager ui_manager;
public:
  std::vector<std::string> actions;
  User(sf::RenderWindow& _window); 
  User(Frames, unsigned, sf::RenderWindow&);
  void init_empty();
  // handle user input
  void handle_input(sf::Event event, InputState& input); 
  // update state
  void update(const InputState& input); 
  // manage frames of animation
  unsigned get_frame_index();
  unsigned get_frame_count();
  unsigned get_fps();
  // saving files 
  void save_to_file(std::string);
  void load_from_file(std::string);
  // drawing program
  void draw(sf::RenderWindow *window);
};
