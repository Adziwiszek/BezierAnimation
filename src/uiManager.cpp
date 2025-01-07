#include "../include/uiManager.hpp"
#include "../include/inputHandler.hpp"

using UI::Element, UI::ImageElement, UI::Container, UI::Manager;

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

ImageElement::ImageElement() {
  set_size({40.0, 40.0});
} 

ImageElement::ImageElement(const sf::Texture* texture): ImageElement() {
  set_texture(texture); 
}

ImageElement::ImageElement(const sf::Texture* texture, std::function<void()> handler):
  ImageElement(texture) {
    set_on_click(handler);
}

void ImageElement::draw(sf::RenderWindow& _window) {
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

void Container::draw(sf::RenderWindow& _window) {
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

Manager::Manager(sf::RenderWindow& _window, InputHandler& input_handler): window{_window} {
  if(!load_texture("assets/cursor.png")) {
    std::cout << "failed to load cursor.png" << std::endl;
  }

  auto testcont = std::make_unique<Container>();
  testcont->set_color(sf::Color::Cyan);
  testcont->set_padding(Padding{20.0, 10.0, 20.0, 20.0});
  testcont->set_orientation(Orientation::Vertical);
  testcont->add_elem(std::make_unique<ImageElement>(
        get_texture("assets/cursor.png"),
        [&input_handler]() {
        input_handler.switch_to_state(State::Normal,"Normal");
        std::cout<<"dupa\n";
        } ));
  testcont->add_elem(std::make_unique<ImageElement>());
  testcont->add_elem(std::make_unique<ImageElement>());
  testcont->add_elem(std::make_unique<ImageElement>());

  auto testcont2 = std::make_unique<Container>();
  testcont2->set_color(sf::Color::Green);
  testcont2->set_padding(Padding{10.0, 20.0, 20.0, 30.0});
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

void Manager::drawUI() {
  Vec2f window_size{window.getSize()};
  for(auto& elem: elements) {
    elem->draw(window);
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

