#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <optional>

#include "animationState.hpp"
#include "frame.hpp"
#include "userUtils.hpp"

using std::optional;
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
    optional<State> required_state{std::nullopt}; 
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
    virtual void on_click(const InputState& input) = 0;
    virtual void update(const InputState& input) = 0;
    virtual void handle_input(sf::Event event) = 0;
    void set_position(Vec2f _pos); 
    void set_size(Vec2f _size);
    void set_color(sf::Color color);
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
    void update(const InputState& input) override {};
    void handle_input(sf::Event event) override {};
    void on_click(const InputState& input) override;
    void set_texture(const sf::Texture* texture);
    void set_position(Vec2f _pos);
    void set_size(Vec2f _size);
    void update_sprite_size();
    void set_on_click(std::function<void()> handler);
  };

  class Container : public Element {
    std::vector<std::unique_ptr<Element>> children;
    Orientation orientation;
  protected:
    Vec2f spacing{0.0f, 20.0f};
    bool auto_size_x{true};
    bool auto_size_y{true};
    Vec2f max_children_size{0.0, 0.0};
  public:
    void update(const InputState& input) override;
    void on_click(const InputState& input) override;
    void handle_input(sf::Event event) override {};
    void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) override;
    void set_padding(Padding _pad);
    void add_elem(std::unique_ptr<Element> elem);
    void set_orientation(Orientation _or);
    Vec2f calculate_size() override;
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
        DrawingSettings& ds, State& st, AnimationState& animation_state);
    void drawUI();
    void update(const InputState& input);
    void handle_input(sf::Event event);
    bool load_texture(const std::string& path);
    const sf::Texture* get_texture(const std::string& path) const;
    void handle_click(const InputState& input);
    bool check_if_elem_can_act(const std::unique_ptr<Element>& elem, 
        const State& curr_state);
  };


  class Tooltip : public Element {

  };

  class TextInput : public Element {
    Vec2f center_pos;
    sf::Font font;
    sf::Text text;
    std::string input_text;
    State& current_state;
    bool started_typing{false};
    AnimationState& animation_state;
  public:
    TextInput(State& cs, Vec2f pos, AnimationState& as): 
      current_state{cs}, center_pos{pos}, animation_state{as} { 
      required_state = State::Saving;
      if (!font.loadFromFile("assets/Roboto-Black.ttf")) {
          throw std::runtime_error("Failed to load font");
      }
      text.setFont(font);
      text.setCharacterSize(24);
      text.setFillColor(sf::Color::Black);
      background.setOutlineColor(sf::Color::Black);
      background.setOutlineThickness(3.0f);
    }
    Vec2f calculate_size() override {return size;}
    void on_click(const InputState& input) override {}
    void handle_input(sf::Event event) override {
      if (event.type == sf::Event::TextEntered) {
        if(!started_typing) {
          started_typing = true;
          return;
        }
        // enter
        if (event.text.unicode == 13) {
          std::cout << "saving to file = " << input_text << std::endl;
          animation_state.save_to_file(input_text);
          current_state = State::Normal;
          started_typing = false;
        }
        if (event.text.unicode == 8 && !input_text.empty()) {
          input_text.pop_back();  // Remove last character
        }
        else if (event.text.unicode < 128 && event.text.unicode != 8) {
          input_text += static_cast<char>(event.text.unicode);
        }
      }
    }
    void update(const InputState& input) override {
      //text.setString("Enter file path: " + input_text);
    }
    void draw(sf::RenderWindow& window, 
        std::vector<std::string> selected) override {
      text.setString("Enter file path: " + input_text);

      sf::FloatRect text_bounds = text.getLocalBounds();
      set_size({text_bounds.width + 20.0f, text_bounds.height + 20.0f});
      background.setFillColor(sf::Color::Cyan);
      Vec2f bg_pos = {
          center_pos.x - text_bounds.width/2,
          center_pos.y - text_bounds.height/2
          };
      Vec2f txt_pos = {
          bg_pos.x - (background.getSize().x - text_bounds.width)/2,
          bg_pos.y - (background.getSize().y - text_bounds.height)/2 + 5.0f
        };
      text.setPosition(bg_pos);
      background.setPosition(txt_pos);
      window.draw(background);
      window.draw(text);
    }
    std::string get_input() const {
      return input_text;
    }
  };

  class Slider : public Element {
    float max_value{0};
    float min_value{0};
    float current_value{0};
    std::function<void(float)> on_value_change;
    bool started_sliding{false};
    sf::RectangleShape slider_indicator;
  public:
    Slider(float minv, float maxv, std::function<void(float)> onvalchange):
      max_value{maxv}, min_value{minv}, current_value{(max_value-min_value)/2},
      on_value_change{onvalchange}
    {
      slider_indicator.setFillColor(sf::Color::Black);
      slider_indicator.setPosition(background.getPosition());
    }
    void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) override {
      _window.draw(background);
      _window.draw(slider_indicator);
    }
    void set_size(Vec2f _size) {
      slider_indicator.setSize({_size.x, _size.x/15});
      Element::set_size(_size);
    }
    void set_position(Vec2f _pos) {
      slider_indicator.setPosition({
          _pos.x,
          slider_indicator.getPosition().y
          }); 
      Element::set_position(_pos);
    }
    Vec2f calculate_size() override {return size;}
    void on_click(const InputState& input) override {
      auto mpos = input.mouse_position;
      if (mpos.x >= position.x && mpos.x <= position.x + size.x &&
        mpos.y >= position.y && mpos.y <= position.y + size.y) {
        started_sliding = input.left_mouse_down;
      }
    }
    void update(const InputState& input)  override {
      if(!input.left_mouse_down)
        started_sliding = false;
      if(started_sliding) {
        float val_on_slider = 
          std::clamp(input.mouse_position.y, position.y, position.y+size.y) - position.y; 
        float percentage = (val_on_slider / size.y);
        current_value = max_value * percentage;
        slider_indicator.setPosition({
            position.x,
            position.y + val_on_slider
            });
      }
      
      std::cout << "current val = " << current_value << std::endl;
    }
    void handle_input(sf::Event event)  override {}
  };

  class ColorPicker : public Container {
    sf::Color selected_color{sf::Color::Red};
    sf::Color old_color{selected_color};
    float r,g,b;
  public:
    ColorPicker() {
      spacing={0,0};
      required_state = State::PickColor;
      background.setFillColor(sf::Color::Cyan);
      background.setPosition(400, 400);
      set_size({200,200});
      set_padding({10.0, 10.0, 10.0, 10.0});
      set_orientation(Orientation::Vertical);
      auto slider_R = std::make_unique<Slider>(0, 255, 
          [this](float new_r) {r = new_r;});
      slider_R->set_size({50.0, 100.0});
      slider_R->set_color(sf::Color::White);

      add_elem(std::move(slider_R));
    }

    void draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) override {
      auto window_size = _window.getSize();
      set_position({
          (window_size.x - size.x)/2,
          (window_size.y - size.y)/2
          });
      Container::draw(_window, selected);
    }

    //Vec2f calculate_size() override {return size;};
    void on_click(const InputState& input) override {
      Container::on_click(input);
    }
    void update(const InputState& input)  override {
      Container::update(input);
    }
    void handle_input(sf::Event event) override{}
  };
};
