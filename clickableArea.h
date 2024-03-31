#ifndef CLICKABLEAREA_H
#define CLICKABLEAREA_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
using namespace sf;

struct clickableArea {
    IntRect rect;
    string name;
};

#endif