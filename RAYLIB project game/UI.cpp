#include "UI.h"
#include "globals.h"
#include "utils.h"
#include <src/raylib-cpp.hpp>
#include <iostream>

void UIFunctions::generalUI() {
	

	if (IsKeyPressed(KeyboardKey::KEY_Q)) {
		G::gUI.pushToStack(FUNC_UI_ID::ID_PAUSE);
		G::GameIsPlaying = false;
	}
}

void UIFunctions::pause() {
	
	auto pauseText = raylib::Text("GAME PAUSED", 30.0f, BLACK, GetFontDefault(), 2.0f);
	pauseText.Draw(raylib::Vector2(G::camera.target.x, G::camera.target.y) - raylib::CenterText(pauseText));

	if (IsKeyPressed(KeyboardKey::KEY_ESCAPE)) {//TODO: add a way to go back to main menu
		G::gUI.pushToStack(FUNC_UI_ID::ID_GENERAL_UI);
		G::GameIsPlaying = true;
	}
}
