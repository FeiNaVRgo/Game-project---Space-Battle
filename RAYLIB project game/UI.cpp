#include "UI.h"

#include "Components/Health.h"
#include "Components/Inventory.h"

#include "globals.h"
#include "utils.hpp"
#include "FunctionalBox.h"
#include "Components.h"
#include "ECS.h"

inline void UIFunctions::generalUI() {
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

inline void UIFunctions::pause() {
	auto pauseText = raylib::Text("GAME PAUSED", 30.0f, BLACK, GetFontDefault(), 2.0f);
	pauseText.Draw(raylib::Vector2(G::camera.target) - raylib::CenterText(pauseText));
}

inline void UIFunctions::inventory() {
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

inline void UIFunctions::inventoryStats() {
	EndMode2D();

	raylib::Rectangle r(0, 0, 10, 10);
	r.Draw(RED);

	BeginMode2D(G::camera);
}

inline void UIFunctions::procUpgrades() {

}

ParentItem::ParentItem(Key _pkey, Item _item) : pKey(_pkey), item(_item) {}
ParentItem::ParentItem(Item _item, Key _pkey) : pKey(_pkey), item(_item) {}

inline void UI::initStack() {
	prevKey = Key::ID_NULL;
	keyStack.push(Key::ID_GENERAL_UI);
}

UI::UI() {
	init();
}

inline void UI::resetPreviousKey() {
	prevKey = Key::ID_NULL;
}

inline UI::Key UI::getPreviousKey() const {
	return prevKey;
}

inline bool UI::checkKey(Key key) {
	return getKey() == key;
}

inline UI::Key UI::getKey() {
	return keyStack.top();
}

inline ParentItem UI::getItemByKey(Key key) {
	return funcMap.at(key);
}

inline void UI::pushToStack(Key key) {
	prevKey = keyStack.top();
	keyStack.pop();
	keyStack.push(key);
}

inline void UI::init() {
	initStack();
	pushToMap<ParentItem>(Key::ID_GENERAL_UI, ItemDef::generalUI, Key::ID_NULL);
	pushToMap<ParentItem>(Key::ID_PAUSE, ItemDef::pause, Key::ID_GENERAL_UI);
	pushToMap<ParentItem>(Key::ID_INVENTORY, ItemDef::inventory, Key::ID_GENERAL_UI);
	pushToMap<ParentItem>(Key::ID_INVENTORY_STATS, ItemDef::inventoryStats, Key::ID_INVENTORY);
}

void UI::executeById(Key id) {
	if (id == Key::ID_NULL) {
		return;
	}

	auto p = funcMap.at(id);
	p.item();
	executeById(p.pKey);
}