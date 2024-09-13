#include "Inventory.h"

#include "Sprite.h"

#include "../globals.h"
#include "../GeometryCalc.hpp"
#include "Transforms.h"
#include "Sprite.h"
#include "../Weapons/Weapons.h"

Inventory::Inventory() {
	inventoryBase = { G::screenWidth - INVENTORY_WIDTH, 0, INVENTORY_WIDTH, G::screenHeight };
	populateAllSlots();
	SetSlotsPos();
}

void Inventory::populateWeapons() {
	for (int i = 0; i < 4; i++) {
		addSlotWeapon();
	}
}

void Inventory::populateInventory() {
	for (int i = 0; i < 12; i++) {
		addSlotInventory();
	}
}

void Inventory::populateAllSlots() {
	allSlots.push_back({ SLOT_IMPL::ACTIVE, SLOT_STATE::STATE_EMPTY, SLOT_PURPOSE::SLOT_CORE, {30.f, 30.f}, {0, 0, 0, 50}, nullptr, { 0.f, 0.f } });
	populateWeapons();
	populateInventory();
}

void Inventory::addSlotInventory() {
	allSlots.push_back({ SLOT_IMPL::NONACTIVE, SLOT_STATE::STATE_EMPTY, SLOT_PURPOSE::SLOT_IVNENTORY, { 30.0f, 30.0f }, { 0, 0, 0, 50 }, nullptr, { 0.f, 0.f } });
	invSize++;
}

void Inventory::addSlotWeapon() {
	allSlots.push_back({ SLOT_IMPL::ACTIVE, SLOT_STATE::STATE_EMPTY, SLOT_PURPOSE::SLOT_WEAPON, { 30.f, 30.f }, { 0, 0, 0, 50 }, nullptr, { 0.f, 0.f } });
	weaponSize++;
}

void Inventory::DrawSlot(const SlotDef& slot) {
	raylib::Rectangle(slot.position, slot.dimensions).Draw(slot.slotCol);
}

void Inventory::DrawSlotOutline(const SlotDef& slot) {
	raylib::Rectangle(slot.position, slot.dimensions).DrawLines(slot.slotCol);//TODO --diffrent colors for outline
}

void Inventory::DrawVecSlot(const std::vector<SlotDef>& vecSlot, bool drawOutline, bool drawSlot) {
	for (const auto& slot : vecSlot) {
		if (drawSlot) {
			DrawSlot(slot);
		}

		if (drawOutline) {
			DrawSlotOutline(slot);
		}
	}
}

void Inventory::SetSlotsPos() {
	float r = 70.0f;


	for (auto& slot : allSlots) {
		if (slot.slotPurpuse == SLOT_PURPOSE::SLOT_CORE) {
			slot.position = raylib::getCenterRect(inventoryBase) - slot.dimensions / 2.0f;
			break;
		}
	}
	//slotCore.position = raylib::getCenterRect(inventoryBase) - slotCore.dimensions / 2.0f;


	int weaponCount = 0;
	for (auto it = allSlots.begin(); it != allSlots.end(); ++it) {
		//auto i = std::distance(allSlots.begin(), it);
		if (it->slotPurpuse == SLOT_PURPOSE::SLOT_WEAPON) {
			float angle = DEG2RAD * (static_cast<float>(weaponCount) * 360.0f / static_cast<float>(weaponSize));

			it->position = raylib::Vector2{ std::floor(r * cosf(angle)), std::floor(r * sinf(angle)) } + raylib::getCenterRect(inventoryBase) - it->dimensions / 2.0f;
			weaponCount++;
		}
	}

	float totalWidth = 0.f;
	float y = 0.f;//for the current level of lines in inventory
	float maxWidth = 0.f;//for later centering of position in invBase
	for (auto it = allSlots.begin(); it != allSlots.end(); ++it) {
		//auto i = std::distance(slotsInv.begin(), it); //just in case when i want to change something
		if (it->slotPurpuse == SLOT_PURPOSE::SLOT_IVNENTORY) {
			if (totalWidth + it->dimensions.x >= INVENTORY_WIDTH) {//without additiion it fucks up
				maxWidth = totalWidth;
				totalWidth = 0.0f;
				y++;
			}

			it->position = { totalWidth, y * it->dimensions.y };
			totalWidth += it->dimensions.x;
		}
	}

	for (auto& slot : allSlots) {
		if (slot.slotPurpuse == SLOT_PURPOSE::SLOT_IVNENTORY) {
			slot.position.x += inventoryBase.GetPosition().x + (INVENTORY_WIDTH - maxWidth) / 2.0f;
			slot.position.y += allSlots.at(0).position.y + r + 60.f;//at(0) means slot core
		}
	}
}

void Inventory::InteractWithSlot(SlotDef& slot) {
	if (slot.ptrItem != nullptr) {
		auto& miniWeapon = G::gCoordinator.GetComponent<WeaponMini>(*slot.ptrItem);
		auto const& sprite = G::gCoordinator.GetComponent<Sprite>(*slot.ptrItem);
		auto const& transform = G::gCoordinator.GetComponent<Transforms>(*slot.ptrItem);

		if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT) && miniWeapon.isSelected) {
			miniWeapon.isSelected = false;
			miniWeapon.afterSelecting = true;
		}

		if (raylib::Rectangle(slot.position, slot.dimensions).CheckCollision(GetMousePosition()) && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
			miniWeapon.isSelected = true;
		}
	}
}

void Inventory::swapItemSlots(SlotDef& slot1, SlotDef& slot2) {
	auto temp_slot = slot2;
	slot2.ptrItem = slot1.ptrItem;
	slot1.ptrItem = temp_slot.ptrItem;
}

void Inventory::moveUptrItem(std::vector<SlotDef>& vecSlot, ECS::Entity entity) {
	auto& miniWeapon = G::gCoordinator.GetComponent<WeaponMini>(entity);
	auto& transforms = G::gCoordinator.GetComponent<Transforms>(entity);
	auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
	auto& inventory = G::gCoordinator.GetComponent<Inventory>(G::player);

	//TODO -- fix memory leak

	auto spriteRect = raylib::Rectangle(transforms.position - sprite.origin, sprite.sprite.GetSize());
	Inventory::SlotDef* t_slot = new Inventory::SlotDef;

	for (auto& slot : allSlots) {
		if (slot.ptrItem != nullptr && *(slot.ptrItem) == entity) {
			t_slot = &slot;

			//takes care of deleting normal weapon from slot_weappon - creawting is takken care in vibecheck function
			//yippe
			if (slot.slotPurpuse == Inventory::SLOT_PURPOSE::SLOT_WEAPON) {
				auto& weaponMini = G::gCoordinator.GetComponent<WeaponMini>(*slot.ptrItem);
				if (weaponMini.isNormalInWorld) {
					auto& weaponNormal = G::gCoordinator.GetComponent<WeaponNormal>(*weaponMini.ptrWeaponNormal);
					G::gEntitySetToBeDestroyed.insert(*weaponMini.ptrWeaponNormal);
					weaponMini.ptrWeaponNormal = nullptr;
					weaponMini.isNormalInWorld = false;
				}
			}
		}
	}

	for (auto& slot : vecSlot) {
		if (raylib::areaOfRectangle(raylib::Rectangle(slot.position, slot.dimensions).GetCollision(spriteRect)) >= 0.5f * raylib::areaOfRectangle(spriteRect)) {
			miniWeapon.posToStay = slot.position + raylib::Vector2(sprite.sprite.GetSize()) * 0.5f;
			miniWeapon.afterSelecting = false;

			inventory.swapItemSlots(*t_slot, slot);

			break;
		}
	}

	//delete t_slot;
}

void Inventory::copySlotPtr(SlotDef& slot, std::shared_ptr<SlotDef> target) {
	target = std::make_shared<SlotDef>(slot);
}

void Inventory::copySlotPtr(SlotDef& slot, SlotDef* target) {
	target = &slot;
}

void Inventory::copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<std::shared_ptr<SlotDef>>& target) {
	for (auto const& slot : slotVec) {
		target.push_back(std::make_shared<SlotDef>(slot));
	}
}

void Inventory::copySlotVecPtr(std::vector<SlotDef>& slotVec, std::vector<SlotDef*>& target) {
	for (auto& slot : slotVec) {
		target.push_back(&slot);
	}
}

void Inventory::DrawSprites() {
	EndMode2D();
	for (auto& slot : allSlots) {
		if (slot.ptrItem != nullptr) {
			auto _It = *slot.ptrItem;

			auto& sprite = G::gCoordinator.GetComponent<Sprite>(_It);
			sprite.draw(_It);
		}
	}
	BeginMode2D(G::camera);
}

void Inventory::DrawAllSlots() {
	if (G::debugMode) {
		auto v = raylib::getCenterRect(inventoryBase);
		DrawCircleLines(v.x, v.y, 70.0f, RED);
	}

	inventoryBase.Draw({ 0, 0, 0, 100 });
	DrawVecSlot(allSlots, true);
}

void Inventory::InteractWithSlots() {
	for (auto& slot : allSlots) {
		InteractWithSlot(slot);
	}
}

void Inventory::updateSlots() {
	//TODO
}