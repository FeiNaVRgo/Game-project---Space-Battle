#pragma once
#include "WeaponEnumeraions.h"

#include "../globals.h"
#include "../spatialHash/grid.h"

#include "../Components.h"
#include "../Components/Inventory.h"
#include "../Components/Sprite.h"
#include "../Components/Transforms.h"
#include "../Components/EntitySpecific.h"
#include "../Components/Damage.h"
#include "../Components/TimerComponent.h"
#include "../Components/RigidBody.h"
#include "../Components/Health.h"
#include "../Components/Bullet.h"
#include "../Components/MovmentAI.h"


template<size_t N>
struct StringLiteral {
	constexpr StringLiteral(const char(&str)[N]) {
		std::copy_n(str, N, value);
	}

	char value[N];
};

struct Inventory;
struct WeaponMini;

class ICreation {
public:
	inline std::optional<raylib::Vector2> findEmptySlot(Inventory& inv, raylib::Texture2DUnmanaged const& sprite, ECS::Entity WeaponMini) {
		for (auto& slot : inv.allSlots) {
			if (slot.slotPurpuse == Inventory::SLOT_PURPOSE::SLOT_IVNENTORY && slot.ptrItem == nullptr) {
				slot.ptrItem = std::make_shared<ECS::Entity>(WeaponMini);
				return std::optional<raylib::Vector2>{ slot.position + raylib::Vector2(sprite.GetSize()) * 0.5f };
			}
		}
		return std::nullopt;
	}

	virtual void createMini() = 0;
	virtual void createNormal(Inventory& inv, WeaponMini& weaponMini) = 0;
};

template
<
	class T, 
	StringLiteral name, 
	StringLiteral desc, 
	ID_WEAPON id_weapon,
	ID_WEAPON_RARITY id_rarity,
	raylib::Texture2DUnmanaged const& texture_mini,
	raylib::Texture2DUnmanaged const& texture_normal,
	raylib::Texture2DUnmanaged const& bullet_sprite,
	Damage damage
> 
class BasicCreation : public ICreation {//less go
public:
	inline void createMini() override {

		auto const& mini_weapon_entity = G::gCoordinator.CreateEntity();
		auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

		G::gCoordinator.AddComponent<WeaponType>(mini_weapon_entity, WeaponType{
			.id = ID_WEAPON_TYPE::MINI
			});
		G::gCoordinator.AddComponent<WeaponMini>(mini_weapon_entity, WeaponMini{
			.isHeld = false,
			.isEquipped = false,
			.rarity = id_rarity,
			.name = name.value,
			.description = desc.value,
			.id = id_weapon
			});
		G::gCoordinator.AddComponent<Sprite>(mini_weapon_entity, Sprite{
			.sprite = texture_mini,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(texture_mini.GetSize()) * 0.5f,
			.isDependent = true
			});
		G::gCoordinator.AddComponent<EntitySpecific>(mini_weapon_entity, EntitySpecific{
			.id = ID_ENTITY::WEAPON_ID
			});

		auto p_pos = findEmptySlot(inventory, texture_mini, mini_weapon_entity);
		if (!p_pos.has_value()) {
			G::gEntitySetToBeDestroyed.insert(mini_weapon_entity);
			return;
		}

		G::gCoordinator.AddComponent<Transforms>(mini_weapon_entity, Transforms{
			.position = p_pos.value(),
			.rotation = {0.0f, 0.0f},
			.scale = {1.0f, 1.0f}
			});
		auto& wp = G::gCoordinator.GetComponent<WeaponMini>(mini_weapon_entity);
		wp.posToStay = p_pos.value();
	}

	inline void createNormal(Inventory& inv, WeaponMini& weaponMini) override {
		auto const& canonNormal = G::gCoordinator.CreateEntity();
		
		G::gCoordinator.AddComponent<WeaponType>(canonNormal, WeaponType{
			.id = ID_WEAPON_TYPE::NORMAL
			});
		G::gCoordinator.AddComponent<Sprite>(canonNormal, Sprite{
			.sprite = texture_normal,
			.angle = 0.0f,
			.tint {255, 255, 255, 255},
			.origin = raylib::Vector2(texture_normal.GetSize()) * 0.5f,
			.isDependent = false
			});
		G::gCoordinator.AddComponent<Transforms>(canonNormal, Transforms{
			.position = G::gPlayerPos,
			.rotation {0.0f, 0.0f},
			.scale {1.0f, 1.0f}
			});
		G::gCoordinator.AddComponent<Damage>(canonNormal, Damage{
			damage
			});
		G::gCoordinator.AddComponent<WeaponNormal>(canonNormal, WeaponNormal{
			.bulletSprite = bullet_sprite,
			.id = id_weapon
			});
		G::gCoordinator.AddComponent<EntitySpecific>(canonNormal, EntitySpecific{
			.id = ID_ENTITY::WEAPON_ID
			});
		G::gCoordinator.AddComponent<TimerComponent>(canonNormal, TimerComponent{});

		weaponMini.ptrWeaponNormal = std::make_shared<ECS::Entity>(canonNormal);
	}
};

template<class T> class BasicVarModifier {
public:
	void modify(){}
};

template<class T, ID_ENTITY id_entity_target, float see_radius> 
class ClosestEnemy {
public:
	std::optional<ECS::Entity> detect(ECS::Entity weaponNormalEntity) {
		auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
		return spatial_hash::gGird.queryNearestEntityById(transforms_weapon.position, see_radius, id_entity_target);
	}
};

class IBasicShooting {
public:
	inline void entityAngleToPos(raylib::Vector2 const& entityPos, float& angle, raylib::Vector2 const& pos) {
		float distance = Vector2Distance(entityPos, pos);
		raylib::Vector2 v = { (pos.x - entityPos.x) / distance,
							  (pos.y - entityPos.y) / distance };
		float angle_temp = atan2f(v.x, -v.y);

		angle = angle_temp;
	}

	virtual void shoot(ECS::Entity weaponNormalEntity, std::optional<ECS::Entity>& optEnt) = 0;
};

template<class T, long long fire_rate> 
class BasicShooting : public IBasicShooting {
public:
	void Idle(float& angle) {
		angle = 0;
	}

	void shoot(ECS::Entity weaponNormalEntity, std::optional<ECS::Entity>& optEnt) override {
		auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
		auto& sprite_weapon = G::gCoordinator.GetComponent<Sprite>(weaponNormalEntity);
		auto& timer_weapon = G::gCoordinator.GetComponent<TimerComponent>(weaponNormalEntity);
		auto& normal_weapon = G::gCoordinator.GetComponent<WeaponNormal>(weaponNormalEntity);

		timer_weapon.timerCont.insertTimer(1, Timer(fire_rate, TIMER_ID::WAITTIMER_ID));

		if (optEnt.has_value()) {
			auto& rigidBody_targetedEntity = G::gCoordinator.GetComponent<RigidBody>(optEnt.value());
			auto const& entityCenter = rigidBody_targetedEntity.hitbox.getHitBoxCenter();

			entityAngleToPos(transforms_weapon.position, sprite_weapon.angle, entityCenter);

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
			Idle(sprite_weapon.angle);
		}
	}
};

template
<
	template <class T> class P_VarModifier,
	template <class T> class P_Detecting,
	template <class T> class P_Shooting
>
class Behaviour 
	: public P_VarModifier<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
	, public P_Detecting<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
	, public P_Shooting<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
{
	using Beh = Behaviour<P_VarModifier, P_Detecting, P_Shooting>;
	using VarModifier = P_VarModifier<Beh>;
	using Detecting = P_Detecting<Beh>;
	using Shooting = P_Shooting<Beh>;

public:
	void behaviour(ECS::Entity weaponNormalEntity) {
		VarModifier::modify();
		Shooting::shoot(weaponNormalEntity, Detecting::detect(weaponNormalEntity));;
	}
};

template
< 
	template <class T> class P_Creation,
	template <class T> class P_VarModify,
	template <class T> class P_Detection,
	template <class T> class P_Shooting
	
>
class Weapon  
	: private P_Creation<Weapon<P_Creation, P_VarModify, P_Detection, P_Shooting>>
	, private Behaviour<P_VarModify, P_Detection, P_Shooting>
{
private:
	using Wpn = Weapon<P_Creation, P_VarModify, P_Detection, P_Shooting>;
	using Creation = P_Creation<Wpn>;

public:
	void p_CreateMini() {
		Creation::createMini();
	}

	void p_CreateNormal(Inventory& inv, WeaponMini& weaponMini) {
		Creation::createNormal(inv, weaponMini);
	}

	void p_Behaviour(ECS::Entity weaponNormalEntity) {
		//Behaviour::behaviour(weaponNormalEntity);
		this->behaviour(weaponNormalEntity);
	}
};