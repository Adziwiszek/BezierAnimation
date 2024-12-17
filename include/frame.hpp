#pragma once

#include <SFML/Graphics.hpp>
#include "bcurve.hpp"
#include <vector>
#include <memory>

using Curves = std::vector<std::shared_ptr<BCurve>>;

class Frame {
private:
  Curves curves;
public:

};
