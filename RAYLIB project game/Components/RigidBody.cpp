#include "RigidBody.h"

raylib::Vector2 Hitbox::getHitBoxCenter() {
	return hitboxRect.GetPosition() + hitboxRect.GetSize() * 0.5f;
}