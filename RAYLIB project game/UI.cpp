#include "UI.h"
#include "globals.h"
#include "utils.h"
#include "FunctionalBox.h"
#include "Components.h"
#include <src/raylib-cpp.hpp>
#include <iostream>

#define INVENTORY_WIDTH 200

void UIFunctions::generalUI() {
	EndMode2D();

	const auto& health = G::gCoordinator.GetComponent<Health>(G::player);
	
	auto healthBox = FunctionalBox(
		raylib::Text("Health: " + std::to_string(health.health), 15.0f, BLACK, GetFontDefault(), 2.0f),
		raylib::Rectangle(0.0f, 0.0f, 200.0f, 30.0f),
		raylib::Color(RED)
	);

	healthBox.DrawTextBoxed(true);

	if (IsKeyPressed(KeyboardKey::KEY_ESCAPE)) {
		if (G::GameIsPlaying) {
			G::gUI.pushToStack(FUNC_UI_ID::ID_PAUSE);
			G::GameIsPlaying = false;
		}
		else {
			G::gUI.pushToStack(FUNC_UI_ID::ID_GENERAL_UI);
			G::GameIsPlaying = true;
		}
	}

	if (IsKeyPressed(KeyboardKey::KEY_E)) {
		if (G::gUI.getKey() == FUNC_UI_ID::ID_GENERAL_UI) {
			G::gUI.pushToStack(FUNC_UI_ID::ID_INVENTORY);
		}
		else {
			G::gUI.pushToStack(FUNC_UI_ID::ID_GENERAL_UI);
		}
	}

	BeginMode2D(G::camera);
}

void UIFunctions::pause() {
	auto pauseText = raylib::Text("GAME PAUSED", 30.0f, BLACK, GetFontDefault(), 2.0f);
	pauseText.Draw(raylib::Vector2(G::camera.target) - raylib::CenterText(pauseText));
}

void UIFunctions::inventory() {
	EndMode2D();
	raylib::Rectangle invBase(G::screenWidth - INVENTORY_WIDTH, 0, INVENTORY_WIDTH, G::screenHeight);
	invBase.Draw({0, 0, 0, 100});

	raylib::Rectangle core(raylib::getCenterRect(invBase) - raylib::Vector2{30, 30}, {60, 60});
	core.Draw({ 0, 0, 0, 50 });

	raylib::Rectangle staticWeapon1(raylib::getCenterRect(core) - raylib::Vector2{60, 80}, raylib::Vector2{40, 40});
	staticWeapon1.Draw({ 0, 0, 0, 50 });

	raylib::Rectangle staticWeapon2(raylib::getCenterRect(core) - raylib::Vector2{ -20, 80 }, raylib::Vector2{ 40, 40 });
	staticWeapon2.Draw({ 0, 0, 0, 50 });
	if (IsKeyPressed(KeyboardKey::KEY_R)) {
		if (G::gUI.getKey() == FUNC_UI_ID::ID_INVENTORY) {
			G::gUI.pushToStack(FUNC_UI_ID::ID_INVENTORY_STATS);
		}
		else {
			G::gUI.pushToStack(FUNC_UI_ID::ID_INVENTORY);
		}
	}

	BeginMode2D(G::camera);
}

void UIFunctions::inventoryStats() {
	EndMode2D();

	raylib::Rectangle r(0, 0, 10, 10);
	r.Draw(RED);

	BeginMode2D(G::camera);
}
