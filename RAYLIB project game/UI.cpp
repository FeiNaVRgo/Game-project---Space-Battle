#include "UI.h"
#include "globals.h"
#include "utils.hpp"
#include "FunctionalBox.h"
#include "Components.h"
#include <src/raylib-cpp.hpp>

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

	if (IsKeyPressed(KeyInputs::INVENTORY)) {
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
	auto& inv = G::gCoordinator.GetComponent<Inventory>(G::player);

	inv.DrawAllSlots();
	inv.DrawSprites();
	inv.InteractWithSlots();

	if (IsKeyPressed(KeyboardKey::KEY_R)) {
		using enum FUNC_UI_ID;
		if (G::gUI.getKey() == ID_INVENTORY) {
			G::gUI.pushToStack(ID_INVENTORY_STATS);
		}
		else {
			G::gUI.pushToStack(ID_INVENTORY);
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

void UIFunctions::procUpgrades() {

}
