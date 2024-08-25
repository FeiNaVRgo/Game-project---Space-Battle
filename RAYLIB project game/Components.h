#pragma once
#include <src/raylib-cpp.hpp>
#include <random>
#include <vector>
#include <any>
#include <numbers>
#include <iostream>
#include "ECS.h"
#include "globals.h"
#include "Timer.h"
#include "utils.h"
#include "spatialHash/grid.h"
#include "GeometryCalc.h"
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

	raylib::Vector2 getHitBoxCenter() {
		return hitboxRect.GetPosition() + hitboxRect.GetSize() / 2.0f;
	}
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
	
	~Sprite() {
		//sprite.Unload();
	}
};

struct Health {
	float maxHealth;//1
	float health;//2
	bool isDamaged;//3
	uint16_t frameImmunityTime;//4
	float healthToSubstract;//5
	bool toBeDamaged;//6
	
	void drawHealthBar(const raylib::Vector2& origin, const raylib::Vector2& offsetFromOrigin) {
		raylib::Rectangle r1 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(maxHealth, 4) };
		raylib::Rectangle r2 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(health, 4) - raylib::Vector2(1, 1)};
		r2.Draw(BLUE);
		r1.DrawLines(BLACK);
	}

	void drawDamageIndicator() {
		//task for particle system
	}
};

struct PlayerSpecific {
	float dash;//1
};

struct Inventory {
	enum class SLOT_IMPL {
		ACTIVE         = 0,
		NONACTIVE      = 1
	};

	enum class SLOT_STATE {
		STATE_EMPTY		= 0,
		STATE_OCCUPIED  = 1
	};

	struct SlotDef {
		SLOT_IMPL slotImpl{};
		SLOT_STATE slotState{};
		
		raylib::Vector2 dimensions{};
		raylib::Color slotCol{};

		std::shared_ptr<ECS::Entity> uptrItem{};

		raylib::Vector2 position{};
	};


	SlotDef slotCore{ SLOT_IMPL::ACTIVE, SLOT_STATE::STATE_EMPTY, {30.f, 30.f}, {0, 0, 0, 50}, nullptr, { 0.f, 0.f } };
	std::vector<SlotDef> slotsWeapon{};
	std::vector<SlotDef> slotsAmmo{};
	std::vector<SlotDef> slotsInv{};
	
	raylib::Rectangle inventoryBase{ G::screenWidth - INVENTORY_WIDTH, 0, INVENTORY_WIDTH, G::screenHeight };

	Inventory() {
		populateWeapons();
		populateInventory();

		SetSlotsPos();
	}
private:
	void populateWeapons() {
		for (int i = 0; i < 4; i++) {
			addSlotWeapon();
		}
	}

	void populateInventory() {
		for (int i = 0; i < 12; i++) {
			addInventorySlot();
	    }
	}

	void addInventorySlot() {
		slotsInv.push_back({ SLOT_IMPL::NONACTIVE, SLOT_STATE::STATE_EMPTY, { 30.0f, 30.0f }, { 0, 0, 0, 50 }, nullptr, { 0.f, 0.f } });
	}

	void addSlotWeapon() {
		slotsWeapon.push_back({ SLOT_IMPL::ACTIVE, SLOT_STATE::STATE_EMPTY, { 30.f, 30.f }, { 0, 0, 0, 50 }, nullptr, { 0.f, 0.f } });
	}
	
	void DrawSlot(const SlotDef& slot) {
		raylib::Rectangle(slot.position, slot.dimensions).Draw(slot.slotCol);
	}

	void DrawSlotOutline(const SlotDef& slot) {
		raylib::Rectangle(slot.position, slot.dimensions).DrawLines(slot.slotCol);//TODO --diffrent colors for outline
	}

	void DrawVecSlot(const std::vector<SlotDef>& vecSlot, bool drawOutline, bool drawSlot = true) {
		for (const auto& slot : vecSlot) {
			if (drawSlot) {
				DrawSlot(slot);
			}

			if (drawOutline) {
				DrawSlotOutline(slot);
			}
		}
	}

	void SetSlotsPos() {
		uint16_t r = 70.0f;
		slotCore.position = raylib::getCenterRect(inventoryBase) - slotCore.dimensions / 2.0f;

		for (auto it = slotsWeapon.begin(); it != slotsWeapon.end(); ++it) {
			auto i = std::distance(slotsWeapon.begin(), it);
			float angle = DEG2RAD * (static_cast<float>(i) * 360.0f / static_cast<float>(slotsWeapon.size()));
		
			it->position = raylib::Vector2{ std::floor(r * cosf(angle)), std::floor(r * sinf(angle)) } + raylib::getCenterRect(inventoryBase) - it->dimensions / 2.0f;
		}

		float totalWidth = 0.f;
		float y = 0.f;//for the current level of lines in inventory
		float maxWidth = 0.f;//for later centering of position in invBase
		for (auto it = slotsInv.begin(); it != slotsInv.end(); ++it) {
			//auto i = std::distance(slotsInv.begin(), it); //just in case when i want to change something
			
			if (totalWidth + it->dimensions.x >= INVENTORY_WIDTH) {//without additiion it fucks up
				maxWidth = totalWidth;
				totalWidth = 0.0f;
				y++;
			}
			
			it->position = { totalWidth, y * it->dimensions.y };
			totalWidth += it->dimensions.x;
		}

		for (auto& slot : slotsInv) {
			slot.position.x += inventoryBase.GetPosition().x + (INVENTORY_WIDTH - maxWidth) / 2.0f;
			slot.position.y += slotCore.position.y + r + 60.f;
		}
	}

	void interactWithSlot(SlotDef slot) {
		if (raylib::Rectangle(slot.position, slot.dimensions).CheckCollision(GetMousePosition()) && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
			if (slot.uptrItem != nullptr) {
				auto& miniWeapon = G::gCoordinator.GetComponent<WeaponMini>(*slot.uptrItem);
				miniWeapon.isSelected = true;

				slot.uptrItem = nullptr;
			}
		}
	}
public:
	void DrawSprites() {
		for (auto& slot : slotsInv) {
			if (slot.uptrItem != nullptr) {
				auto sprite = G::gCoordinator.GetComponent<Sprite>(*slot.uptrItem);
				auto transform = G::gCoordinator.GetComponent<Transforms>(*slot.uptrItem);

				EndMode2D();

				sprite.sprite.Draw(
					raylib::Rectangle(0.0f, 0.0f, sprite.sprite.width, sprite.sprite.height),
					raylib::Rectangle(transform.position.x, transform.position.y, sprite.sprite.width, sprite.sprite.height),
					raylib::Vector2(sprite.origin),
					sprite.angle * RAD2DEG,
					sprite.tint
				);

				BeginMode2D(G::camera);
			}
		}
	}

	void DrawAllSlots() {
		if (G::debugMode) {
			auto v = raylib::getCenterRect(inventoryBase);
			DrawCircleLines(v.x, v.y, 70.0f, RED);
		}

		inventoryBase.Draw({ 0, 0, 0, 100 });
		DrawSlot(slotCore);
		DrawVecSlot(slotsWeapon, false);
		DrawVecSlot(slotsInv, true);
	}

	void interactWithSlots() {
		interactWithSlot(slotCore);
		for (auto& slot : slotsWeapon) {
			interactWithSlot(slot);
		}
		for (auto& slot : slotsInv) {
			interactWithSlot(slot);
		}
	}
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


	void chase(const raylib::Vector2& pos, const raylib::Vector2& posToChase, raylib::Vector2& velocity, float& angle, float speed, float seeRadius = std::numeric_limits<float>::infinity()) {
		float distance = Vector2Distance(pos, posToChase);
		if (distance <= seeRadius) {
			raylib::Vector2 v = { (posToChase.x - pos.x) / distance,
								  (posToChase.y - pos.y) / distance };
			angle = atan2f(v.x, -v.y);

			velocity.x = sinf(angle) * speed;
			velocity.y = -cosf(angle) * speed;
		}
		else {
			velocity = { 0.0f, 0.0f };
		}
	}

	/**
	* @brief a * sin (x * PI / b)
	* @param params -  0 for b, 1 for a
	*/
	static inline double sinwaveExactFunc(double x, std::vector<float> params) {
		return params.at(1) * sinf(x * (PI / params.at(0)));
	}

	/**
	* @brief y = a * sin (xb)
	* @param params -  0 for b, 1 for a
	*/
	static inline double sinwaveFunc(double x, std::vector<float> params) {
		return params.at(1) * sinf(static_cast<float>(x) * params.at(0));
	}

	/**
	* @brief y = ax^2 + bx
	* @param params - 0 for a, 1 for b
	*/
	static inline double parabolicFunc(double x, std::vector<float> params) {
		return params.at(0) * x * x + params.at(1) * x;
	}

	/**
	* @brief y = ax^2 + bx
	* @param params -  0 for T, 1 for H
	*/
	static inline double parabolicExactFunc(double x, std::vector<float> params) {
		float T = params.at(0);
		float H = params.at(1);

		double c = T / 2.0f;
		double a = H / (c * c);
		double b = H;

		return -(a * (x - c) * (x - c)) + b;
	}

	static inline double straightLineFunc(double x, std::vector<float> params) {
		return params.at(0) * x;
	}

	/**
	* @brief makes bullet follow path of given function
	*/
	void moveInLineOfFunc(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f) {
		auto& born = G::gCoordinator.GetComponent<JustBorn>(anchor);
		auto& transform = G::gCoordinator.GetComponent<Transforms>(anchor);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(anchor);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(anchor);

		auto vel2 = born.bornVel.Rotate(-PI / 2.0f);
		auto vel3 = born.bornVel.Rotate(PI / 2.0f);

		raylib::Ray rayBorn1(Vector3{ born.bornPos.x, born.bornPos.y, 0 }, Vector3{ born.bornVel.x, born.bornVel.y, 0.0f });
		raylib::Ray rayBorn2(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel2.x, vel2.y, 0.0f });
		raylib::Ray rayBorn3(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel3.x, vel3.y, 0.0f });
		
		if (G::debugMode) {
			rayBorn1.Draw(RED);
			rayBorn2.Draw(RED);
			rayBorn3.Draw(RED);
		}
		
		auto translatedX1 = raylib::intersectsRayToRay(rayBorn1, rayBorn2);
		auto translatedX2 = raylib::intersectsRayToRay(rayBorn1, rayBorn3);

		if (!translatedX1.empty()) {
			raylib::Vector2 vecTX{ translatedX1.front().x, translatedX1.front().y };
			if (G::debugMode)
				vecTX.DrawCircle(10, BLUE);

			auto x = Vector2Distance(vecTX, born.bornPos);
			sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

			rigidBody.velocity.x = sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
		else if (!translatedX2.empty()) {
			raylib::Vector2 vecTX{ translatedX2.front().x, translatedX2.front().y };
			if (G::debugMode)
				vecTX.DrawCircle(10, BLUE);

			auto x = Vector2Distance(vecTX, born.bornPos);
			sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

			rigidBody.velocity.x = sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
	}

	/**
	* @brief makes bullet follow certain path
	* @brief first argument of params must by maxX
	* @brief still in development
	* TODO
	*/
	void moveInLineOfFuncAndGoBack(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f, raylib::Vector2 whereToDestroy) {
		auto& born = G::gCoordinator.GetComponent<JustBorn>(anchor);
		auto& transform = G::gCoordinator.GetComponent<Transforms>(anchor);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(anchor);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(anchor);

		auto vel2 = born.bornVel.Rotate(-PI / 2.0f);
		auto vel3 = born.bornVel.Rotate(PI / 2.0f);

		raylib::Ray rayBorn1(Vector3{ born.bornPos.x, born.bornPos.y, 0 }, Vector3{ born.bornVel.x, born.bornVel.y, 0.0f });
		raylib::Ray rayBorn2(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel2.x, vel2.y, 0.0f });
		raylib::Ray rayBorn3(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel3.x, vel3.y, 0.0f });

		rayBorn1.Draw(RED);
		rayBorn2.Draw(RED);
		rayBorn3.Draw(RED);

		auto translatedX1 = raylib::intersectsRayToRay(rayBorn1, rayBorn2);
		auto translatedX2 = raylib::intersectsRayToRay(rayBorn1, rayBorn3);

		if (!translatedX1.empty()) {
			raylib::Vector2 vecTX{ translatedX1.front().x, translatedX1.front().y };
			vecTX.DrawCircle(10, BLUE);

			auto x = Vector2Distance(vecTX, born.bornPos);
			sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

			if (x < params.at(0) && transform.position.x != 9) {
				rigidBody.velocity.x = sinf(sprite.angle) * speed;
				rigidBody.velocity.y = -cosf(sprite.angle) * speed;
			}
			else {
				if (!DistanceH) {
					DistanceH = true;
				}
				rigidBody.velocity.x = -sinf(sprite.angle) * speed;
				rigidBody.velocity.y = -cosf(sprite.angle) * speed;
			}
		}
		else if (!translatedX2.empty()) {
			raylib::Vector2 vecTX{ translatedX2.front().x, translatedX2.front().y };
			vecTX.DrawCircle(10, BLUE);

			auto x = Vector2Distance(vecTX, born.bornPos);
			sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

			if (x < params.at(0) && transform.position.x != 9) {
				rigidBody.velocity.x = sinf(sprite.angle) * speed;
				rigidBody.velocity.y = -cosf(sprite.angle) * speed;
			}
			else {
				if (!DistanceH) {
					DistanceH = true;
				}
				rigidBody.velocity.x = -sinf(sprite.angle) * speed;
				rigidBody.velocity.y = -cosf(sprite.angle) * speed;
			}
		}

		if (whereToDestroy.CheckCollision(rigidBody.hitbox.hitboxRect) && DistanceH) {
			G::gEntitySetToBeDestroyed.insert(anchor);
		}
	}
};

struct UpgradeSystem : public ECS::System {

};

struct PhysicsSystem : public ECS::System {
public:
	void update(float dt) {
		for (auto const& entity : mEntities) {
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
			
			rigidBody.hitbox.hitboxRect.SetX(transform.position.x - 0.5f * (rigidBody.hitbox.hitboxRect.GetWidth()));
			rigidBody.hitbox.hitboxRect.SetY(transform.position.y - 0.5f * (rigidBody.hitbox.hitboxRect.GetHeight()));
			

			if (G::debugMode) {
				rigidBody.hitbox.hitboxRect.DrawLines(rigidBody.hitbox.hitboxColor);
			}

			transform.position += rigidBody.velocity * dt;
			rigidBody.velocity += rigidBody.acceleration;
		}
	}
};

struct RenderSystem : public ECS::System {
public:
	void updateSprites() {
		for (auto const& entity : mEntities) {
			auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);

			if (!sprite.isDependent) {
				
				sprite.sprite.Draw(
					raylib::Rectangle(0.0f, 0.0f, sprite.sprite.width, sprite.sprite.height),
					raylib::Rectangle(transform.position.x, transform.position.y, sprite.sprite.width, sprite.sprite.height),
					raylib::Vector2(sprite.origin),
					sprite.angle * RAD2DEG,
					sprite.tint
				);
			}

			G::gridRect.DrawLines(raylib::Color::Blue());
		}
	}
};

struct WeaponSystem : ECS::System {
	using WeaponmMini = ECS::Entity;

	void update() {
		for (auto const& entity : mEntities) {
			auto& miniWeapon = G::gCoordinator.GetComponent<WeaponMini>(entity);
			auto& transforms = G::gCoordinator.GetComponent<Transforms>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);

			if (miniWeapon.isSelected) {
				transforms.position = GetMousePosition();
			}
		}
	}

	static inline void createWeaponNormalCanon(WeaponMini mini) {

	}

	static inline void createWeaponMiniCanon() {
		auto const& canonMini = G::gCoordinator.CreateEntity();
		auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

		G::gCoordinator.AddComponent<WeaponMini>(canonMini, WeaponMini{
			.isHeld = false,
			.isEquipped = false,
			.rarity = ID_WEAPON_RARITY::COMMON,
			.name = "OMNIPOTENT CANON",
			.description = "This canon transcendents all of universe"
			});
		G::gCoordinator.AddComponent<Sprite>(canonMini, Sprite{
			.sprite = G::weapon_mini_canon,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(G::weapon_mini_canon.GetSize()) * 0.5f,
			.isDependent = true
			});

		raylib::Vector2 pos{};
		for (auto& slot : inventory.slotsInv) {
			if (slot.uptrItem == nullptr) {
				pos = slot.position + raylib::Vector2(G::weapon_mini_canon.GetSize()) * 0.5f;
				slot.uptrItem = std::make_shared<ECS::Entity>(canonMini);
				break;
			}
		}

		G::gCoordinator.AddComponent<Transforms>(canonMini, Transforms{
			.position = pos,
			.rotation = {0.0f, 0.0f},
			.scale = {1.0f, 1.0f}
			});

	}
};

struct InputSystem : public ECS::System {
public:
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
			auto& pSpecific = G::gCoordinator.GetComponent<PlayerSpecific>(entity);

			G::playerBoundingBoxForBullets = { G::gPlayerPos.x - G::screenWidth * 0.5f, G::gPlayerPos.y - G::screenHeight * 0.5f, G::screen.width, G::screen.height };

			G::playerBoundingBoxForBullets.DrawLines(RED);

			static float minSpeed = 30;
			static float minEffectLength = 10;
			static float fractionSpeed = 0.8f;

			G::camera.offset = Vector2(G::screenWidth / 2.0f, G::screenHeight / 2.0f);
			Vector2 diff = Vector2Subtract(transform.position, G::camera.target);
			float length = Vector2Length(diff);

			if (length > minEffectLength) {
				float speed = fmaxf(fractionSpeed * length, minSpeed);
				G::camera.target = Vector2Add(G::camera.target, Vector2Scale(diff, speed * GetFrameTime() / length));
			}


			G::gPlayerPos = transform.position;

			if (IsKeyPressed(KeyboardKey::KEY_C)) {
				WeaponSystem::createWeaponMiniCanon();
			}

			if (IsKeyPressed(KeyInputs::DASH) && pSpecific.dash <= 1.0f) {
				pSpecific.dash = 5.0f;
			}

			if (pSpecific.dash > 1.0f) {
				pSpecific.dash -= GetFrameTime() * 6.0f;
			}

			if (IsKeyDown(KeyInputs::BREAKS)) {
				if (rigidBody.velocity.x >= 2.f) {
					rigidBody.velocity.x--;
				}
				else if (rigidBody.velocity.x < 2.f) {
					rigidBody.velocity.x++;
				}
				else if (rigidBody.velocity.x <= 2.f && rigidBody.velocity.x >= -2.f) {
					rigidBody.velocity.x = 0.f;
				}

				if (rigidBody.velocity.y >= 2.f) {
					rigidBody.velocity.y--;
				}
				else if (rigidBody.velocity.y < 2.f) {
					rigidBody.velocity.y++;
				}
				else if (rigidBody.velocity.y <= 2.f && rigidBody.velocity.y >= -2.f) {
					rigidBody.velocity.y = 0.f;
				}
			}


			if (raylib::IsKeyOrMouseDown(KeyInputs::FLY)) {
				auto m_v = GetScreenToWorld2D(GetMousePosition(), G::camera);
				auto v = raylib::Vector2((m_v.x - transform.position.x) / Vector2Distance(transform.position, m_v),
										 (m_v.y - transform.position.y) / Vector2Distance(transform.position, m_v));
				sprite.angle = atan2f(v.x, -v.y);
				rigidBody.velocity.x = sinf(sprite.angle) * 80.0f * pSpecific.dash;
				rigidBody.velocity.y = -cosf(sprite.angle) * 80.0f * pSpecific.dash;
			}

			if (IsKeyPressed(KeyboardKey::KEY_H)) {
				if (!G::debugMode) { G::debugMode = true; }
				else { G::debugMode = false; }
			}

			G::gTimer.insertTimer(1, Timer(400, TIMER_ID::WAITTIMER_ID));

			if (raylib::IsKeyOrMouseDown(KeyInputs::SHOOT) && G::gTimer.checkTimer(1)) {
				G::gTimer.resetTimer(1);
				ECS::Entity entity1 = G::gCoordinator.CreateEntity();
				G::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
					.position = transform.position,
					.rotation = raylib::Vector2(0.0f, 0.0f),
					.scale = raylib::Vector2(1.0f, 1.0f)
					});
				G::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
					.velocity = raylib::Vector2(3.0f * rigidBody.velocity.x / pSpecific.dash, 3.0f * rigidBody.velocity.y / pSpecific.dash),
					.acceleration = raylib::Vector2(0.0f, 0.0f),
					.hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Green()},
					.isColliding = false,
					.onWhatSideIsColliding = {false ,false ,false ,false }
					});
				G::gCoordinator.AddComponent<Sprite>(entity1, Sprite{
					.sprite = G::playerBulletTexture2,
					.angle = sprite.angle,
					.tint = {255, 255, 255, 255},
					.origin = raylib::Vector2(G::playerBulletTexture2.width * 0.5f, G::playerBulletTexture2.height * 0.5f)
					});
				G::gCoordinator.AddComponent<Health>(entity1, Health{
					.maxHealth = 5.0f,
					.health = 5.0f,
					.isDamaged = false,
					.frameImmunityTime = 1,
					.healthToSubstract = 0.0f,
					.toBeDamaged = false
					});
				G::gCoordinator.AddComponent<Bullet>(entity1, Bullet{
					});
				G::gCoordinator.AddComponent<EntitySpecific>(entity1, EntitySpecific{
					.id = ENTITY_ID::PLAYER_BULLET_ID
					});
				ComponentCommons::addComponent<TimerComponent>(entity1);
				ComponentCommons::addComponent<Damage>(entity1, 5, 5);
				ComponentCommons::addComponent<MovmentAI>(entity1, false, false);
				ComponentCommons::addComponent<JustBorn>(entity1, entity, transform.position, rigidBody.velocity, sprite.angle, GetTime());

				auto& t = G::gCoordinator.GetComponent<RigidBody>(entity1);
				spatial_hash::gGird.insert(entity1, t.hitbox.hitboxRect);
			}
		}
	}
};

struct HealthSystem : public ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& _health = G::gCoordinator.GetComponent<Health>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& timerComponent = G::gCoordinator.GetComponent<TimerComponent>(entity);
			auto& entitySpecific = G::gCoordinator.GetComponent<EntitySpecific>(entity);
			
			if (entitySpecific.id == ENTITY_ID::ENEMY_ID) {
				_health.drawHealthBar(rigidBody.hitbox.getHitBoxCenter(), raylib::Vector2(0, sprite.sprite.height / 2.0f + 2));
			}

			if (_health.isDamaged) {
				sprite.tint = { 230, 41, 55, 255 };
				timerComponent.timerCont.insertTimer(1, Timer(_health.frameImmunityTime, TIMER_ID::WAITTIMER_ID));

				if (_health.toBeDamaged) {
					_health.health -= _health.healthToSubstract;
					_health.toBeDamaged = false;
				}

				if (timerComponent.timerCont.checkTimer(1)) {
					_health.healthToSubstract = 0.0f;
					_health.isDamaged = false;
					sprite.tint = { 255, 255, 255, 255 };
					timerComponent.timerCont.removeTimer(1);
				}
			}

			if (_health.health <= 0.0f) {
				if (entitySpecific.id == ENTITY_ID::ENEMY_ID) {
					G::gEnemyCounter--;
				}
				G::gEntitySetToBeDestroyed.insert(entity);
			}
		}
	}
};

struct CollisionSystem : public ECS::System {
	//Entity
	//RigidBody
	//Health
	void update() {
		for (auto const& entity1 : mEntities) {
			auto& transform1 = G::gCoordinator.GetComponent<Transforms>(entity1);
			auto& health1 = G::gCoordinator.GetComponent<Health>(entity1);
			auto& rigidBody1 = G::gCoordinator.GetComponent<RigidBody>(entity1);
			auto& entitySpecific1 = G::gCoordinator.GetComponent<EntitySpecific>(entity1);
			auto& damage1 = G::gCoordinator.GetComponent<Damage>(entity1);

			for (auto const& entity2 : spatial_hash::gGird.query(rigidBody1.hitbox.hitboxRect)) {
				auto& transform2 = G::gCoordinator.GetComponent<Transforms>(entity2);
				auto& health2 = G::gCoordinator.GetComponent<Health>(entity2);
				auto& rigidBody2 = G::gCoordinator.GetComponent<RigidBody>(entity2);
				auto& entitySpecific2 = G::gCoordinator.GetComponent<EntitySpecific>(entity2);
				auto& damage2 = G::gCoordinator.GetComponent<Damage>(entity2);
				
				if (entity1 != entity2 && rigidBody2.hitbox.hitboxRect.CheckCollision(rigidBody1.hitbox.hitboxRect)) {
					if (entitySpecific1.id == ENTITY_ID::ENEMY_ID && entitySpecific2.id == ENTITY_ID::PLAYER_ID) {
						if (!health2.isDamaged) {
							health2.healthToSubstract = damage1.damageOnContact;
							health2.toBeDamaged = true;
						}
						health2.isDamaged = true;
					}
					else if (entitySpecific1.id == ENTITY_ID::ENEMY_ID && entitySpecific2.id == ENTITY_ID::PLAYER_BULLET_ID) {
						if (!health1.isDamaged) {
							health1.healthToSubstract = damage2.damageOnContact;
							health1.toBeDamaged = true;
						}
						health1.isDamaged = true;
						health2.health = 0.0f;
					}

					if (entitySpecific1.id == ENTITY_ID::ENEMY_ID && entitySpecific2.id == ENTITY_ID::ENEMY_ID) {
						//handling collison here bruh
						raylib::AABBcollisionResponse(transform1.position, transform2.position, rigidBody1.hitbox.hitboxRect, rigidBody2.hitbox.hitboxRect);

					}
				}
			}
		}
	}
};

struct EnemySpawningSystem : public ECS::System{
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
			auto& pSpecific = G::gCoordinator.GetComponent<PlayerSpecific>(entity);
			
			if (G::gEnemyCounter == 0) {
				if (IsKeyPressed(KeyInputs::NEXT_WAVE) || G::gLevel == 0) {
					G::gLevel++;
					G::gEnemyCounter = G::gLevel;

					raylib::Vector2 vP = G::gPlayerPos;
					std::uniform_int_distribution distribX(static_cast<int>(vP.x) - 100, static_cast<int>(vP.x) + 100);
					std::uniform_int_distribution distribY(static_cast<int>(vP.y) - 100, static_cast<int>(vP.y) + 100);

					for (int i = 0; i < G::gEnemyCounter; i++) {
						Vector2 v = { distribX(gen), distribY(gen) };

						while (Vector2Distance(vP, v) < 70 || !G::gridRect.CheckCollision(v)) {
							v = { (float)distribX(gen), (float)distribY(gen) };
						}

						ECS::Entity entity1 = G::gCoordinator.CreateEntity();
						G::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
							.position = raylib::Vector2(v),
							.rotation = raylib::Vector2(0.0f, 0.0f),
							.scale = raylib::Vector2(1.0f, 1.0f)
							});
						G::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
							.velocity = raylib::Vector2(0.0f, 0.0f),
							.acceleration = raylib::Vector2(0.0f, 0.0f),
							.hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Red()},
							.isColliding = false,
							.onWhatSideIsColliding = {false ,false ,false ,false }
							});
						G::gCoordinator.AddComponent<Sprite>(entity1, Sprite{
							.sprite = G::enemyTexture,
							.angle = 0.0f,
							.tint = {255, 255, 255, 255},
							.origin = raylib::Vector2(G::enemyTexture.width * 0.5f, G::enemyTexture.height * 0.5f)
							});
						G::gCoordinator.AddComponent<Health>(entity1, Health{
							.maxHealth = 30.0f,
							.health = 30.0f,
							.isDamaged = false,
							.frameImmunityTime = 10,
							.healthToSubstract = 0.0f,
							.toBeDamaged = false
							});
						G::gCoordinator.AddComponent<Enemy>(entity1, Enemy{

							});
						ComponentCommons::addComponent<MovmentAI>(entity1, false, false);
						G::gCoordinator.AddComponent<EntitySpecific>(entity1, EntitySpecific{
						.id = ENTITY_ID::ENEMY_ID
							});
						G::gCoordinator.AddComponent<TimerComponent>(entity1, TimerComponent{});
						ComponentCommons::addComponent<Damage>(entity1, 5, 5);

						auto& t = G::gCoordinator.GetComponent<RigidBody>(entity1);
						spatial_hash::gGird.insert(entity1, t.hitbox.hitboxRect);
					}
				}
			}
		}
	}
};

struct EnemyAIMovmentSystem : ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
			auto& movmentAI = G::gCoordinator.GetComponent<MovmentAI>(entity);
			//auto& enemy = GLOBALS::gCoordinator.GetComponent<Enemy>(entity);
			
			movmentAI.chase(transform.position, G::gPlayerPos, rigidBody.velocity, sprite.angle, 50.0f);
		}
	}
};

struct BulletManipulationSystem : ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& health = G::gCoordinator.GetComponent<Health>(entity);
			auto& movmentAI = G::gCoordinator.GetComponent<MovmentAI>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
			auto& justBorn = G::gCoordinator.GetComponent<JustBorn>(entity);
			auto& bullet = G::gCoordinator.GetComponent<Bullet>(entity);
      
			movmentAI.moveInLineOfFunc(200.0f, entity, std::vector<float>{500.f, 600.f}, MovmentAI::sinwaveExactFunc);//func neads to be zeroed to follow bornAngle path

			if (!raylib::containsRect(G::gridRect, rigidBody.hitbox.hitboxRect)) {
				health.health = 0.0f;
			}
		}
	}
};


struct EntityRemovalSystem : ECS::System {
	void update() {
		for (auto const& entity : G::gEntitySetToBeDestroyed) {
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);

			//sprite.sprite.Unload();
			G::gCoordinator.DestroyEntity(entity);
		}

		G::gEntitySetToBeDestroyed.clear();
		spatial_hash::gGird.clearAllTiles();

		for (auto const& entity : mEntities) {
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
			spatial_hash::gGird.insert(entity, rigidBody.hitbox.hitboxRect);
		}
	}
};
