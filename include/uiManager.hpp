#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "frame.hpp"
#include "userUtils.hpp"

class InputHandler;

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
    virtual void on_click(const sf::Vector2f& mpos) = 0;
    void set_position(Vec2f _pos); 
    void set_size(Vec2f _size);
    void set_color(sf::Color color);
  };

  class ImageElement : public Element {
    sf::Sprite sprite;
    std::function<void()> on_click_lam;
  public:
    ImageElement();
    ImageElement(sf::Color color);
    ImageElement(sf::Color color, std::function<void()> handler);
    ImageElement(const sf::Texture* texture);
    ImageElement(const sf::Texture* texture, std::function<void()> handler);
    void draw(sf::RenderWindow& _window) override;
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
    void on_click(const sf::Vector2f& mpos);
    void set_padding(Padding _pad);
    void add_elem(std::unique_ptr<Element> elem);
    void set_orientation(Orientation _or);
    Vec2f calculate_size();
    void draw(sf::RenderWindow& _window);
  };

  class Manager {
    std::vector<std::unique_ptr<Element>> elements;
    std::unordered_map<std::string, sf::Texture> textures;
    sf::RenderWindow& window;
  public:
    Vec2f max_size{0.0, 0.0};
    Manager(sf::RenderWindow& _window, InputHandler& input_handler,
        DrawingSettings& drawing_settings);
    void drawUI();
    bool load_texture(const std::string& path);
    const sf::Texture* get_texture(const std::string& path) const;
    void handle_click(const sf::Vector2f& mpos);
  };
};
