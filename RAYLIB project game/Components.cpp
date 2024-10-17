#include "Components.h"

#include "spatialHash/grid.h"
#include "globals.h"

#include "./Weapons/Weapons.h"
#include "./Weapons/WeaponDef/Cannon.h"
#include "./Weapons/WeaponDef/Minigun.h"
#include "./Weapons/WeaponDef/LaserPistol.h"

#include "EntityIds.h"

#include "Components/Transforms.h"
#include "Components/RigidBody.h"
#include "Components/Health.h"
#include "Components/Bullet.h"
#include "Components/EntitySpecific.h"
#include "Components/TimerComponent.h"
#include "Components/Damage.h"
#include "Components/PlayerSpecific.h"
#include "Components/ComponentsCommons.h"
#include "Components/Enemy.h"
#include "Components/JustBorn.h"
#include "Components/Sprite.h"
#include "Components/MovmentAI.h"


#include "Components/Inventory.h"

#include "Timer.h"

#include "GeometryCalc.hpp"
#include "utils.hpp"

inline void ENTITY_CREATION_FUNCTIONS::createBullet(ECS::Entity parent,
	Transforms      const& transforms,
	RigidBody       const& rigidBody,
	Sprite          const& sprite,
	Health          const& health,
	Bullet          const& bullet,
	EntitySpecific  const& entitySpecific,
	TimerComponent  const& timerComponent,
	Damage          const& damage,
	MovmentAI       const& movmentAI) {
		//transforms
		//rigidbody
		//sprite
		//health
		//bullet
		//entityspecigic
		//timercomponent
		//damage
		//movmentAI
		//JustBorn
	ECS::Entity entity = G::gCoordinator.CreateEntity();

	G::gCoordinator.AddComponent<Transforms>     (entity, Transforms         (transforms));
	G::gCoordinator.AddComponent<RigidBody>      (entity, RigidBody           (rigidBody));
	G::gCoordinator.AddComponent<Sprite>         (entity, Sprite                 (sprite));
	G::gCoordinator.AddComponent<Health>         (entity, Health                 (health));
	G::gCoordinator.AddComponent<Bullet>         (entity, Bullet                 (bullet));
	G::gCoordinator.AddComponent<EntitySpecific> (entity, EntitySpecific (entitySpecific));
	G::gCoordinator.AddComponent<TimerComponent> (entity, TimerComponent (timerComponent));
	G::gCoordinator.AddComponent<Damage>         (entity, Damage                 (damage));
	G::gCoordinator.AddComponent<MovmentAI>      (entity, MovmentAI           (movmentAI));
	
	ComponentCommons::addComponent<JustBorn>(entity, parent, transforms.position, raylib::Vector2{ sinf(sprite.angle), -cosf(sprite.angle) }, sprite.angle, GetTime());

	auto& t = G::gCoordinator.GetComponent<RigidBody>(entity);
	spatial_hash::gGird.insert(entity, t.hitbox.hitboxRect);
}

void PhysicsSystem::update(float dt) {
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

void RenderSystem::updateSprites() {
	for (auto const& entity : mEntities) {
		//auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		
		if (!sprite.isDependent) {
			sprite.draw(entity);
		}

		G::gridRect.DrawLines(raylib::Color::Blue());
	}
}

void WeaponSystem::update() {
	auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);
	auto const& weaponLibrary = G::gCoordinator.GetComponent<WeaponLibrary>(G::player);
	auto const& sprite_player = G::gCoordinator.GetComponent<Sprite>(G::player);
	auto& rigidbody_player = G::gCoordinator.GetComponent<RigidBody>(G::player);

	float r = 60.f;
	for (int i = 0; i < inventory.weaponSize; i++) {
		raylib::Rectangle rect{ 0.0f, 0.0f, 30.f, 30.f };
		float angle = DEG2RAD * (static_cast<float>(i) * 360.0f / static_cast<float>(inventory.weaponSize));
		rect.SetPosition(raylib::Vector2{ r * cosf(angle + sprite_player.angle), r * sinf(angle + sprite_player.angle) } + rigidbody_player.hitbox.getHitBoxCenter() - rect.GetSize() * 0.5f);
		rect.Draw({ 0, 0, 0, 100 });
	}
	weaponInvVibeCheck(inventory, weaponLibrary);

	uint32_t weaponNormalCount = 0;
	for (auto const& entity : mEntities) {
		auto const& weaponType = G::gCoordinator.GetComponent<WeaponType>(entity);
		auto& transforms = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);

		if (weaponType.id == ID_WEAPON_TYPE::MINI) {
			auto& miniWeapon = G::gCoordinator.GetComponent<WeaponMini>(entity);
			if (miniWeapon.isSelected) {
				transforms.position = GetMousePosition();
			}
			else {
				if (miniWeapon.afterSelecting) {
					inventory.moveUptrItem(inventory.allSlots, entity);
					miniWeapon.afterSelecting = false;
				}

				transforms.position = miniWeapon.posToStay;
			}
		}
	}
}

void WeaponSystem::weaponInvVibeCheck(Inventory const& inv, WeaponLibrary const& weaponLibrary) {
	uint32_t weaponSlotCount = 0;
	
	auto const& sprite_player = G::gCoordinator.GetComponent<Sprite>(G::player);
	auto& rigidbody_player = G::gCoordinator.GetComponent<RigidBody>(G::player);
	float r = 60.f;

	for (auto const& slot : inv.allSlots) {
		if (slot.slotPurpuse == Inventory::SLOT_PURPOSE::SLOT_WEAPON) {
			if (slot.ptrItem != nullptr) {
				auto& weaponMini = G::gCoordinator.GetComponent<WeaponMini>(*slot.ptrItem);
				
				if (weaponMini.isNormalInWorld) {
					auto& transforms = G::gCoordinator.GetComponent<Transforms>(*weaponMini.ptrWeaponNormal);

					float angle = DEG2RAD * (static_cast<float>(weaponSlotCount) * 360.f / static_cast<float>(inv.weaponSize));
					transforms.position = raylib::Vector2{ r * cosf(angle + sprite_player.angle), r * sinf(angle + sprite_player.angle) } + rigidbody_player.hitbox.getHitBoxCenter();
					
					weaponLibrary.weaponBehaviourMap.at(weaponMini.id)(*weaponMini.ptrWeaponNormal);
				}

				if (weaponLibrary.weaponNormalCreationMap.contains(weaponMini.id) && weaponMini.isNormalInWorld == false) {
					weaponLibrary.weaponNormalCreationMap.at(weaponMini.id)(G::gCoordinator.GetComponent<Inventory>(G::player), weaponMini);
					weaponMini.isNormalInWorld = true;
				}
			}
			weaponSlotCount++;
		}
	}
}

template<typename T>
void WeaponLibrary::insertToLib() {
	auto id_name = std::string(typeid(T).name());

	id_name.erase(0, 13);//erase "struct weapon" at beginning  
	id_name.insert(0, "ID");//add "ID" at beginning

	//now id_name is matching naming pattern from enum class ID_WEAPON
	auto id_weapon = magic_enum::enum_cast<ID_WEAPON>(id_name);

	assert(id_weapon.has_value() && "you fucked up naming man");

	rarityToWeaponsMap.at(T::getIdWeaponRarity()).emplace_back(id_weapon.value());

	static T it;
	CreateMiniFunc   crMini = std::bind(&T::p_CreateMini,   &it);
	CreateNormalFunc crNorm = std::bind(&T::p_CreateNormal, &it, std::placeholders::_1, std::placeholders::_2);
	WeaponBehaviour  wpnBeh = std::bind(&T::p_Behaviour,    &it, std::placeholders::_1);

	weaponMiniCreationMap.try_emplace(id_weapon.value(), crMini);
	weaponNormalCreationMap.try_emplace(id_weapon.value(), crNorm);
	weaponBehaviourMap.try_emplace(id_weapon.value(), wpnBeh);
}

void WeaponLibrary::populateRaToWe() {
	auto elem_count = magic_enum::enum_count<ID_WEAPON_RARITY>();

	for (int i = 0; i < elem_count; i++) {
		rarityToWeaponsMap.try_emplace(ID_WEAPON_RARITY(i), std::vector<ID_WEAPON>{});
	}
}

WeaponLibrary::WeaponLibrary() {
	populateRaToWe();
	insertToLib<Weapon_CANON>();
	insertToLib<Weapon_MINIGUN>();
	insertToLib<Weapon_LASERPISTOL>();
}

void WeaponHandOut::update() {
	if (G::gEnemyCounter == 0) {
		//TODO -- use stack for it

		
	}
}

inline void InputSystem::breaksVelocity(RigidBody& rigidBody) {
	if (IsKeyDown(KeyInputs::BREAKS)) {
		if (rigidBody.velocity.x >= 5.f) {
			rigidBody.velocity.x--;
		}
		else if (rigidBody.velocity.x <= -5.f) {
			rigidBody.velocity.x++;
		}
		else if (rigidBody.velocity.x <= 5.f && rigidBody.velocity.x >= -5.f) {
			rigidBody.velocity.x = 0.f;
		}

		if (rigidBody.velocity.y >= 5.f) {
			rigidBody.velocity.y--;
		}
		else if (rigidBody.velocity.y <= -5.f) {
			rigidBody.velocity.y++;
		}
		else if (rigidBody.velocity.y <= 5.f && rigidBody.velocity.y >= -5.f) {
			rigidBody.velocity.y = 0.f;
		}
	}
}

inline void InputSystem::shoot(ECS::Entity entity, Transforms const& transform, RigidBody const& rigidBody, Sprite const& sprite, TimerComponent& timer) {
	timer.timerCont.insertTimer(2, Timer(400, TIMER_ID::WAITTIMER_ID));

	if (raylib::IsKeyOrMouseDown(KeyInputs::SHOOT) && timer.timerCont.checkTimer(2)) {
		timer.timerCont.resetTimer(2);
		ECS::Entity entity1 = G::gCoordinator.CreateEntity();
		G::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
			.position = transform.position,
			.rotation = raylib::Vector2(0.0f, 0.0f),
			.scale = raylib::Vector2(1.0f, 1.0f)
			});
		G::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
			.velocity = raylib::Vector2(3.0f * rigidBody.velocity.x, 3.0f * rigidBody.velocity.y),
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
			.id = ID_ENTITY::PLAYER_BULLET_ID
			});
		ComponentCommons::addComponent<TimerComponent>(entity1);
		ComponentCommons::addComponent<Damage>(entity1, 5, 5);
		ComponentCommons::addComponent<MovmentAI>(entity1, false, false);
		ComponentCommons::addComponent<JustBorn>(entity1, entity, transform.position, rigidBody.velocity, sprite.angle, GetTime());

		auto& t = G::gCoordinator.GetComponent<RigidBody>(entity1);
		spatial_hash::gGird.insert(entity1, t.hitbox.hitboxRect);
	}
}

inline void InputSystem::fly(Transforms const& transform, RigidBody& rigidBody, Sprite& sprite, PlayerSpecific const& pSpecific) {
	if (raylib::IsKeyOrMouseDown(KeyInputs::FLY)) {
		auto m_v = GetScreenToWorld2D(GetMousePosition(), G::camera);
		MovmentAI::chase(transform.position, m_v, rigidBody.velocity, sprite.angle, 80.0f * pSpecific.dash);
	}
}

void InputSystem::update() {
	for (auto const& entity : mEntities) {
		auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		auto& pSpecific = G::gCoordinator.GetComponent<PlayerSpecific>(entity);
		auto& timer = G::gCoordinator.GetComponent<TimerComponent>(entity);

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
			Weapon_CANON().createMini();
		}
		if (IsKeyPressed(KeyboardKey::KEY_V)) {
			Weapon_MINIGUN().createMini();
		}
		if (IsKeyPressed(KeyboardKey::KEY_B)) {
			Weapon_LASERPISTOL().createMini();
		}

		if (IsKeyPressed(KeyInputs::DASH) && pSpecific.dash <= 1.0f) {
			pSpecific.dash = 5.0f;
		}

		if (pSpecific.dash > 1.0f) {
			pSpecific.dash -= GetFrameTime() * 6.0f;
		}

		if (IsKeyPressed(KeyboardKey::KEY_H)) {
			if (!G::debugMode) { G::debugMode = true; }
			else { G::debugMode = false; }
		}
		
		breaksVelocity(rigidBody);
		fly(transform, rigidBody, sprite, pSpecific);
		shoot(entity, transform, rigidBody, sprite, timer);
	}
}

void HealthSystem::update() {
	for (auto const& entity : mEntities) {
		auto& _health = G::gCoordinator.GetComponent<Health>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		auto& timerComponent = G::gCoordinator.GetComponent<TimerComponent>(entity);
		auto& entitySpecific = G::gCoordinator.GetComponent<EntitySpecific>(entity);

		if (entitySpecific.id == ID_ENTITY::ENEMY_ID) {
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
			if (entitySpecific.id == ID_ENTITY::ENEMY_ID) {
				G::gEnemyCounter--;
			}
			G::gEntitySetToBeDestroyed.insert(entity);
		}
	}
}

void CollisionSystem::update() {
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
				if (entitySpecific1.id == ID_ENTITY::ENEMY_ID && entitySpecific2.id == ID_ENTITY::PLAYER_ID) {
					if (!health2.isDamaged) {
						health2.healthToSubstract = damage1.damageOnContact;
						health2.toBeDamaged = true;
					}
					health2.isDamaged = true;
				}
				else if (entitySpecific1.id == ID_ENTITY::ENEMY_ID && entitySpecific2.id == ID_ENTITY::PLAYER_BULLET_ID) {
					if (!health1.isDamaged) {
						health1.healthToSubstract = damage2.damageOnContact;
						health1.toBeDamaged = true;
					}
					health1.isDamaged = true;
					health2.health = 0.0f;
				}

				if (entitySpecific1.id == ID_ENTITY::ENEMY_ID && entitySpecific2.id == ID_ENTITY::ENEMY_ID) {
					//handling collison here bruh
					raylib::AABBcollisionResponse(transform1.position, transform2.position, rigidBody1.hitbox.hitboxRect, rigidBody2.hitbox.hitboxRect);

				}
			}
		}
	}
}

void EnemySpawningSystem::update() {
	for (auto const& entity : mEntities) {
		auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& pSpecific = G::gCoordinator.GetComponent<PlayerSpecific>(entity);

		if (G::gEnemyCounter == 0) {
			if (IsKeyPressed(KeyInputs::NEXT_WAVE) || G::gLevel == 0) {
				G::gLevel++;
				G::gEnemyCounter = G::gLevel;

				const int BOUNDS = 400;
				
				raylib::Vector2 vP = G::gPlayerPos;
				std::uniform_int_distribution distribX(static_cast<int>(vP.x) - BOUNDS, static_cast<int>(vP.x) + BOUNDS);
				std::uniform_int_distribution distribY(static_cast<int>(vP.y) - BOUNDS, static_cast<int>(vP.y) + BOUNDS);

				for (uint32_t i = 0; i < G::gEnemyCounter; i++) {
					Vector2 v = { (float)distribX(gen), (float)distribY(gen) };

					while (Vector2Distance(vP, v) < 300 || !G::gridRect.CheckCollision(v)) {
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
						.frameImmunityTime = 2,
						.healthToSubstract = 0.0f,
						.toBeDamaged = false
						});
					G::gCoordinator.AddComponent<Enemy>(entity1, Enemy{

						});
					ComponentCommons::addComponent<MovmentAI>(entity1, false, false);
					G::gCoordinator.AddComponent<EntitySpecific>(entity1, EntitySpecific{
					.id = ID_ENTITY::ENEMY_ID
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

void EnemyAIMovmentSystem::update() {
	for (auto const& entity : mEntities) {
		auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		auto& movmentAI = G::gCoordinator.GetComponent<MovmentAI>(entity);
		//auto& enemy = GLOBALS::gCoordinator.GetComponent<Enemy>(entity);

		MovmentAI::chase(transform.position, G::gPlayerPos, rigidBody.velocity, sprite.angle, 50.0f);
	}
}

void BulletManipulationSystem::update() {
	for (auto const& entity : mEntities) {
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		auto& health = G::gCoordinator.GetComponent<Health>(entity);
		auto& movmentAI = G::gCoordinator.GetComponent<MovmentAI>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		auto& justBorn = G::gCoordinator.GetComponent<JustBorn>(entity);
		auto& bullet = G::gCoordinator.GetComponent<Bullet>(entity);

		movmentAI.moveInLineOfFunc(500.0f, entity, std::vector<float>{DEG2RAD * sprite.angle}, MovmentAI::straightLineFunc);
		//movmentAI.moveInLineOfFunc(200.0f, entity, std::vector<float>{300.f, 100.f}, MovmentAI::sinwaveExactFunc);

		if (!raylib::containsRect(G::gridRect, rigidBody.hitbox.hitboxRect)) {
			health.health = 0.0f;
		}
	}
}

void EntityRemovalSystem::update() {
	for (auto const& entity : G::gEntitySetToBeDestroyed) {
		G::gCoordinator.DestroyEntity(entity);
	}

	G::gEntitySetToBeDestroyed.clear();
	spatial_hash::gGird.clearAllTiles();

	for (auto const& entity : mEntities) {
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		spatial_hash::gGird.insert(entity, rigidBody.hitbox.hitboxRect);
	}
}