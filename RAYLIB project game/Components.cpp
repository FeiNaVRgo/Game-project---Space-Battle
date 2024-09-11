#include "Components.h"
#include "spatialHash/grid.h"

inline void ComponentCommons::createBullet(ECS::Entity parent,
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

raylib::Vector2 Hitbox::getHitBoxCenter() {
	return hitboxRect.GetPosition() + hitboxRect.GetSize() * 0.5f;
}

inline void Sprite::draw(ECS::Entity entity) {
	auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
	auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);

	sprite.sprite.Draw(
		raylib::Rectangle(0.0f, 0.0f, sprite.sprite.width, sprite.sprite.height),
		raylib::Rectangle(transform.position.x, transform.position.y, sprite.sprite.width, sprite.sprite.height),
		raylib::Vector2(sprite.origin),
		sprite.angle * RAD2DEG,
		sprite.tint
	);
}

Sprite::~Sprite(){}

void Health::drawHealthBar(const raylib::Vector2& origin, const raylib::Vector2& offsetFromOrigin) {
	raylib::Rectangle r1 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(maxHealth, 4) };
	raylib::Rectangle r2 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(health, 4) - raylib::Vector2(1, 1) };
	r2.Draw(BLUE);
	r1.DrawLines(BLACK);
}

void Health::drawDamageIndicator(){}

Inventory::Inventory(){
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
		auto const& sprite= G::gCoordinator.GetComponent<Sprite>(*slot.ptrItem);
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

			std::cout << "here1\n";
		}
	}

	for (auto& slot : vecSlot) {
		if (raylib::areaOfRectangle(raylib::Rectangle(slot.position, slot.dimensions).GetCollision(spriteRect)) >= 0.5f * raylib::areaOfRectangle(spriteRect)) {
			miniWeapon.posToStay = slot.position + raylib::Vector2(sprite.sprite.GetSize()) * 0.5f;
			miniWeapon.afterSelecting = false;

			inventory.swapItemSlots(*t_slot, slot);

			std::cout << "here\n";

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
			Sprite::draw(*slot.ptrItem);
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

void  MovmentAI::chase(const raylib::Vector2& pos, const raylib::Vector2& posToChase, raylib::Vector2& velocity, float& angle, float speed, float seeRadius) {
	float distance = Vector2Distance(pos, posToChase);
	if (distance <= seeRadius) {
		raylib::Vector2 v = { (posToChase.x - pos.x) / distance,
							  (posToChase.y - pos.y) / distance };
		angle = atan2f(v.x, -v.y);

		velocity.x = sinf(angle) * speed;
		velocity.y = -cosf(angle) * speed;
	}
	else {
		velocity = { 0.0f, 0.0f };
	}
}

inline double MovmentAI::sinwaveExactFunc(double x, std::vector<float> params) {
	return params.at(1) * sinf(static_cast<float>(x * (PI / params.at(0))));
}

inline double MovmentAI::sinwaveFunc(double x, std::vector<float> params) {
	return params.at(1) * sinf(static_cast<float>(x) * params.at(0));
}

inline double MovmentAI::parabolicFunc(double x, std::vector<float> params) {
	return params.at(0) * x * x + params.at(1) * x;
}

inline double MovmentAI::parabolicExactFunc(double x, std::vector<float> params) {
	float T = params.at(0);
	float H = params.at(1);

	double c = T / 2.0f;
	double a = H / (c * c);
	double b = H;

	return -(a * (x - c) * (x - c)) + b;
}

inline double MovmentAI::straightLineFunc(double x, std::vector<float> params) {
	return params.at(0) * x;
}

void MovmentAI::moveInLineOfFunc(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f) {
	auto& born = G::gCoordinator.GetComponent<JustBorn>(anchor);
	auto& transform = G::gCoordinator.GetComponent<Transforms>(anchor);
	auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(anchor);
	auto& sprite = G::gCoordinator.GetComponent<Sprite>(anchor);

	auto vel2 = born.bornVel.Rotate(-PI / 2.0f);
	auto vel3 = born.bornVel.Rotate(PI / 2.0f);

	raylib::Ray rayBorn1(Vector3{ born.bornPos.x, born.bornPos.y, 0 }, Vector3{ born.bornVel.x, born.bornVel.y, 0.0f });
	raylib::Ray rayBorn2(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel2.x, vel2.y, 0.0f });
	raylib::Ray rayBorn3(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel3.x, vel3.y, 0.0f });

	if (G::debugMode) {
		rayBorn1.Draw(RED);
		rayBorn2.Draw(RED);
		rayBorn3.Draw(RED);
	}

	auto translatedX1 = raylib::intersectsRayToRay(rayBorn1, rayBorn2);
	auto translatedX2 = raylib::intersectsRayToRay(rayBorn1, rayBorn3);

	if (!translatedX1.empty()) {
		raylib::Vector2 vecTX{ translatedX1.front().x, translatedX1.front().y };
		if (G::debugMode)
			vecTX.DrawCircle(10, BLUE);

		auto x = Vector2Distance(vecTX, born.bornPos);
		sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

		rigidBody.velocity.x = sinf(sprite.angle) * speed;
		rigidBody.velocity.y = -cosf(sprite.angle) * speed;
	}
	else if (!translatedX2.empty()) {
		raylib::Vector2 vecTX{ translatedX2.front().x, translatedX2.front().y };
		if (G::debugMode)
			vecTX.DrawCircle(10, BLUE);

		auto x = Vector2Distance(vecTX, born.bornPos);
		sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

		rigidBody.velocity.x = sinf(sprite.angle) * speed;
		rigidBody.velocity.y = -cosf(sprite.angle) * speed;
	}
}

void MovmentAI::moveInLineOfFuncAndGoBack(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f, raylib::Vector2 whereToDestroy) {
	auto& born = G::gCoordinator.GetComponent<JustBorn>(anchor);
	auto& transform = G::gCoordinator.GetComponent<Transforms>(anchor);
	auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(anchor);
	auto& sprite = G::gCoordinator.GetComponent<Sprite>(anchor);

	auto vel2 = born.bornVel.Rotate(-PI / 2.0f);
	auto vel3 = born.bornVel.Rotate(PI / 2.0f);

	raylib::Ray rayBorn1(Vector3{ born.bornPos.x, born.bornPos.y, 0 }, Vector3{ born.bornVel.x, born.bornVel.y, 0.0f });
	raylib::Ray rayBorn2(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel2.x, vel2.y, 0.0f });
	raylib::Ray rayBorn3(Vector3{ transform.position.x, transform.position.y, 0 }, Vector3{ vel3.x, vel3.y, 0.0f });

	rayBorn1.Draw(RED);
	rayBorn2.Draw(RED);
	rayBorn3.Draw(RED);

	auto translatedX1 = raylib::intersectsRayToRay(rayBorn1, rayBorn2);
	auto translatedX2 = raylib::intersectsRayToRay(rayBorn1, rayBorn3);

	if (!translatedX1.empty()) {
		raylib::Vector2 vecTX{ translatedX1.front().x, translatedX1.front().y };
		vecTX.DrawCircle(10, BLUE);
		
		auto x = Vector2Distance(vecTX, born.bornPos);
		sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

		if (x < params.at(0) && transform.position.x != 9) {
			rigidBody.velocity.x = sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
		else {
			if (!DistanceH) {
				DistanceH = true;
			}
			rigidBody.velocity.x = -sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
	}
	else if (!translatedX2.empty()) {
		raylib::Vector2 vecTX{ translatedX2.front().x, translatedX2.front().y };
		vecTX.DrawCircle(10, BLUE);

		auto x = Vector2Distance(vecTX, born.bornPos);
		sprite.angle = atanf(raylib::derivative(f, x, params)) + born.bornAngle;

		if (x < params.at(0) && transform.position.x != 9) {
			rigidBody.velocity.x = sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
		else {
			if (!DistanceH) {
				DistanceH = true;
			}
			rigidBody.velocity.x = -sinf(sprite.angle) * speed;
			rigidBody.velocity.y = -cosf(sprite.angle) * speed;
		}
	}

	if (whereToDestroy.CheckCollision(rigidBody.hitbox.hitboxRect) && DistanceH) {
		G::gEntitySetToBeDestroyed.insert(anchor);
	}
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
			Sprite::draw(entity);
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

				if (weaponLibrary.weaponMap.contains(weaponMini.id) && weaponMini.isNormalInWorld == false) {
					weaponLibrary.weaponMap.at(weaponMini.id)(G::gCoordinator.GetComponent<Inventory>(G::player), weaponMini);
					weaponMini.isNormalInWorld = true;
				}
			}
			weaponSlotCount++;
		}
	}
}

inline void WeaponSystem::createWeaponNormalCanon(Inventory& inv, WeaponMini& weaponMini) {
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

inline void WeaponSystem::createWeaponMiniCanon() {
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
	raylib::Vector2 pos{};
	for (auto& slot : inventory.allSlots) {
		if (slot.slotPurpuse == Inventory::SLOT_PURPOSE::SLOT_IVNENTORY && slot.ptrItem == nullptr) {
			pos = slot.position + raylib::Vector2(G::weapon_mini_canon.GetSize()) * 0.5f;
			slot.ptrItem = std::make_shared<ECS::Entity>(canonMini);
			break;
		}
	}
	
	G::gCoordinator.AddComponent<Transforms>(canonMini, Transforms{
		.position = pos,
		.rotation = {0.0f, 0.0f},
		.scale = {1.0f, 1.0f}
		});
	auto& wp = G::gCoordinator.GetComponent<WeaponMini>(canonMini);
	wp.posToStay = pos;
}

inline void WeaponSystem::behaviourWeaponNormalCanon(ECS::Entity weaponNormalEntity) {
	auto const& transforms_weapon = G::gCoordinator.GetComponent<Transforms>(weaponNormalEntity);
	auto& sprite_weapon = G::gCoordinator.GetComponent<Sprite>(weaponNormalEntity);
	auto& timer_weapon = G::gCoordinator.GetComponent<TimerComponent>(weaponNormalEntity);
	auto& normal_weapon = G::gCoordinator.GetComponent<WeaponNormal>(weaponNormalEntity);
	
	timer_weapon.timerCont.insertTimer(1, Timer(800, TIMER_ID::WAITTIMER_ID));

	auto targetedEntity = spatial_hash::gGird.queryNearestEntityById(transforms_weapon.position, 800.0f, ID_ENTITY::ENEMY_ID);

	if (targetedEntity.has_value()) {
		auto& rigidBody_targetedEntity = G::gCoordinator.GetComponent<RigidBody>(targetedEntity.value());
		auto const& entityCenter = rigidBody_targetedEntity.hitbox.getHitBoxCenter();
		
		float distance = Vector2Distance(transforms_weapon.position, entityCenter);
		raylib::Vector2 v = { (entityCenter.x - transforms_weapon.position.x) / distance,
							  (entityCenter.y - transforms_weapon.position.y) / distance };
		float angle = atan2f(v.x, -v.y);

		sprite_weapon.angle = angle;
	
		if (timer_weapon.timerCont.checkTimer(1)) {
			timer_weapon.timerCont.resetTimer(1);
			//TODO -- creation of bullets should be in seperate function
			ComponentCommons::createBullet(weaponNormalEntity,
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
				.angle = angle,
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

WeaponLibrary::WeaponLibrary() {
	weaponMap.try_emplace(ID_WEAPON::ID_CANON, WeaponSystem::createWeaponNormalCanon);
	weaponBehaviourMap.try_emplace(ID_WEAPON::ID_CANON, WeaponSystem::behaviourWeaponNormalCanon);
}
 
void InputSystem::update() {
	for (auto const& entity : mEntities) {
		auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
		auto& pSpecific = G::gCoordinator.GetComponent<PlayerSpecific>(entity);

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
			WeaponSystem::createWeaponMiniCanon();
		}

		if (IsKeyPressed(KeyInputs::DASH) && pSpecific.dash <= 1.0f) {
			pSpecific.dash = 5.0f;
		}

		if (pSpecific.dash > 1.0f) {
			pSpecific.dash -= GetFrameTime() * 6.0f;
		}

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


		if (raylib::IsKeyOrMouseDown(KeyInputs::FLY)) {
			auto m_v = GetScreenToWorld2D(GetMousePosition(), G::camera);
			auto v = raylib::Vector2((m_v.x - transform.position.x) / Vector2Distance(transform.position, m_v),
				(m_v.y - transform.position.y) / Vector2Distance(transform.position, m_v));
			sprite.angle = atan2f(v.x, -v.y);
			rigidBody.velocity.x = sinf(sprite.angle) * 80.0f * pSpecific.dash;
			rigidBody.velocity.y = -cosf(sprite.angle) * 80.0f * pSpecific.dash;
		}

		if (IsKeyPressed(KeyboardKey::KEY_H)) {
			if (!G::debugMode) { G::debugMode = true; }
			else { G::debugMode = false; }
		}

		G::gTimer.insertTimer(1, Timer(400, TIMER_ID::WAITTIMER_ID));

		if (raylib::IsKeyOrMouseDown(KeyInputs::SHOOT) && G::gTimer.checkTimer(1)) {
			G::gTimer.resetTimer(1);
			ECS::Entity entity1 = G::gCoordinator.CreateEntity();
			G::gCoordinator.AddComponent<Transforms>(entity1, Transforms{
				.position = transform.position,
				.rotation = raylib::Vector2(0.0f, 0.0f),
				.scale = raylib::Vector2(1.0f, 1.0f)
				});
			G::gCoordinator.AddComponent<RigidBody>(entity1, RigidBody{
				.velocity = raylib::Vector2(3.0f * rigidBody.velocity.x / pSpecific.dash, 3.0f * rigidBody.velocity.y / pSpecific.dash),
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

				raylib::Vector2 vP = G::gPlayerPos;
				std::uniform_int_distribution distribX(static_cast<int>(vP.x) - 100, static_cast<int>(vP.x) + 100);
				std::uniform_int_distribution distribY(static_cast<int>(vP.y) - 100, static_cast<int>(vP.y) + 100);

				for (int i = 0; i < G::gEnemyCounter; i++) {
					Vector2 v = { distribX(gen), distribY(gen) };

					while (Vector2Distance(vP, v) < 70 || !G::gridRect.CheckCollision(v)) {
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
						.frameImmunityTime = 10,
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

		movmentAI.chase(transform.position, G::gPlayerPos, rigidBody.velocity, sprite.angle, 50.0f);
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

		movmentAI.moveInLineOfFunc(200.0f, entity, std::vector<float>{DEG2RAD * sprite.angle}, MovmentAI::straightLineFunc);
		//movmentAI.moveInLineOfFunc(200.0f, entity, std::vector<float>{500.f, 500.f}, MovmentAI::sinwaveExactFunc);

		if (!raylib::containsRect(G::gridRect, rigidBody.hitbox.hitboxRect)) {
			health.health = 0.0f;
		}
	}
}

void EntityRemovalSystem::update() {
	for (auto const& entity : G::gEntitySetToBeDestroyed) {
		//auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		//auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);

		//sprite.sprite.Unload();
		G::gCoordinator.DestroyEntity(entity);
	}

	G::gEntitySetToBeDestroyed.clear();
	spatial_hash::gGird.clearAllTiles();

	for (auto const& entity : mEntities) {
		auto& rigidBody = G::gCoordinator.GetComponent<RigidBody>(entity);
		spatial_hash::gGird.insert(entity, rigidBody.hitbox.hitboxRect);
	}
}