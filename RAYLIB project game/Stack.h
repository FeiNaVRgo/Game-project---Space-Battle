#pragma once
#include <src/raylib-cpp.hpp>
#include <src/rlgl.h>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>


enum FUNC_ID {
	ID_MAIN_MENU,
	ID_PLAY_GAME,
	ID_SETTINGS,
	ID_EXIT,
	ID_SETTINGS_KEYBOARD,
	ID_SETTINGS_SOUND
};

struct StackFunctions {
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
	Stack();

	//execute top element od stack
	void execute();
	//insert element at the top of stack
	void insert(std::function<void()> func, uint32_t id);
	//push all functions
	void init();
	void pushToTop(uint32_t id);
};