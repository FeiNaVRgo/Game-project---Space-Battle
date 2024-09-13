#include "MovmentAI.h"

#include "JustBorn.h"
#include "Transforms.h"
#include "RigidBody.h"
#include "Sprite.h"

#include "../utils.hpp"
#include "../globals.h"

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

double MovmentAI::sinwaveExactFunc(double x, std::vector<float> params) {
	return params.at(1) * sinf(static_cast<float>(x * (PI / params.at(0))));
}

double MovmentAI::sinwaveFunc(double x, std::vector<float> params) {
	return params.at(1) * sinf(static_cast<float>(x) * params.at(0));
}

double MovmentAI::parabolicFunc(double x, std::vector<float> params) {
	return params.at(0) * x * x + params.at(1) * x;
}

double MovmentAI::parabolicExactFunc(double x, std::vector<float> params) {
	float T = params.at(0);
	float H = params.at(1);

	double c = T / 2.0f;
	double a = H / (c * c);
	double b = H;

	return -(a * (x - c) * (x - c)) + b;
}

double MovmentAI::straightLineFunc(double x, std::vector<float> params) {
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