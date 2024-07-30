#pragma once
#include <memory>
#include <functional>
#include <stack>
#include <unordered_map>

enum FUNC_UI_ID {
	ID_NULL               = 0,
	ID_GENERAL_UI         = 1,
	ID_PAUSE              = 2,
	ID_INVENTORY          = 3,
	ID_INVENTORY_STATS    = 4
};

struct UIFunctions {
	static void generalUI();
	static void pause();
	static void inventory();
	static void inventoryStats();
};

template<typename Key = uint32_t, typename Item = std::function<void()>>
struct ParentItem {//bad naming
	Key key;
	Item item;

	ParentItem(Key _key, Item _item) : key(_key), item(_item) {}
	ParentItem(Item _item, Key _key) : key(_key), item(_item) {}
};

struct InvItem : ParentItem<> {
	
};

template<typename Key = uint32_t, typename Item = std::function<void()>>
class UI {
	using pKey = Key;
private:
	std::unordered_map<Key, std::shared_ptr<ParentItem<>>> funcMap;
	std::stack<Key> keyStack;
	Key prevKey;

	void initStack() {
		prevKey = ID_NULL;
		keyStack.push(FUNC_UI_ID::ID_GENERAL_UI);
	}

	template<typename... Types>
	void pushToMap(Key key, Types... args) {
		funcMap.emplace(key, std::make_shared<ParentItem<>>(args...));
	}
public:
	void resetPreviousKey() {
		prevKey = ID_NULL;
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

	std::shared_ptr<ParentItem<>> getItemByKey(Key key) {
		return funcMap.at(key);
	}

	void pushToStack(Key key) {
		prevKey = keyStack.top();
		keyStack.pop();
		keyStack.push(key);
	}

	void init() {
		initStack();
		pushToMap(FUNC_UI_ID::ID_GENERAL_UI, UIFunctions::generalUI, FUNC_UI_ID::ID_NULL);
		pushToMap(FUNC_UI_ID::ID_PAUSE, UIFunctions::pause, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap(FUNC_UI_ID::ID_INVENTORY, UIFunctions::inventory, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap(FUNC_UI_ID::ID_INVENTORY_STATS, UIFunctions::inventoryStats, FUNC_UI_ID::ID_INVENTORY);
	}

	void executeById(Key id) {
		if (id == 0) {
			return;
		}

		auto p = funcMap.at(id);
		p->item();
		executeById(p->key);
	}
};

