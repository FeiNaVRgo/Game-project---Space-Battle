#include "Health.h"

void Health::drawHealthBar(const raylib::Vector2& origin, const raylib::Vector2& offsetFromOrigin) {
	raylib::Rectangle r1 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(maxHealth, 4) };
	raylib::Rectangle r2 = { origin + offsetFromOrigin - raylib::Vector2(maxHealth / 2.0f, 0), raylib::Vector2(health, 4) - raylib::Vector2(1, 1) };
	r2.Draw(BLUE);
	r1.DrawLines(BLACK);
}

void Health::drawDamageIndicator() {}

void Health::clear() {
	this->health = this->maxHealth;
	this->healthToSubstract = 0;
	this->isDamaged = false;
	this->toBeDamaged = false;
}