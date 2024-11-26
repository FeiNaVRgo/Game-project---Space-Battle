#pragma once
#include "../../WeaponEnumeraions.h"

#include "../../../globals.h"
#include "../../../spatialHash/grid.h"

#include "../../../Components/Transforms.h"
#include "../../../Components/EntitySpecific.h"


template<class T, ID_ENTITY id_entity_target, float see_radius>
class ClosestTarget {
public:
	std::optional<ECS::Entity> detect(ECS::Entity weaponNormalEntity) {
		auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
		return spatial_hash::gGird.queryNearestEntityById(transforms_weapon.position, see_radius, id_entity_target);
	}
};