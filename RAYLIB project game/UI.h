#pragma once
#include <src/raylib-cpp.hpp>
#include <memory>
#include <functional>
#include <stack>
#include <unordered_map>

#include "ECS.h"

enum class FUNC_UI_ID {
	ID_NULL               = 0,
	ID_GENERAL_UI         = 1,
	ID_PAUSE              = 2,
	ID_INVENTORY          = 3,
	ID_INVENTORY_STATS    = 4,
	ID_UPGRADES           = 5
};

struct UIFunctions {
	static void generalUI();
	static void pause();
	static void inventory();
	static void inventoryStats();
	static void procUpgrades();
};

template<typename Key = FUNC_UI_ID, typename Item = std::function<void()>>
struct ParentItem {//bad naming
	Key pKey;
	Item item;

	ParentItem(Key _pkey, Item _item) : pKey(_pkey), item(_item) {}
	ParentItem(Item _item, Key _pkey) : pKey(_pkey), item(_item) {}
};


template<typename Key = FUNC_UI_ID, typename Item = std::function<void()>>
class UI {
	using pKey = Key;
private:
	std::unordered_map<Key, ParentItem<>> funcMap;
	std::stack<Key> keyStack;
	Key prevKey;

	void initStack() {
		prevKey = FUNC_UI_ID::ID_NULL;
		keyStack.push(FUNC_UI_ID::ID_GENERAL_UI);
	}

	template<typename T, typename... Types>
	void pushToMap(Key key, Types... args) {
		funcMap.emplace(key, T(args...));
	}
public:
	void resetPreviousKey() {
		prevKey = FUNC_UI_ID::ID_NULL;
	}

	Key getPreviousKey() {
		return prevKey;
	}

	bool checkKey(Key key) {
		return getKey() == key;
	}

	Key getKey() {
		return keyStack.top();
	}

	ParentItem<> getItemByKey(Key key) {
		return funcMap.at(key);
	}

	void pushToStack(Key key) {
		prevKey = keyStack.top();
		keyStack.pop();
		keyStack.push(key);
	}

	void init() {
		initStack();
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_GENERAL_UI, UIFunctions::generalUI, FUNC_UI_ID::ID_NULL);
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_PAUSE, UIFunctions::pause, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_INVENTORY, UIFunctions::inventory, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_INVENTORY_STATS, UIFunctions::inventoryStats, FUNC_UI_ID::ID_INVENTORY);
	}

	void executeById(Key id) {
		if (id == FUNC_UI_ID::ID_NULL) {
			return;
		}

		auto p = funcMap.at(id);
		p.item();
		executeById(p.pKey);
	}
};

