#pragma once
#include "IShooting.h"
#include "../../Weapons.h"



template<class T, long long fire_rate>
class BasicShooting : public IShooting {
public:
	void Idle(float& angle) {
		angle = 0;
	}

	void shoot(ECS::Entity weaponNormalEntity, std::optional<ECS::Entity> optEnt) override {
		auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
		auto& sprite_weapon = G::gCoordinator.GetComponent<Sprite>(weaponNormalEntity);
		auto& timer_weapon = G::gCoordinator.GetComponent<TimerComponent>(weaponNormalEntity);
		auto& normal_weapon = G::gCoordinator.GetComponent<WeaponNormal>(weaponNormalEntity);
		auto& damage_weapon = G::gCoordinator.GetComponent<Damage>(weaponNormalEntity);

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
						.damage = damage_weapon.damage,
						.damageOnContact = damage_weapon.damageOnContact,
						.damageType = damage_weapon.damageType
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