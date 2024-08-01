#pragma once
#include <src/raylib-cpp.hpp>
#include <memory>
#include <functional>
#include <stack>
#include <unordered_map>

#include "ECS.h"

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
	Key pKey;
	Item item;

	ParentItem(Key _pkey, Item _item) : pKey(_pkey), item(_item) {}
	ParentItem(Item _item, Key _pkey) : pKey(_pkey), item(_item) {}
};

struct InvItem : ParentItem<> {
	using Key = uint32_t;
	using Item = std::function<void()>;

	enum class SLOT_PURPOSE_ID {
		KEEPING       = 0,
		ACTIVE        = 1
	};

	struct InventorySlot {
		ECS::Entity itemToKeep;
		raylib::Rectangle rectSlot;
		uint16_t purposeId;
 	};

	void DragAndPut() {
		//will need to implement upgrades now for fucks sake
		
	}

	InvItem(Item _item, Key _pkey) : ParentItem(_item, _pkey) {}
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

	template<typename T, typename... Types>
	void pushToMap(Key key, Types... args) {
		funcMap.emplace(key, std::make_shared<T>(args...));
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
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_GENERAL_UI, UIFunctions::generalUI, FUNC_UI_ID::ID_NULL);
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_PAUSE, UIFunctions::pause, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap<ParentItem<>>(FUNC_UI_ID::ID_INVENTORY, UIFunctions::inventory, FUNC_UI_ID::ID_GENERAL_UI);
		pushToMap<InvItem>(FUNC_UI_ID::ID_INVENTORY_STATS, UIFunctions::inventoryStats, FUNC_UI_ID::ID_INVENTORY);
	}

	void executeById(Key id) {
		if (id == 0) {
			return;
		}

		auto p = funcMap.at(id);
		p->item();
		executeById(p->pKey);
	}
};

