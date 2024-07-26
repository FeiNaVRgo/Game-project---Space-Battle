#pragma once
#include "Stack.h"
#include <functional>

class UI {
	TreeNode<uint32_t, std::function<void()>> funcTree;

};

