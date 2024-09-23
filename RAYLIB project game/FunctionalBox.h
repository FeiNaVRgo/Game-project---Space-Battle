#pragma once
#include <src/raylib-cpp.hpp>
#include <string>
#include "globals.h"

class FunctionalBox {
public:
	raylib::Rectangle rect;
    raylib::Color colRect;
    raylib::Text text;
public:
    FunctionalBox(const raylib::Vector2& pos, const raylib::Color& colRect, const raylib::Text& text);
    FunctionalBox(const raylib::Text& text, const raylib::Rectangle& rect, const raylib::Color& colRect);
    FunctionalBox() = default;

    bool update();
    void changeTextOnInput(uint16_t& _t);
    void DrawTextBoxed(bool wordWrap);
    static void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
    static void DrawTextBoxed(Font font, std::string text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
};
