#pragma once
#include <src\raylib-cpp.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

enum FUNC_ID {
	ID_MAIN_MENU,
	ID_PLAY_GAME,
	ID_SETTINGS,
	ID_EXIT,
	ID_SETTINGS_KEYBOARD,
	ID_SETTINGS_SOUND
};

struct Functions {
	static void MainMenu();
	static void Settings();
	static void SettingsKeyBoard();
	static void SettingsSound();
	static void PlayGame();
	static void Exit();
};

class Stack {
private:
	std::vector<uint32_t> mIdvec;
	std::unordered_map<uint32_t, std::function<void()>> mFuncMap;
public:
	//execute top element od stack
	void execute() {
		mFuncMap.at(mIdvec.front())();
	}

	//insert element at the top of stack
	void insert(std::function<void()> func, uint32_t id) {
		mIdvec.emplace_back(id);
		mFuncMap.try_emplace(id, func);
	}

	//push all functions
	void init() {
		insert(Functions::MainMenu, FUNC_ID::ID_MAIN_MENU);
		insert(Functions::Exit, FUNC_ID::ID_EXIT);
		insert(Functions::Settings, FUNC_ID::ID_SETTINGS);
		insert(Functions::PlayGame, FUNC_ID::ID_PLAY_GAME);
		insert(Functions::SettingsKeyBoard, FUNC_ID::ID_SETTINGS_KEYBOARD);
		insert(Functions::SettingsSound, FUNC_ID::ID_SETTINGS_SOUND);
	}

	void pushToTop(uint32_t id) {
		auto it1 = std::find(mIdvec.begin(), mIdvec.end(), id);
		std::swap(*it1, mIdvec.front());
	}
};

template<typename Kt, typename T>
struct TreeNode {
	using TreeNode = TreeNode<Kt, T>;

	Kt key;
	T myItem;

	std::vector<TreeNode*> children;

	static inline TreeNode* GetNewNode(Kt key, T item) {
		TreeNode* newNode = new TreeNode();
		newNode->key = key;
		newNode->myItem = item;

		return newNode;
	}

	static inline void addChild(TreeNode* node, Kt key, T item) { 
		TreeNode* newNode = GetNewNode(key, item);
		node->children.push_back(newNode);
	}
};