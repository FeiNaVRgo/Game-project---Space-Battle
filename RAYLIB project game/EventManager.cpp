#include "EventManager.h"
#include "globals.h"
#include "EntityIds.h"

#include "Components.h"
#include "Components/all.h"

void events::Actions::splitBullet(Target target, Args const& args) {
	auto& target_justborn		= G::gCoordinator.GetComponent<JustBorn>(target);
	auto& target_transform		= G::gCoordinator.GetComponent<Transforms>(target);
	auto& target_rigidbody		= G::gCoordinator.GetComponent<RigidBody>(target);
	auto& target_sprite			= G::gCoordinator.GetComponent<Sprite>(target);
	auto& target_health			= G::gCoordinator.GetComponent<Health>(target);
	auto& target_bullet			= G::gCoordinator.GetComponent<Bullet>(target);
	auto& target_entityspecific = G::gCoordinator.GetComponent<EntitySpecific>(target);
	auto& target_timercomponent = G::gCoordinator.GetComponent<TimerComponent>(target);
	auto& target_damage         = G::gCoordinator.GetComponent<Damage>(target);
	auto& target_movmentai		= G::gCoordinator.GetComponent<MovmentAI>(target);

	Damage nerfed_damage = target_damage * 0.5f;
	Health health_fixed = target_health;
	health_fixed.clear();

	for (int i = 0; i < 4; i++) {
		float angle = DEG2RAD * (static_cast<float>(i) * 360.0f / static_cast<float>(4));
		auto target_fixed_rigidbody = target_rigidbody;

		target_fixed_rigidbody.velocity = { sinf(angle), -cosf(angle) };

		ENTITY_CREATION_FUNCTIONS::createBullet(target_justborn.parent, target_transform, target_fixed_rigidbody, target_sprite, health_fixed, target_bullet, target_entityspecific, target_timercomponent, nerfed_damage, target_movmentai);
	}
}