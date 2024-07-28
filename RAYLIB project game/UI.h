#pragma once
#include <functional>
#include <stack>
#include <unordered_map>

enum FUNC_UI_ID {
	ID_NULL               = 0,
	ID_GENERAL_UI         = 1,
	ID_PAUSE              = 2
};

struct UIFunctions {
	static void generalUI();
	static void pause();
};

template<typename Key = uint32_t, typename Item = std::function<void()>>
struct ParentItem {//bad naming
	Key key;
	Item item;
};

template<typename Key = uint32_t, typename Item = std::function<void()>>
class UI {
	using pKey = Key;
private:
	std::unordered_map<Key, ParentItem<>> funcMap;
	std::stack<Key> keyStack;

	void initStack() {
		keyStack.push(FUNC_UI_ID::ID_GENERAL_UI);
	}

	void pushToMap(Key key, Item item, pKey parentKey) {
		funcMap.emplace(key, ParentItem(parentKey, item) );
	}
public:
	Key getKey() {
		return keyStack.top();
	}

	void pushToStack(Key key) {
		keyStack.pop();
		keyStack.push(key);
	}

	void init() {
		initStack();
		pushToMap(FUNC_UI_ID::ID_GENERAL_UI, UIFunctions::generalUI, FUNC_UI_ID::ID_NULL);
		pushToMap(FUNC_UI_ID::ID_PAUSE, UIFunctions::pause, FUNC_UI_ID::ID_GENERAL_UI);
	}

	void executeById(Key id) {
		if (id == 0) {
			return;
		}

		auto p = funcMap.at(id);
		p.item();
		executeById(p.key);
	}
};

