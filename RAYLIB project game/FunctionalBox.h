#pragma once
#include <src/raylib-cpp.hpp>
#include <string>
#include "globals.h"

class FunctionalBox {
private:
	raylib::Rectangle rect;
	raylib::Color colRect;
	std::string text;
	raylib::Color colText;
	uint8_t fontSize;
public:
	FunctionalBox(const raylib::Vector2& rPos, const raylib::Color& cRect, const std::string& text, const raylib::Color& cText, uint8_t fontSize) : colRect(cRect), text(text), colText(cText), fontSize(fontSize) {
		float scalefactor = fontSize / GetFontDefault().baseSize;
		rect.x = rPos.x;
		rect.y = rPos.y;
		rect.height = GetGlyphAtlasRec(GetFontDefault(), text.front()).height * scalefactor;
		for (auto const& t : text) {
			rect.width += GetGlyphAtlasRec(GetFontDefault(), t).width * scalefactor + GetGlyphInfo(GetFontDefault(), t).advanceX;
		}
	}

	bool update() {
		raylib::DrawText(text, rect.GetX(), rect.GetY(), fontSize, colText);
		if (rect.CheckCollision(GetScreenToWorld2D(GetMousePosition(), GLOBALS::camera))) {
			rect.DrawLines(colRect);
			return true;
		}
		return false;
	}

	void changeTextOnInput(uint16_t& _t) {
		int key = GetKeyPressed();

		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) { _t = MouseButton::MOUSE_BUTTON_LEFT; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT)) { _t = MouseButton::MOUSE_BUTTON_RIGHT; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_MIDDLE)) { _t = MouseButton::MOUSE_BUTTON_MIDDLE; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_SIDE)) { _t = MouseButton::MOUSE_BUTTON_SIDE; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_EXTRA)) { _t = MouseButton::MOUSE_BUTTON_EXTRA; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_FORWARD)) { _t = MouseButton::MOUSE_BUTTON_FORWARD; return; }
		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_BACK)) { _t = MouseButton::MOUSE_BUTTON_BACK; return; }

		while (key > 0) {
			_t = key;
			key = GetKeyPressed();
		}
	}
};

