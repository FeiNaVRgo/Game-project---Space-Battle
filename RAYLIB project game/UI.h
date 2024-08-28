#pragma once
#include <src/raylib-cpp.hpp>
#include <memory>
#include <functional>
#include <stack>
#include <unordered_map>

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

struct ParentItem {//bad naming
	using Key = FUNC_UI_ID;
	using Item = std::function<void()>;

	Key pKey;
	Item item;

	ParentItem(Key _pkey, Item _item);
	ParentItem(Item _item, Key _pkey);
};

class UI {
public:
	using Key = FUNC_UI_ID;
	using ItemDef = UIFunctions;
	using Item = std::function<void()>;
	using pKey = Key;
private:
	std::unordered_map<Key, ParentItem> funcMap;
	std::stack<Key> keyStack;
	Key prevKey;

	void initStack();
	template<typename T, typename... Types>
	void pushToMap(Key key, Types... args);
public:
	UI();

	void resetPreviousKey();
	Key getPreviousKey() const;
	bool checkKey(Key key);
	Key getKey();
	ParentItem getItemByKey(Key key);
	void pushToStack(Key key);
	void init();
	void executeById(Key id);
};

template<typename T, typename... Types>
void UI::pushToMap(Key key, Types... args) {
	funcMap.emplace(key, T(args...));
}

