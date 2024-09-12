#include "./Weapons.h"
#include "../Components.h"
#include "../spatialHash/grid.h"

inline void WEAPON_DEFINITIONS::WEAPON_COMMONS::entityAngleToPos(raylib::Vector2 const& entityPos, float& angle, raylib::Vector2 const& pos) {
	float distance = Vector2Distance(entityPos, pos);
	raylib::Vector2 v = { (pos.x - entityPos.x) / distance,
						  (pos.y - entityPos.y) / distance };
	float angle_temp = atan2f(v.x, -v.y);

	angle = angle_temp;
}\

inline std::optional<raylib::Vector2> WEAPON_DEFINITIONS::WEAPON_COMMONS::findEmptySlot(Inventory& inv, raylib::Texture2DUnmanaged const& sprite, ECS::Entity WeaponMini) {
	for (auto& slot : inv.allSlots) {
		if (slot.slotPurpuse == Inventory::SLOT_PURPOSE::SLOT_IVNENTORY && slot.ptrItem == nullptr) {
			slot.ptrItem = std::make_shared<ECS::Entity>(WeaponMini);
			return std::optional<raylib::Vector2>{ slot.position + raylib::Vector2(sprite.GetSize()) * 0.5f };
		}
	}
	return std::nullopt;
}

inline void WEAPON_DEFINITIONS::CANON::createWeaponMini() {
	auto const& canonMini = G::gCoordinator.CreateEntity();
	auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

	G::gCoordinator.AddComponent<WeaponType>(canonMini, WeaponType{
		.id = ID_WEAPON_TYPE::MINI
		});
	G::gCoordinator.AddComponent<WeaponMini>(canonMini, WeaponMini{
		.isHeld = false,
		.isEquipped = false,
		.rarity = ID_WEAPON_RARITY::COMMON,
		.name = "OMNIPOTENT CANON",
		.description = "This canon transcendents all of universe",
		.id = ID_WEAPON::ID_CANON
		});
	G::gCoordinator.AddComponent<Sprite>(canonMini, Sprite{
		.sprite = G::weapon_mini_canon,
		.angle = 0.0f,
		.tint {255, 255, 255, 255},
		.origin = raylib::Vector2(G::weapon_mini_canon.GetSize()) * 0.5f,
		.isDependent = true
		});
	G::gCoordinator.AddComponent<EntitySpecific>(canonMini, EntitySpecific{
		.id = ID_ENTITY::WEAPON_ID
		});

	auto p_pos = WEAPON_DEFINITIONS::WEAPON_COMMONS::findEmptySlot(inventory, G::weapon_mini_canon, canonMini);
	if (!p_pos.has_value()) {
		G::gEntitySetToBeDestroyed.insert(canonMini);
		return;
	}

	G::gCoordinator.AddComponent<Transforms>(canonMini, Transforms{
		.position = p_pos.value(),
		.rotation = {0.0f, 0.0f},
		.scale = {1.0f, 1.0f}
		});
	auto& wp = G::gCoordinator.GetComponent<WeaponMini>(canonMini);
	wp.posToStay = p_pos.value();
}

inline void WEAPON_DEFINITIONS::CANON::createWeaponNormal(Inventory& inv, WeaponMini& weaponMini) {
	auto const& canonNormal = G::gCoordinator.CreateEntity();
	//sprite
	//transforms
	//weaponNormal
	//damage
	G::gCoordinator.AddComponent<WeaponType>(canonNormal, WeaponType{
		.id = ID_WEAPON_TYPE::NORMAL
		});
	G::gCoordinator.AddComponent<Sprite>(canonNormal, Sprite{
		.sprite = G::weapon_normal_canon,
		.angle = 0.0f,
		.tint {255, 255, 255, 255},
		.origin = raylib::Vector2(G::weapon_normal_canon.GetSize()) * 0.5f,
		.isDependent = false
		});
	G::gCoordinator.AddComponent<Transforms>(canonNormal, Transforms{
		.position = G::gPlayerPos,
		.rotation {0.0f, 0.0f},
		.scale {1.0f, 1.0f}
		});
	G::gCoordinator.AddComponent<Damage>(canonNormal, Damage{
		.damage = 25.f,
		.damageOnContact = 25.f,
		.damageType = Damage::DAMAGE_TYPE::PHYSICAL
		});
	G::gCoordinator.AddComponent<WeaponNormal>(canonNormal, WeaponNormal{
		.bulletSprite = G::playerBulletTexture1,
		.id = ID_WEAPON::ID_CANON
		});
	G::gCoordinator.AddComponent<EntitySpecific>(canonNormal, EntitySpecific{
		.id = ID_ENTITY::WEAPON_ID
		});
	G::gCoordinator.AddComponent<TimerComponent>(canonNormal, TimerComponent{});

	weaponMini.ptrWeaponNormal = std::make_shared<ECS::Entity>(canonNormal);
}

inline void WEAPON_DEFINITIONS::CANON::behaviourWeaponNormal(ECS::Entity weaponNormalEntity) {
	auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
	auto& sprite_weapon = G::gCoordinator.GetComponent<Sprite>(weaponNormalEntity);
	auto& timer_weapon = G::gCoordinator.GetComponent<TimerComponent>(weaponNormalEntity);
	auto& normal_weapon = G::gCoordinator.GetComponent<WeaponNormal>(weaponNormalEntity);

	timer_weapon.timerCont.insertTimer(1, Timer(800, TIMER_ID::WAITTIMER_ID));

	auto targetedEntity = spatial_hash::gGird.queryNearestEntityById(transforms_weapon.position, 800.0f, ID_ENTITY::ENEMY_ID);

	if (targetedEntity.has_value()) {
		auto& rigidBody_targetedEntity = G::gCoordinator.GetComponent<RigidBody>(targetedEntity.value());
		auto const& entityCenter = rigidBody_targetedEntity.hitbox.getHitBoxCenter();

		WEAPON_DEFINITIONS::WEAPON_COMMONS::entityAngleToPos(transforms_weapon.position, sprite_weapon.angle, entityCenter);
		
		if (timer_weapon.timerCont.checkTimer(1)) {
			timer_weapon.timerCont.resetTimer(1);
			//TODO -- creation of bullets should be in seperate function
			
			ENTITY_CREATION_FUNCTIONS::createBullet(weaponNormalEntity,
				Transforms{
				.position = transforms_weapon.position, 
				.rotation = raylib::Vector2(0.0f, 0.0f),
				.scale = raylib::Vector2(1.0f, 1.0f),
				},
				RigidBody{
				.velocity = raylib::Vector2(0.0f, 0.0f),
				.acceleration = raylib::Vector2(0.0f, 0.0f),
				.hitbox = {{0.0f, 0.0f, 18.0f, 18.0f}, raylib::Color::Green()},
				.isColliding = false,
				.onWhatSideIsColliding = {false ,false ,false ,false }
				},
				Sprite{
				.sprite = normal_weapon.bulletSprite,
				.angle = sprite_weapon.angle,
				.tint = {255, 255, 255, 255},
				.origin = raylib::Vector2(normal_weapon.bulletSprite.width * 0.5f, normal_weapon.bulletSprite.height * 0.5f)
				},
				Health{
			   .maxHealth = 5.0f,
			   .health = 5.0f,
			   .isDamaged = false,
			   .frameImmunityTime = 1,
			   .healthToSubstract = 0.0f,
			   .toBeDamaged = false
				},
				Bullet{
				},
				EntitySpecific{
				.id = ID_ENTITY::PLAYER_BULLET_ID
				},
				TimerComponent{
				},
				Damage{
					.damage = 5.f,
					.damageOnContact = 5.f
				},
				MovmentAI{
					.DistanceH = false,
					.DistanceW = false
				}
			);
		}
	}
	else {
		sprite_weapon.angle = 0.0f;
	}
}

inline void WEAPON_DEFINITIONS::MINIGUN::createWeaponMini() {}

inline void WEAPON_DEFINITIONS::MINIGUN::createWeaponNormal(Inventory& inv, WeaponMini& weaponMini) {}

inline void WEAPON_DEFINITIONS::MINIGUN::behaviourWeaponNormal(ECS::Entity weaponNormalEntity) {}