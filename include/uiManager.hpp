#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include "frame.hpp"
#include "userUtils.hpp"

class InputHandler;

namespace UI {
  constexpr static float selected_thick{6.0};
  constexpr static float unselected_thick{3.0};
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

    virtual void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) = 0;
    virtual Vec2f calculate_size() = 0;
    virtual void on_click(const sf::Vector2f& mpos) = 0;
    virtual void update() {}
    virtual void handle_input(sf::Event event) {}
    void set_position(Vec2f _pos); 
    void set_size(Vec2f _size);
    void set_color(sf::Color color);
  };

  class TextInput : public Element {
    Vec2f center_pos;
    sf::Font font;
    sf::Text text;
    std::string input;
    State& current_state;
    bool started_typing{false};
  public:
    TextInput(State& cs, Vec2f pos): current_state{cs},
      center_pos{pos} { 
      if (!font.loadFromFile("assets/Roboto-Black.ttf")) {
          throw std::runtime_error("Failed to load font");
      }
      text.setFont(font);
      text.setCharacterSize(24);
      text.setFillColor(sf::Color::White);
    }
    Vec2f calculate_size() override {return size;}
    void on_click(const sf::Vector2f& mpos) override {}
    void handle_input(sf::Event event) override {
      std::cout << "textint = " << input<<"\n";
      if (event.type == sf::Event::TextEntered) {
        if(!started_typing) {
          started_typing = true;
          return;
        }
        // enter
        if (event.text.unicode == 13) {
          std::cout << "saving to file = " << input << std::endl;
          current_state = State::Normal;
          started_typing = false;
        }
        if (event.text.unicode == 8 && !input.empty()) {
          input.pop_back();  // Remove last character
        }
        else if (event.text.unicode < 128 && event.text.unicode != 8) {
          input += static_cast<char>(event.text.unicode);
        }
      }
    }
    void update() override {
      text.setString("Enter file path: " + input);
    }
    void draw(sf::RenderWindow& window, 
        std::vector<std::string> selected) override {
      text.setString(input);
      text.setPosition(center_pos);
      window.draw(text);
    }
    std::string getInput() const {
      return input;
    }
  };

  class ImageElement : public Element {
    std::string name;
    sf::Sprite sprite;
    std::function<void()> on_click_lam;
  public:
    float outline_thickness{3.0};

    ImageElement(std::string name);
    ImageElement(sf::Color color, std::string name);
    ImageElement(sf::Color color, std::function<void()> handler, std::string name);
    ImageElement(const sf::Texture* texture, std::string name);
    ImageElement(const sf::Texture* texture, 
        std::function<void()> handler, std::string name);
    void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) override;
    Vec2f calculate_size() override;
    void set_texture(const sf::Texture* texture);
    void set_position(Vec2f _pos);
    void set_size(Vec2f _size);
    void update_sprite_size();
    void set_on_click(std::function<void()> handler);
    void on_click(const sf::Vector2f& mpos) override;
  };

  class Container : public Element {
    std::vector<std::unique_ptr<Element>> children;
    Orientation orientation;
    Vec2f spacing{0.0f, 20.0f};
    bool auto_size_x{true};
    bool auto_size_y{true};
    Vec2f max_children_size{0.0, 0.0};
  public:
    void on_click(const sf::Vector2f& mpos) override;
    void set_padding(Padding _pad);
    void add_elem(std::unique_ptr<Element> elem);
    void set_orientation(Orientation _or);
    Vec2f calculate_size() override;
    void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) override;
  };

  class Manager {
    std::vector<std::unique_ptr<Element>> elements;
    std::unordered_map<std::string, sf::Texture> textures;
    sf::RenderWindow& window;
    DrawingSettings& drawing_settings;
    State& current_state;

  public:
    Vec2f max_size{0.0, 0.0};
    Manager(sf::RenderWindow& _window, InputHandler& input_handler,
        DrawingSettings& ds, State& st);
    void drawUI();
    void handle_input(sf::Event event);
    bool load_texture(const std::string& path);
    const sf::Texture* get_texture(const std::string& path) const;
    void handle_click(const sf::Vector2f& mpos);
  };
};
