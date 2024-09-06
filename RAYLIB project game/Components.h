#pragma once
#include <src/raylib-cpp.hpp>
#include <random>
#include <vector>
#include <any>
#include <numbers>
#include <iostream>
#include <algorithm>
#include <functional>
#include "ECS.h"
#include "globals.h"
#include "Timer.h"
#include "spatialHash/grid.h"
#include "GeometryCalc.hpp"
#include "utils.hpp"
#include "Coroutines.h"
#include "EntityIds.h"
#include "./Weapons/Weapons.h"


#define INVENTORY_WIDTH 200

static std::random_device rd;
static std::mt19937 gen(rd());

class ComponentCommons {
public:
	template<typename Component, typename... Types>
	static inline void addComponent(ECS::Entity entity, Types... args) {
		G::gCoordinator.AddComponent<Component>(entity, Component(args...));
	}
};

class Hitbox {
public:
	raylib::Rectangle hitboxRect;
	raylib::Color hitboxColor;

	raylib::Vector2 getHitBoxCenter();
};

template<typename T>
struct Side {
	T isLeft;//1
	T isRight;//2
	T isTop;//3
	T isBottom;//4
};

struct RigidBody {
	raylib::Vector2 velocity;//1
	raylib::Vector2 acceleration;//2
	Hitbox hitbox;//3
	bool isColliding;//4
	Side<bool> onWhatSideIsColliding;//5
};

struct Transforms {
	raylib::Vector2 position;//1
	raylib::Vector2 rotation;//2
	raylib::Vector2 scale;//3
};

struct Sprite {
	raylib::Texture2DUnmanaged sprite;//1
	float angle;//2
	raylib::Color tint;//3
	raylib::Vector2 origin;//4
	bool isDependent = false;//5

	static void draw(ECS::Entity entity);

	~Sprite();
};

struct Health {
	float maxHealth;//1
	float health;//2
	bool isDamaged;//3
	uint16_t frameImmunityTime;//4
	float healthToSubstract;//5
	bool toBeDamaged;//6
	
	void drawHealthBar(const raylib::Vector2& origin, const raylib::Vector2& offsetFromOrigin);

	void drawDamageIndicator();
};

struct PlayerSpecific {
	float dash;//1
};

struct Inventory {
	enum class SLOT_IMPL {
		ACTIVE = 0,
		NONACTIVE = 1
	};

	enum class SLOT_STATE {
		STATE_EMPTY = 0,
		STATE_OCCUPIED = 1
	};

	enum class SLOT_PURPOSE {
		SLOT_CORE         = 0,
		SLOT_WEAPON       = 1,
		SLOT_AMMO         = 2,
		SLOT_IVNENTORY    = 3
	};

	struct SlotDef {
		SLOT_IMPL slotImpl{};
		SLOT_STATE slotState{};
		SLOT_PURPOSE slotPurpuse{};

		raylib::Vector2 dimensions{};
		raylib::Color slotCol{};

		std::shared_ptr<ECS::Entity> ptrItem{};

		raylib::Vector2 position{};
	};
	
	std::vector<SlotDef> allSlots;
	uint16_t invSize = 0;
	uint16_t weaponSize = 0;
	uint16_t ammoSize = 0;

	raylib::Rectangle inventoryBase{ G::screenWidth - INVENTORY_WIDTH, 0, INVENTORY_WIDTH, G::screenHeight };

	Inventory();
private:
	void populateWeapons();
	void populateInventory();
	void populateAllSlots();
	void addSlotInventory();
	void addSlotWeapon();
	void DrawSlot(const SlotDef& slot);
	void DrawSlotOutline(const SlotDef& slot);
	void DrawVecSlot(const std::vector<SlotDef>& vecSlot, bool drawOutline, bool drawSlot = true);
	void SetSlotsPos();
	void InteractWithSlot(SlotDef& slot);
	void swapItemSlots(SlotDef& slot1, SlotDef& slot2);
	void copySlotPtr(SlotDef& slot, std::shared_ptr<SlotDef> target);
	void copySlotPtr(SlotDef& slot, SlotDef* target);
	void copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<std::shared_ptr<SlotDef>>& target);
	void copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<SlotDef*>& target);
public:
	void moveUptrItem(std::vector<SlotDef>& vecSlot, ECS::Entity entity);
	void DrawSprites();
	void DrawAllSlots();
	void InteractWithSlots();
	void updateSlots();
};

struct Bullet {
	//uint8_t id;
};

struct Enemy {
};

/**
* @brief damage - for ...onContact when entity creates a new one
* @brief damageOnContact
*/
struct Damage {
	enum class DAMAGE_TYPE {
		PHYSICAL         = 0,
		EXPLOSIVE        = 1,
		PENETRATION      = 2,
		FIRE             = 3,
		LIGHTNING        = 4,
		POISON           = 5,
		RADIANT          = 6,
		COLD             = 7,
		ENERGY           = 8
	};

	float damage;//1
	float damageOnContact;//2
	DAMAGE_TYPE damageType{};//3
};

struct upgDebuff {
	enum class DEBUFF_TYPES {
		BLEED            = 0,
		BURN             = 1,
		CHILL            = 2,
		FROSTBITE        = 3,
		GLITTER          = 4,
		JUDGMENT         = 5,
		NECROSIS         = 6,
		NETTLES          = 7,
		POSION           = 8,
		RUPTURE          = 9,
		SCORCH           = 10,
		SHOCK            = 11,
		TOXIC            = 12,
		ZAP              = 13
	};

	bool isDamageOverTime;
	uint32_t damageOverTime;
};

struct Debuffs : upgDebuff {
	struct debuffProck {
		DEBUFF_TYPES type;
		uint32_t prock;
	};

	std::vector<debuffProck> debuffVec;
	
	void checkDebuffs(){
		//TODO
	}
};

struct Fuel {
	uint16_t fuelAmount;
	uint8_t fuelUsagePerSecond;
};

struct ChancesOrPercentages {
	uint8_t criticalChance;
	uint8_t accuracy;
	uint8_t luckyHit;
	uint8_t luck;
};

struct TimerComponent {
	TimerContainer timerCont;
};

struct EntitySpecific {
	uint8_t id;
};

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

struct JustBorn {
	ECS::Entity parent;//1
	raylib::Vector2 bornPos;//2
	raylib::Vector2 bornVel;//3
	float bornAngle;//4
	double bornTime;//5
};

struct MovmentAI {
	bool DistanceH;
	bool DistanceW;


	void chase(const raylib::Vector2& pos, const raylib::Vector2& posToChase, raylib::Vector2& velocity, float& angle, float speed, float seeRadius = std::numeric_limits<float>::infinity());

	/**
	* @brief a * sin (x * PI / b)
	* @param params -  0 for b, 1 for a
	*/
	static double sinwaveExactFunc(double x, std::vector<float> params);

	/**
	* @brief y = a * sin (xb)
	* @param params -  0 for b, 1 for a
	*/
	static double sinwaveFunc(double x, std::vector<float> params);

	/**
	* @brief y = ax^2 + bx
	* @param params - 0 for a, 1 for b
	*/
	static double parabolicFunc(double x, std::vector<float> params);

	/**
	* @brief y = ax^2 + bx
	* @param params -  0 for T, 1 for H
	*/
	static double parabolicExactFunc(double x, std::vector<float> params);
	
	static double straightLineFunc(double x, std::vector<float> params);

	/**
	* @brief makes bullet follow path of given function
	*/
	void moveInLineOfFunc(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f);

	/**
	* @brief makes bullet follow certain path
	* @brief first argument of params must by maxX
	* @brief still in development
	* TODO
	*/
	void moveInLineOfFuncAndGoBack(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f, raylib::Vector2 whereToDestroy);
};

struct UpgradeSystem : public ECS::System {

};

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

	void weaponInvVibeCheck(Inventory& inv, WeaponLibrary const& weaponLibrary);
	
	static void createWeaponNormalCanon();
	static void createWeaponMiniCanon();
};

struct WeaponLibrary {
	using CreateNormalFunc = std::function<void()>;
	using WeaponBehaviour = std::function<void(ECS::Entity)>;
	
	std::unordered_map<ID_WEAPON, CreateNormalFunc> weaponMap;
	std::unordered_map<ID_WEAPON, WeaponBehaviour> weaponBehaviourMap;

	WeaponLibrary();
};

struct InputSystem : public ECS::System {
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
