#include "../include/uiManager.hpp"
#include "../include/inputHandler.hpp"

#define STR_VALUE(arg)      #arg
using UI::Element, UI::ImageElement, UI::Container, UI::Manager;

std::string state_to_str(State st) {
  switch(st) {
    case Normal:
      return "Normal";
      break;
    case Move:
      return "Move";
      break;
    case AddCurve:
      return  "AddCurve";
      break;
    case AddPoint:
      return  "AddPoint";
      break;
    case PlayAnimation:
      return  "PlayAnimation";
      break;
    case Delete:
      return  "Delete";
      break;
    case Saving:
      return  "Saving";
      break;
    case PickColor:
      return  "PickColor";
      break;
    case Help:
      return  "Help";
      break;
  }
  return "undef";
}

std::string UI::color_to_string(const sf::Color& color) {
  if (color == sf::Color::Green) return "Green";
  if (color == sf::Color::Red) return "Red";
  if (color == sf::Color::Blue) return "Blue";
  if (color == sf::Color::White) return "White";
  return std::to_string(color.r) + " " + 
    std::to_string(color.g) + " " +
    std::to_string(color.b) + " ";
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

void ImageElement::draw(sf::RenderWindow& _window, sf::RenderTarget& tooltip_targ,
        std::vector<std::string> selected, const InputState& input) {
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

  if(show_tooltip && tooltip.has_value()) {
    tooltip.value().draw(tooltip_targ, input);
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
void ImageElement::update(const InputState& input) {
  auto mpos = input.mouse_position;
  if(tooltip.has_value()) {
    sf::RectangleShape* tb = &background;
    if(mpos.x > tb->getPosition().x && mpos.x < tb->getPosition().x + tb->getSize().x && 
        mpos.y > tb->getPosition().y && mpos.y < tb->getPosition().y + tb->getSize().y) {
      show_tooltip = true;
    } else {
      show_tooltip = false;
    }
  } else show_tooltip = false;
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

void ImageElement::on_click(const InputState& input) {
  auto mpos = input.mouse_position;
  if (mpos.x >= position.x && mpos.x <= position.x + size.x &&
      mpos.y >= position.y && mpos.y <= position.y + size.y) {
    if(on_click_lam) {
      on_click_lam(); 
    }
  }  
}

void Container::on_click(const InputState& input) {
  for(const auto& child: children) {
    child->on_click(input);
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
  if(min_width > 0)
    my_size.x = std::max(my_size.x, min_width);
  if(min_height > 0)
    my_size.y = std::max(my_size.y, min_height);
  return my_size;
}

void Container::update(const InputState& input) {
  for(auto& child: children) {
    child->update(input);
  }
}

void Container::draw(sf::RenderWindow& _window,sf::RenderTarget& tooltip_targ, 
    std::vector<std::string> selected,
    const InputState& input) {
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
    elem->draw(_window, tooltip_targ, selected, input);
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

void Container::update_children_position() {
  Vec2f pos = position + Vec2f{padding.west, padding.north};
  for(auto& elem: children) {
    elem->set_position(pos);
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
    DrawingSettings& ds, State& st, AnimationState& animation_state): window{_window}, drawing_settings{ds},
    current_state{st}{
  if(!load_texture("assets/cursor.png") ||
     !load_texture("assets/start.png") ||
     !load_texture("assets/pause.png") ||
     !load_texture("assets/trashcan.png") ||
     !load_texture("assets/addpoint.png") ||
     !load_texture("assets/addcurve.png") ||
     !load_texture("assets/save.png") ||
     !load_texture("assets/size1.png") ||
     !load_texture("assets/size2.png") ||
     !load_texture("assets/size3.png") ||
     !load_texture("assets/size4.png") 
  ) {
    std::cout << "failed to load some .png" << std::endl;
  }
  if(!font.loadFromFile("assets/Roboto-Black.ttf")) {
    std::cerr << "failed to load font\n";
  }
  auto add_change_state_button = 
    [&input_handler, this] (std::unique_ptr<Container>& cont, State state,
        std::string filename) {
      auto elem = std::make_unique<ImageElement>(
            get_texture("assets/"+filename),
            [&input_handler, state]() { 
              input_handler.switch_to_state(state,"..."); 
            }, "state" );
      elem->tooltip = Tooltip(font);
      elem->tooltip->update_text("switching state");
      cont->add_elem(std::move(elem)); 
    };

  auto action_cont1 = std::make_unique<Container>();
  action_cont1->set_color(sf::Color::Cyan);
  action_cont1->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  action_cont1->set_orientation(Orientation::Vertical);
  add_change_state_button(action_cont1, State::Move, "cursor.png");
  add_change_state_button(action_cont1, State::AddCurve, "addcurve.png");
  add_change_state_button(action_cont1, State::AddPoint, "addpoint.png");
  add_change_state_button(action_cont1, State::Delete, "trashcan.png");

  auto action_cont2 = std::make_unique<Container>();
  action_cont2->set_color(sf::Color::Cyan);
  action_cont2->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  action_cont2->set_orientation(Orientation::Vertical);
  add_change_state_button(action_cont2, State::Saving, "save.png");
  add_change_state_button(action_cont2, State::PlayAnimation, "start.png");
  add_change_state_button(action_cont2, State::Move, "pause.png");
  action_cont2->add_elem(std::make_unique<ImageElement>("dupa"));

  auto color_picker = std::make_unique<UI::ColorPicker>(_window.getSize());

  auto color_container = std::make_unique<Container>();
  color_container->set_color(sf::Color::Cyan);
  color_container->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  color_container->set_orientation(Orientation::Vertical);
  auto add_color_button = [&color_container, this] (sf::Color col) {
    color_container->add_elem(std::make_unique<ImageElement>(
          col,
          [col, this]() {
            drawing_settings.selected_col = col;
          }, color_to_string(col)));
  };
  add_color_button(sf::Color::Green);
  add_color_button(sf::Color::Red);
  add_color_button(sf::Color::Blue);
  add_color_button(sf::Color::White);
  auto color_picker_preview = std::make_unique<UI::ColorPreview>(
        color_picker->r, color_picker->g, color_picker->b,
        [this, color_picker = color_picker.get()]() {
          drawing_settings.selected_col = color_picker->selected_color;
          });
  color_container->add_elem(std::move(color_picker_preview));

  auto size_container = std::make_unique<Container>();
  size_container->set_color(sf::Color::Cyan);
  size_container->set_padding(Padding{10.0, 10.0, 10.0, 10.0});
  size_container->set_orientation(Orientation::Vertical);
  auto add_size_button = [&size_container, this] (float thick, std::string filename) {
    size_container->add_elem(std::make_unique<ImageElement>(
          get_texture("assets/"+filename),
          [this, thick]() {
            drawing_settings.selected_thick = thick;
          }, std::to_string(thick)));
  };
  add_size_button(3.0, "size1.png");
  add_size_button(5.0, "size2.png");
  add_size_button(7.0, "size3.png");
  add_size_button(11.0, "size4.png");

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
  final_container->background.setOutlineThickness(2.0);
  final_container->background.setOutlineColor(sf::Color::Black);

  auto text_inp = std::make_unique<TextInput>(current_state, 
      Vec2f{(float)window.getSize().x/2, (float)window.getSize().y/2},
      animation_state);
  elements.push_back(std::move(text_inp));
  elements.push_back(std::move(final_container));

  for(const auto& elem: elements) {
    max_size.x = std::max(max_size.x, elem->calculate_size().x);
    max_size.y = std::max(max_size.y, elem->calculate_size().y);
  }

  elements.push_back(std::move(color_picker));
}

void Manager::drawUI(const InputState& input) {
  Vec2f window_size{window.getSize()};
  std::vector<std::string> selected;
  selected.push_back(color_to_string(drawing_settings.color));
  selected.push_back(std::to_string(drawing_settings.thickness));

  sf::RenderTexture tooltip_targ;
  if (!tooltip_targ.create(window.getSize().x, window.getSize().y)) {
    std::cerr << "Failed to create render texture" << std::endl;
    return;
  }
  tooltip_targ.clear(sf::Color::Transparent);

  for(auto& elem: elements) {
    if(check_if_elem_can_act(elem, current_state)) {
      elem->draw(window, tooltip_targ, selected, input);
    }
  }

  tooltip_targ.display();
  sf::Sprite sprite(tooltip_targ.getTexture());
  sprite.setColor(sprite.getColor());

  window.draw(sprite);
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

void Manager::handle_click(const InputState& input) {
  for(const auto& elem: elements) {
    if(check_if_elem_can_act(elem, current_state)) 
      elem->on_click(input); 
  }
}

void Manager::handle_input(sf::Event event) {
  for(const auto& elem: elements) {
    if(check_if_elem_can_act(elem, current_state)) 
      elem->handle_input(event); 
  }
}

void Manager::update(const InputState& input) {
  for(const auto& elem: elements) {
    if(check_if_elem_can_act(elem, current_state)) {
      elem->update(input);
    }
  }
}
      
bool Manager::check_if_elem_can_act(const std::unique_ptr<Element>& elem,
    const State& curr_state) {
  if(elem->required_state.has_value()) {
    return elem->required_state.value() == curr_state;
  }
  return true;
}
