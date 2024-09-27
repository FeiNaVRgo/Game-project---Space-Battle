#pragma once
#include <src/raylib-cpp.hpp>
#include <random>
#include <vector>
#include <any>
#include <numbers>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <magic_enum_all.hpp>
#include "ECS.h"

struct Transforms;
struct RigidBody;
struct Health;
struct Bullet;
struct EntitySpecific;
struct TimerComponent;
struct Damage;
struct Inventory;
struct WeaponMini;
struct PlayerSpecific;

class Sprite;
class MovmentAI;

enum class ID_WEAPON;
enum class ID_WEAPON_RARITY;

#define INVENTORY_WIDTH 200

static std::random_device rd;
static std::mt19937 gen(rd());

struct Parent {
	std::unordered_map<ECS::Entity, std::vector<std::any>> childMap;

	template<typename T, typename... Args>
	void push_back_vec(std::vector<T>& v, Args&&... args) {
		static_assert((std::is_constructible_v<T, Args&&> && ...));
		(v.push_back(std::forward<Args>(args)), ...);
	}

	template<typename... Types>
	void passArgsToChildern(Types... args) {
		for (auto& [e, anyVec] : childMap) {
			push_back_vec(anyVec, args);
		}
	}
};

struct Child {

};

namespace ENTITY_CREATION_FUNCTIONS {
	//parent, pos, hitbox - dimensions, sprite, angle, maxHealth, damage
	//maybe setup for multishoot
	extern void createBullet(ECS::Entity parent,
		Transforms      const& transforms,
		RigidBody       const& rigidBody,
		Sprite          const& sprite,
		Health          const& health,
		Bullet          const& bullet,
		EntitySpecific  const& entitySpecific,
		TimerComponent  const& timerComponent,
		Damage          const& damage,
		MovmentAI       const& movmentAI);
}

struct PhysicsSystem : public ECS::System {
public:
	void update(float dt);
};

struct RenderSystem : public ECS::System {
public:
	void updateSprites();
};

struct WeaponLibrary;
struct WeaponSystem : ECS::System {
	using WeaponmMini       = ECS::Entity;

	void update();
	
	void weaponInvVibeCheck(Inventory const& inv, WeaponLibrary const& weaponLibrary);
};

struct WeaponLibrary {
	using CreateMiniFunc =   std::function<void()>;
	using CreateNormalFunc = std::function<void(Inventory&, WeaponMini&)>;
	using WeaponBehaviour =  std::function<void(ECS::Entity)>;
	
	template<typename T>
	void insertToLib();

	void populateRaToWe();

	std::unordered_map<ID_WEAPON_RARITY, std::vector<ID_WEAPON>> rarityToWeaponsMap;

	std::unordered_map<ID_WEAPON, CreateMiniFunc> weaponMiniCreationMap;
	std::unordered_map<ID_WEAPON, CreateNormalFunc> weaponNormalCreationMap;
	std::unordered_map<ID_WEAPON, WeaponBehaviour> weaponBehaviourMap;

	WeaponLibrary();
};

struct WeaponHandOut : public ECS::System {
	void update();
};

struct InputSystem : public ECS::System {
private:
	void breaksVelocity(RigidBody& rigidBody);
	void shoot(ECS::Entity entity ,Transforms const& transform, RigidBody const& rigidBody, Sprite const& sprite, TimerComponent& timer);
	void fly(Transforms const& transform, RigidBody& rigidBody, Sprite& sprite, PlayerSpecific const& pSpecific);
public:
	void update();
};

struct HealthSystem : public ECS::System {
	void update();
};

struct CollisionSystem : public ECS::System {
	void update();
};

struct EnemySpawningSystem : public ECS::System{
	void update();
};

struct EnemyAIMovmentSystem : ECS::System {
	void update();
};

struct BulletManipulationSystem : ECS::System {
	void update();
};


struct EntityRemovalSystem : ECS::System {
	void update();
};
