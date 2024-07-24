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

enum ENTITY_ID {
	PLAYER_ID,
	ENEMY_ID,
	PLAYER_BULLET_ID,
	ENEMY_BULLET_ID
};

class ComponentCommons {
public:
	template<typename Component, typename... Types>
	static inline void addComponent(ECS::Entity entity, Types... args) {
		GLOBALS::gCoordinator.AddComponent<Component>(entity, Component(args...));
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
		raylib::Rectangle r1 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(maxHealth, 20) };
		raylib::Rectangle r2 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(health, 20) - raylib::Vector2(1, 1)};
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
	float damage;//1
	float damageOnContact;//2
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

struct MovmentAI {
	void chase(const raylib::Vector2& pos, const raylib::Vector2& posToChase, raylib::Vector2& velocity, float& angle, float speed, float seeRadius) {
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
	* @brief for bullets movment pattern
	* @param angle  -  to change rotation in sinusoidal pattern
	* @param ampltidue  -  how far wave expand
	* @param frequency  -  how fast its going up and down
	*/
	void sinwaveMovment(float speed ,float angle, float ampltidue, float frequency, raylib::Vector2& velocity) {
		float beta = angle + PI / 2.0f;
		float currentDistance = ampltidue * sinf(GetTime() * frequency);

		velocity.x = sinf(beta) * currentDistance + sinf(angle) * speed;
		velocity.y = -cosf(beta) * currentDistance + -cosf(angle) * speed;
	}
};

struct PhysicsSystem : public ECS::System {
public:
	void update(float dt) {
		for (auto const& entity : mEntities) {
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& transform = GLOBALS::gCoordinator.GetComponent<Transforms>(entity);

			
			transform.position += rigidBody.velocity * dt;
			rigidBody.velocity += rigidBody.acceleration;
		}
	}
};

struct RenderSystem : public ECS::System {
public:
	void updateSprites() {
		for (auto const& entity : mEntities) {
			auto& transform = GLOBALS::gCoordinator.GetComponent<Transforms>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);

			rigidBody.hitbox.hitboxRect.SetX(transform.position.x - 0.5f * (rigidBody.hitbox.hitboxRect.GetWidth()));
			rigidBody.hitbox.hitboxRect.SetY(transform.position.y - 0.5f * (rigidBody.hitbox.hitboxRect.GetHeight()));

			sprite.sprite.Draw(
				raylib::Rectangle(0.0f, 0.0f, sprite.sprite.width, sprite.sprite.height),
				raylib::Rectangle(transform.position.x, transform.position.y, sprite.sprite.width, sprite.sprite.height),
				raylib::Vector2(sprite.origin),
				sprite.angle * RAD2DEG,
				sprite.tint
			);

			if(GLOBALS::debugMode) {
				rigidBody.hitbox.hitboxRect.DrawLines(rigidBody.hitbox.hitboxColor);
			}
			//raylib::DrawText(std::to_string(entity), transform.position.x, transform.position.y, 12, raylib::Color::Black());

			GLOBALS::gridRect.DrawLines(raylib::Color::Blue());
		}
	}
};

struct InputSystem : public ECS::System {
public:
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = GLOBALS::gCoordinator.GetComponent<Transforms>(entity);
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);
			auto& pSpecific = GLOBALS::gCoordinator.GetComponent<PlayerSpecific>(entity);

			GLOBALS::playerBoundingBoxForBullets = { GLOBALS::gPlayerPos.x - GLOBALS::screenWidth * 0.5f, GLOBALS::gPlayerPos.y - GLOBALS::screenHeight * 0.5f, GLOBALS::screen.width, GLOBALS::screen.height };

			GLOBALS::playerBoundingBoxForBullets.DrawLines(RED);

			static float minSpeed = 30;
			static float minEffectLength = 10;
			static float fractionSpeed = 0.8f;

			GLOBALS::camera.offset = Vector2(GLOBALS::screenWidth / 2.0f, GLOBALS::screenHeight / 2.0f);
			Vector2 diff = Vector2Subtract(transform.position, GLOBALS::camera.target);
			float length = Vector2Length(diff);

			if (length > minEffectLength) {
				float speed = fmaxf(fractionSpeed * length, minSpeed);
				GLOBALS::camera.target = Vector2Add(GLOBALS::camera.target, Vector2Scale(diff, speed * GetFrameTime() / length));
			}


			GLOBALS::gPlayerPos = transform.position;

			if (IsKeyPressed(KeyboardKey::KEY_SPACE) && pSpecific.dash <= 1.0f) {
				pSpecific.dash = 5.0f;
			}

			if (pSpecific.dash > 1.0f) {
				pSpecific.dash -= GetFrameTime() * 6.0f;
			}

			if (raylib::IsKeyOrMouseDown(KeyInputs::FLY)) {
				auto m_v = GetScreenToWorld2D(GetMousePosition(), GLOBALS::camera);
				auto v = raylib::Vector2((m_v.x - transform.position.x) / Vector2Distance(transform.position, m_v),
										 (m_v.y - transform.position.y) / Vector2Distance(transform.position, m_v));
				sprite.angle = atan2f(v.x, -v.y);
				rigidBody.velocity.x = sinf(sprite.angle) * 80.0f * pSpecific.dash;
				rigidBody.velocity.y = -cosf(sprite.angle) * 80.0f * pSpecific.dash;
			}

			if (IsKeyPressed(KeyboardKey::KEY_H)) {
				if (!GLOBALS::debugMode) { GLOBALS::debugMode = true; }
				else { GLOBALS::debugMode = false; }
			}

			GLOBALS::gTimer.insertTimer(1, Timer(400, TIMER_ID::WAITTIMER_ID));

			if (raylib::IsKeyOrMouseDown(KeyInputs::SHOOT) && GLOBALS::gTimer.checkTimer(1)) {
				GLOBALS::gTimer.resetTimer(1);
				ECS::Entity entity1 = GLOBALS::gCoordinator.CreateEntity();
				GLOBALS::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
					.position = transform.position,
					.rotation = raylib::Vector2(0.0f, 0.0f),
					.scale = raylib::Vector2(1.0f, 1.0f)
					});
				GLOBALS::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
					.velocity = raylib::Vector2(3.0f * rigidBody.velocity.x / pSpecific.dash, 3.0f * rigidBody.velocity.y / pSpecific.dash),
					.acceleration = raylib::Vector2(0.0f, 0.0f),
					.hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Green()},
					.isColliding = false,
					.onWhatSideIsColliding = {false ,false ,false ,false }
					});
				GLOBALS::gCoordinator.AddComponent<Sprite>(entity1, Sprite{
					.sprite = raylib::Texture2DUnmanaged("resources/PlayerBulletModel.png"),
					.angle = sprite.angle,
					.tint = {255, 255, 255, 255},
					.origin = raylib::Vector2(raylib::Texture2D("resources/PlayerBulletModel.png").width * 0.5f, raylib::Texture2D("resources/PlayerBulletModel.png").height * 0.5f)
					});
				GLOBALS::gCoordinator.AddComponent<Health>(entity1, Health{
					.maxHealth = 5.0f,
					.health = 5.0f,
					.isDamaged = false,
					.frameImmunityTime = 1,
					.healthToSubstract = 0.0f,
					.toBeDamaged = false
					});
				GLOBALS::gCoordinator.AddComponent<Bullet>(entity1, Bullet{
					});
				GLOBALS::gCoordinator.AddComponent<EntitySpecific>(entity1, EntitySpecific{
					.id = ENTITY_ID::PLAYER_BULLET_ID
					});
				GLOBALS::gCoordinator.AddComponent<TimerComponent>(entity1, TimerComponent{});
				ComponentCommons::addComponent<Damage>(entity1, 5, 5);
				ComponentCommons::addComponent<MovmentAI>(entity1);

				auto& t = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity1);
				spatial_hash::gGird.insert(entity1, t.hitbox.hitboxRect);
			}
		}
	}
};

struct HealthSystem : public ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& _health = GLOBALS::gCoordinator.GetComponent<Health>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& timerComponent = GLOBALS::gCoordinator.GetComponent<TimerComponent>(entity);
			auto& entitySpecific = GLOBALS::gCoordinator.GetComponent<EntitySpecific>(entity);

			_health.drawHealthBar(rigidBody.hitbox.getHitBoxCenter(), raylib::Vector2(0, sprite.sprite.height / 2.0f + 2));

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
					GLOBALS::gEnemyCounter--;
				}
				GLOBALS::gEntitySetToBeDestroyed.insert(entity);
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
			auto& health1 = GLOBALS::gCoordinator.GetComponent<Health>(entity1);
			auto& rigidBody1 = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity1);
			auto& entitySpecific1 = GLOBALS::gCoordinator.GetComponent<EntitySpecific>(entity1);
			auto& damage1 = GLOBALS::gCoordinator.GetComponent<Damage>(entity1);

			for (auto const& entity2 : spatial_hash::gGird.query(rigidBody1.hitbox.hitboxRect)) {
				auto& health2 = GLOBALS::gCoordinator.GetComponent<Health>(entity2);
				auto& rigidBody2 = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity2);
				auto& entitySpecific2 = GLOBALS::gCoordinator.GetComponent<EntitySpecific>(entity2);
				auto& damage2 = GLOBALS::gCoordinator.GetComponent<Damage>(entity2);
				
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
						raylib::AABBcollisionResponse(rigidBody1.velocity, rigidBody2.velocity, rigidBody1.hitbox.hitboxRect, rigidBody2.hitbox.hitboxRect);

					}
				}
			}
		}
	}
};

struct EnemySpawningSystem : public ECS::System{
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = GLOBALS::gCoordinator.GetComponent<Transforms>(entity);
			auto& pSpecific = GLOBALS::gCoordinator.GetComponent<PlayerSpecific>(entity);
			
			if (GLOBALS::gEnemyCounter == 0) {
				GLOBALS::gLevel++;
				GLOBALS::gEnemyCounter = GLOBALS::gLevel;
			
				std::random_device rd;
				std::mt19937 gen(rd());

				raylib::Vector2 vP = GLOBALS::gPlayerPos;
				std::uniform_int_distribution distribX(static_cast<int>(vP.x) - 100, static_cast<int>(vP.x) + 100);
				std::uniform_int_distribution distribY(static_cast<int>(vP.y) - 100, static_cast<int>(vP.y) + 100);

				for (int i = 0; i < GLOBALS::gEnemyCounter; i++) {
					Vector2 v = { distribX(gen), distribY(gen) };

 					while (Vector2Distance(vP, v) < 70 || !GLOBALS::gridRect.CheckCollision(v)) {
						v = { (float)distribX(gen), (float)distribY(gen) };
					}

					ECS::Entity entity1 = GLOBALS::gCoordinator.CreateEntity();
					GLOBALS::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
						.position = raylib::Vector2(v),
						.rotation = raylib::Vector2(0.0f, 0.0f),
						.scale = raylib::Vector2(1.0f, 1.0f)
						});
					GLOBALS::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
						.velocity = raylib::Vector2(0.0f, 0.0f),
						.acceleration = raylib::Vector2(0.0f, 0.0f),
						.hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Red()},
						.isColliding = false,
						.onWhatSideIsColliding = {false ,false ,false ,false }
						});
					GLOBALS::gCoordinator.AddComponent<Sprite>(entity1, Sprite{
						.sprite = raylib::Texture2DUnmanaged("resources/EnemyModel.png"),
						.angle = 0.0f,
						.tint = {255, 255, 255, 255},
						.origin = raylib::Vector2(raylib::Texture2D("resources/EnemyModel.png").width * 0.5f, raylib::Texture2D("resources/EnemyModel.png").height * 0.5f)
						});
					GLOBALS::gCoordinator.AddComponent<Health>(entity1, Health{
						.maxHealth = 30.0f,
						.health = 30.0f,
						.isDamaged = false,
						.frameImmunityTime = 10,
						.healthToSubstract = 0.0f,
						.toBeDamaged = false
						});
					GLOBALS::gCoordinator.AddComponent<Enemy>(entity1, Enemy{

						});
					GLOBALS::gCoordinator.AddComponent<MovmentAI>(entity1, MovmentAI{

						});
					GLOBALS::gCoordinator.AddComponent<EntitySpecific>(entity1, EntitySpecific{
					.id = ENTITY_ID::ENEMY_ID
						});
					GLOBALS::gCoordinator.AddComponent<TimerComponent>(entity1, TimerComponent{});
					ComponentCommons::addComponent<Damage>(entity1, 5, 5);

					auto& t = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity1);
					spatial_hash::gGird.insert(entity1, t.hitbox.hitboxRect);
				}
			}
		}
	}
};

struct EnemyAIMovmentSystem : ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& transform = GLOBALS::gCoordinator.GetComponent<Transforms>(entity);
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);
			auto& movmentAI = GLOBALS::gCoordinator.GetComponent<MovmentAI>(entity);
			//auto& enemy = GLOBALS::gCoordinator.GetComponent<Enemy>(entity);
			
			movmentAI.chase(transform.position, GLOBALS::gPlayerPos, rigidBody.velocity, sprite.angle, 50.0f, 500.0f);
		}
	}
};

struct BulletManipulationSystem : ECS::System {
	void update() {
		for (auto const& entity : mEntities) {
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& health = GLOBALS::gCoordinator.GetComponent<Health>(entity);
			auto& movmentAI = GLOBALS::gCoordinator.GetComponent<MovmentAI>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);
			auto& bullet = GLOBALS::gCoordinator.GetComponent<Bullet>(entity);
			
			movmentAI.sinwaveMovment(200.0f, sprite.angle, 100, 10, rigidBody.velocity);

			if (!raylib::containsRect(GLOBALS::gridRect, rigidBody.hitbox.hitboxRect)) {
				health.health = 0.0f;
			}
		}
	}
};

struct EntityRemovalSystem : ECS::System {
	void update() {
		for (auto const& entity : GLOBALS::gEntitySetToBeDestroyed) {
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			auto& sprite = GLOBALS::gCoordinator.GetComponent<Sprite>(entity);

			sprite.sprite.Unload();
			GLOBALS::gCoordinator.DestroyEntity(entity);
		}

		GLOBALS::gEntitySetToBeDestroyed.clear();

		spatial_hash::gGird.clearAllTiles();

		for (auto const& entity : mEntities) {
			auto& rigidBody = GLOBALS::gCoordinator.GetComponent<RigidBody>(entity);
			spatial_hash::gGird.insert(entity, rigidBody.hitbox.hitboxRect);
		}
	}
};