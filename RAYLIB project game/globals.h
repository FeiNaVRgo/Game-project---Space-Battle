#pragma once
#include <src\raylib-cpp.hpp>
#include <any>
#include "ECS.h"
#include "Timer.h"
#include "Stack.h"
#include <unordered_set>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

namespace KeyInputs{
	inline uint16_t SHOOT = MouseButton::MOUSE_BUTTON_LEFT;
	inline uint16_t FLY = KeyboardKey::KEY_W;
}

namespace G {
	inline Camera2D camera = { 0 };
	inline raylib::Rectangle screen = { 0,0,800,450 };
	const raylib::Rectangle gridRect = { 0.0f, 0.0f, 5760, 5760 };
	constexpr int screenWidth = 800;
	constexpr int screenHeight = 450;
	inline raylib::Window window(screenWidth, screenHeight, "SPACE BATTLE");
	inline TimerContainer gTimer;
	inline ECS::Coordinator gCoordinator;
	inline ECS::Coordinator upgradeCoordinator;
	inline bool debugMode = false;
	inline uint32_t gEnemyCounter = 0;
	inline uint32_t gLevel = 0;
	inline raylib::Vector2 gPlayerPos;
	inline bool GameIsPlaying = false;
	inline Stack gStack;
	inline std::any physicsSystem;
	inline std::any renderSystem;
	inline std::any inputSystem;
	inline std::any healthSystem;
	inline std::any enemySpawningSystem;
	inline std::any enemyAIMovmentSystem;
	inline std::any bulletManipulationSystem;
	inline std::any collisionSystem;
	inline std::any entityRemovalSystem;
	inline int zoomMode = 0;
	inline bool exitWindow = false;
	inline raylib::Rectangle playerBoundingBoxForBullets;
	inline std::unordered_set<ECS::Entity> gEntitySetToBeDestroyed;
	inline raylib::Shader gBloom = raylib::Shader(0, TextFormat("resources/shaders/glsl100/bloom.glsl", GLSL_VERSION));
	#pragma region loadingTextures
	inline raylib::Texture2DUnmanaged playerTexture("resources/PlayerModel.png");
	inline raylib::Texture2DUnmanaged enemyTexture("resources/EnemyModel.png");
	inline raylib::Texture2DUnmanaged playerBulletTexture("resources/PlayerBulletModel.png");
	#pragma endregion texture unmanaged unload the yourself
}
