#pragma once
#include <src/raylib-cpp.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "../ECS.h"

#define INVENTORY_WIDTH 200

struct Inventory {
	enum class SLOT_IMPL {
		ACTIVE = 0,
		NONACTIVE = 1
	};

	enum class SLOT_STATE {
		STATE_EMPTY = 0,
		STATE_OCCUPIED = 1
	};

	enum class SLOT_PURPOSE {
		SLOT_CORE = 0,
		SLOT_WEAPON = 1,
		SLOT_AMMO = 2,
		SLOT_IVNENTORY = 3
	};

	struct SlotDef {
		SLOT_IMPL slotImpl{};
		SLOT_STATE slotState{};
		SLOT_PURPOSE slotPurpuse{};
		
		raylib::Vector2 dimensions;
		raylib::Color slotCol;

		std::shared_ptr<ECS::Entity> ptrItem{};

		raylib::Vector2 position;
	};

	std::vector<SlotDef> allSlots;
	uint16_t invSize = 0;
	uint16_t weaponSize = 0;
	uint16_t ammoSize = 0;

	raylib::Rectangle inventoryBase;

	Inventory();
private:
	void populateWeapons();
	void populateInventory();
	void populateAllSlots();
	void addSlotInventory();
	void addSlotWeapon();
	void DrawSlot(const SlotDef& slot);
	void DrawSlotOutline(const SlotDef& slot);
	void DrawVecSlot(const std::vector<SlotDef>& vecSlot, bool drawOutline, bool drawSlot = true);
	void SetSlotsPos();
	void InteractWithSlot(SlotDef& slot);
	void swapItemSlots(SlotDef& slot1, SlotDef& slot2);
	void copySlotPtr(SlotDef& slot, std::shared_ptr<SlotDef> target);
	void copySlotPtr(SlotDef& slot, SlotDef* target);
	void copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<std::shared_ptr<SlotDef>>& target);
	void copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<SlotDef*>& target);
public:
	void moveUptrItem(std::vector<SlotDef>& vecSlot, ECS::Entity entity);
	void DrawSprites();
	void DrawAllSlots();
	void InteractWithSlots();
	void updateSlots();
};
