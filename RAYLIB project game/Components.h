#pragma once
#include <src/raylib-cpp.hpp>
#include "ECS.h"
#include "globals.h"
#include "Timer.h"
#include "utils.h"
#include "spatialHash/grid.h"
#include "GeometryCalc.h"
#include "Coroutines.h"
#include <random>
#include <vector>
#include <any>
#include <numbers>
#include <iostream>

static std::random_device rd;
static std::mt19937 gen(rd());

enum ENTITY_ID {
	PLAYER_ID,
	ENEMY_ID,
	PLAYER_BULLET_ID,
	ENEMY_BULLET_ID,
	BOSS_ID
};

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
		NORMAL = 0,
		EXPLOSIVE,
		PENETRATION
	};

	float damage;//1
	float damageOnContact;//2
};

struct upgDebuff {
	enum class DEBUFF_TYPES {
		BLEED = 0,
		BURN,
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
	* @brief sinusoidal pattern
	* @param params -  0 for maxX, 1 for maxY
	*/
	static inline double sinwaveExactFunc(double x, std::vector<float> params) {
		return params.at(1) * sinf(x * (PI / params.at(0)));
	}

	/**
	* @brief y = a * sin (b)
	* @param params -  0 for b, 1 for a
	*/
	static inline double sinwaveFunc(double x, std::vector<float> params) {
		return params.at(1) * sinf(x * params.at(0));
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
			auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);

			rigidBody.hitbox.hitboxRect.SetX(transform.position.x - 0.5f * (rigidBody.hitbox.hitboxRect.GetWidth()));
			rigidBody.hitbox.hitboxRect.SetY(transform.position.y - 0.5f * (rigidBody.hitbox.hitboxRect.GetHeight()));

			sprite.sprite.Draw(
				raylib::Rectangle(0.0f, 0.0f, sprite.sprite.width, sprite.sprite.height),
				raylib::Rectangle(transform.position.x, transform.position.y, sprite.sprite.width, sprite.sprite.height),
				raylib::Vector2(sprite.origin),
				sprite.angle * RAD2DEG,
				sprite.tint
			);

			if(G::debugMode) {
				rigidBody.hitbox.hitboxRect.DrawLines(rigidBody.hitbox.hitboxColor);
			}
			//raylib::DrawText(std::to_string(entity), transform.position.x, transform.position.y, 12, raylib::Color::Black());

			G::gridRect.DrawLines(raylib::Color::Blue());
		}
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

			if (IsKeyPressed(KeyboardKey::KEY_SPACE) && pSpecific.dash <= 1.0f) {
				pSpecific.dash = 5.0f;
			}

			if (pSpecific.dash > 1.0f) {
				pSpecific.dash -= GetFrameTime() * 6.0f;
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
			
			movmentAI.moveInLineOfFunc(200.0f, entity, std::vector<float>{tanf(2.f*PI)}, MovmentAI::straightLineFunc);//func neads to be zeroed to follow bornAngle path

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