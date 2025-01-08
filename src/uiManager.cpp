#include "../include/uiManager.hpp"
#include "../include/inputHandler.hpp"

#define STR_VALUE(arg)      #arg
using UI::Element, UI::ImageElement, UI::Container, UI::Manager;

std::string color_to_string(const sf::Color& color) {
  if (color == sf::Color::Green) return "Green";
  if (color == sf::Color::Red) return "Red";
  if (color == sf::Color::Blue) return "Blue";
  if (color == sf::Color::White) return "White";
  return "Unknown";
}

void Element::set_position(Vec2f _pos) {
  position = _pos;
  background.setPosition(position);
}

void Element::set_size(Vec2f _size) {
  size = _size;
  background.setSize(size);
}

void Element::set_color(sf::Color color) {
  background.setFillColor(color);
}

ImageElement::ImageElement(std::string name) {
  this->name = name;
  background.setOutlineColor(sf::Color::Black);
  set_size({40.0, 40.0});
} 

ImageElement::ImageElement(sf::Color color, std::string name): ImageElement(name) {
  background.setFillColor(color);
}
ImageElement::ImageElement(sf::Color color, std::function<void()> handler, 
    std::string name):
  ImageElement(color, name) {
    set_on_click(handler);
}

ImageElement::ImageElement(const sf::Texture* texture, std::string name):
  ImageElement(name) {
  set_texture(texture); 
}

ImageElement::ImageElement(const sf::Texture* texture, std::function<void()> handler,
    std::string name):
  ImageElement(texture, name) {
    set_on_click(handler);
}

void ImageElement::draw(sf::RenderWindow& _window,
        std::vector<std::string> selected) {
  if (std::find(selected.begin(), selected.end(), name) != selected.end()) {
    outline_thickness = selected_thick;
  } else {
    outline_thickness = unselected_thick;
  }

  background.setOutlineThickness(outline_thickness);
  _window.draw(background);
  if(sprite.getTexture()) {
    sprite.setPosition(position);
    _window.draw(sprite);
  }
}

Vec2f ImageElement::calculate_size() {return size;}

void ImageElement::set_texture(const sf::Texture* texture) {
  if(texture) {
    sprite.setTexture(*texture); 
    update_sprite_size();
  }
}

void ImageElement::set_position(Vec2f _pos) {
  Element::set_position(_pos);
  sprite.setPosition(_pos);
}

void ImageElement::set_size(Vec2f _size) {
  Element::set_size(_size);
  update_sprite_size();
}

void ImageElement::update_sprite_size() {
  if (sprite.getTexture()) {
    const sf::Vector2u textureSize = sprite.getTexture()->getSize();
    //sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    const Vec2f scale{
      size.x / static_cast<float>(textureSize.x),
        size.y / static_cast<float>(textureSize.y)
    };
    sprite.setScale(scale.x, scale.y);
    sprite.setPosition(position);
  }
}

void ImageElement::set_on_click(std::function<void()> handler) {
  on_click_lam = std::move(handler);
}

void ImageElement::on_click(const sf::Vector2f& mpos) {
  if (mpos.x >= position.x && mpos.x <= position.x + size.x &&
      mpos.y >= position.y && mpos.y <= position.y + size.y) {
    if(on_click_lam) {
      on_click_lam(); 
    }
  }  
}

void Container::on_click(const sf::Vector2f& mpos) {
  for(const auto& child: children) {
    child->on_click(mpos);
  }
}

void Container::set_padding(Padding _pad) {padding = _pad;}

void Container::add_elem(std::unique_ptr<Element> elem) {
  if(elem->size.x > max_children_size.x)
    max_children_size.x = elem->size.x;

  children.push_back(std::move(elem));
}

void Container::set_orientation(Orientation _or) {
  orientation = _or;
}

Vec2f Container::calculate_size() {
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

void Container::draw(sf::RenderWindow& _window, 
    std::vector<std::string> selected) {
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
    elem->draw(_window, selected);
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
Manager::Manager(sf::RenderWindow& _window, InputHandler& input_handler,
    DrawingSettings& ds, State& st): window{_window}, drawing_settings{ds},
    current_state{st}{
  if(!load_texture("assets/cursor.png")) {
    std::cout << "failed to load cursor.png" << std::endl;
  }
  auto add_change_state_button = 
    [&input_handler] (std::unique_ptr<Container>& cont, State state) {
      cont->add_elem(std::make_unique<ImageElement>(
            sf::Color::White,
            [&input_handler, state]() { 
              input_handler.switch_to_state(state,"..."); 
            }, "state" )); 
    };

  auto action_cont1 = std::make_unique<Container>();
  action_cont1->set_color(sf::Color::Cyan);
  action_cont1->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  action_cont1->set_orientation(Orientation::Vertical);
  action_cont1->add_elem(std::make_unique<ImageElement>(
        get_texture("assets/cursor.png"),
        [&input_handler]() { 
          input_handler.switch_to_state(State::Move,"Move"); 
        }, "cursor" ));
  add_change_state_button(action_cont1, State::AddCurve);
  add_change_state_button(action_cont1, State::AddPoint);
  add_change_state_button(action_cont1, State::Delete);

  auto action_cont2 = std::make_unique<Container>();
  action_cont2->set_color(sf::Color::Cyan);
  action_cont2->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  action_cont2->set_orientation(Orientation::Vertical);
  action_cont2->add_elem(std::make_unique<ImageElement>("dupa"));
  action_cont2->add_elem(std::make_unique<ImageElement>("dupa"));
  action_cont2->add_elem(std::make_unique<ImageElement>("dupa"));
  action_cont2->add_elem(std::make_unique<ImageElement>("dupa"));

  auto color_container = std::make_unique<Container>();
  color_container->set_color(sf::Color::Cyan);
  color_container->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  color_container->set_orientation(Orientation::Vertical);
  auto add_color_button = [&color_container, this] (sf::Color col) {
    color_container->add_elem(std::make_unique<ImageElement>(
          col,
          [col, this]() {
            drawing_settings.color = col;
          }, color_to_string(col)));
  };
  add_color_button(sf::Color::Green);
  add_color_button(sf::Color::Red);
  add_color_button(sf::Color::Blue);
  add_color_button(sf::Color::White);

  auto size_container = std::make_unique<Container>();
  size_container->set_color(sf::Color::Cyan);
  size_container->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  size_container->set_orientation(Orientation::Vertical);
  auto add_size_button = [&size_container, this] (float thick) {
    size_container->add_elem(std::make_unique<ImageElement>(
          sf::Color::Yellow,
          [this, thick]() {
            drawing_settings.thickness = thick;
          }, std::to_string(thick)));
  };
  add_size_button(3.0);
  add_size_button(5.0);
  add_size_button(7.0);
  add_size_button(11.0);

  auto col1 = std::make_unique<Container>(); 
  col1->set_position({0.0, 0.0});
  col1->set_orientation(Orientation::Vertical);
  col1->stretch_height = true;
  col1->set_color(sf::Color::Cyan);
  col1->set_padding(Padding{0.0, 0.0, 0.0, 0.0});
  col1->add_elem(std::move(action_cont1));
  col1->add_elem(std::move(size_container));

  auto col2 = std::make_unique<Container>(); 
  col2->set_position({0.0, 0.0});
  col2->set_orientation(Orientation::Vertical);
  col2->stretch_height = true;
  col2->set_color(sf::Color::Cyan);
  col2->set_padding(Padding{0.0, 0.0, 0.0, 0.0});
  col2->add_elem(std::move(action_cont2));
  col2->add_elem(std::move(color_container));

  auto final_container = std::make_unique<Container>(); 
  final_container->set_position({0.0, 0.0});
  final_container->set_orientation(Orientation::Horizontal);
  final_container->stretch_height = true;
  final_container->set_color(sf::Color::Cyan);
  final_container->set_padding(Padding{0.0, 0.0, 0.0, 0.0});
  final_container->add_elem(std::move(col1));
  final_container->add_elem(std::move(col2));

  auto text_inp = std::make_unique<TextInput>(current_state);
  elements.push_back(std::move(text_inp));
  elements.push_back(std::move(final_container));

  for(const auto& elem: elements) {
    max_size.x = std::max(max_size.x, elem->calculate_size().x);
    max_size.y = std::max(max_size.y, elem->calculate_size().y);
  }
}

void Manager::drawUI() {
  Vec2f window_size{window.getSize()};
  std::vector<std::string> selected;
  selected.push_back(color_to_string(drawing_settings.color));
  selected.push_back(std::to_string(drawing_settings.thickness));

  for(auto& elem: elements) {
    if (dynamic_cast<UI::TextInput*>(elem.get()) && current_state == State::Saving) {
      elem->draw(window, selected);
    }    
    elem->draw(window, selected);
  }
}

bool Manager::load_texture(const std::string& path) {
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

const sf::Texture* Manager::get_texture(const std::string& path) const {
  auto it = textures.find(path);
  if (it != textures.end()) {
    return &it->second;
  }
  return nullptr;
}

void Manager::handle_click(const sf::Vector2f& mpos) {
  for(const auto& elem: elements) {
    elem->on_click(mpos); 
  }
}

void Manager::handle_input(sf::Event event) {
  for(const auto& elem: elements) {
    elem->handle_input(event); 
  }
}

