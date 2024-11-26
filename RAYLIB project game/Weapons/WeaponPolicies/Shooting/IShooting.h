#pragma once
#include "../../WeaponEnumeraions.h"

#include "../../../globals.h"
#include "../../../spatialHash/grid.h"

#include "../../../Components.h"
#include "../../../Components/Inventory.h"
#include "../../../Components/Sprite.h"
#include "../../../Components/Transforms.h"
#include "../../../Components/EntitySpecific.h"
#include "../../../Components/Damage.h"
#include "../../../Components/TimerComponent.h"
#include "../../../Components/RigidBody.h"
#include "../../../Components/Health.h"
#include "../../../Components/Bullet.h"
#include "../../../Components/MovmentAI.h"

class IShooting {
public:
	inline void entityAngleToPos(raylib::Vector2 const& entityPos, float& angle, raylib::Vector2 const& pos) {
		float distance = Vector2Distance(entityPos, pos);
		raylib::Vector2 v = { (pos.x - entityPos.x) / distance,
							  (pos.y - entityPos.y) / distance };
		float angle_temp = atan2f(v.x, -v.y);

		angle = angle_temp;
	}

	virtual void shoot(ECS::Entity weaponNormalEntity, std::optional<ECS::Entity> optEnt) = 0;
};