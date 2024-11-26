#pragma once
#include "../../WeaponEnumeraions.h"

#include "../../../globals.h"

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