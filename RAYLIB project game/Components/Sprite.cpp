#include "Sprite.h"

#include "../globals.h"
#include "./Transforms.h"

void Sprite::draw(ECS::Entity entity) {
	//auto& sprite = G::gCoordinator.GetComponent<Sprite>(entity);
	auto& transform = G::gCoordinator.GetComponent<Transforms>(entity);

	sprite.Draw(
		raylib::Rectangle(0.0f, 0.0f, sprite.width, sprite.height),
		raylib::Rectangle(transform.position.x, transform.position.y, sprite.width, sprite.height),
		origin,
		angle * RAD2DEG,
		tint
	);
}

Sprite::~Sprite() {}