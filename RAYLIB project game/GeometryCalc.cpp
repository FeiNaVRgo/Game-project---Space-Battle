#include "GeometryCalc.hpp"

inline constexpr float raylib::getRightRect(const raylib::Rectangle& r1) noexcept {
	return r1.x + r1.width;
}

inline constexpr float raylib::getBottomRect(const raylib::Rectangle& r1) noexcept {
	return r1.y + r1.height;
}

inline raylib::Vector2 raylib::getTopLeftRect(const raylib::Rectangle& r1) noexcept {
	return Vector2(r1.x, r1.y);
}

inline raylib::Vector2 raylib::getBottomRightRect(const raylib::Rectangle& r1) noexcept {
	return Vector2(r1.x, r1.y) + Vector2(r1.width, r1.height);
}

inline raylib::Vector2 raylib::getCenterRect(const raylib::Rectangle& r1) noexcept {
	return r1.GetPosition() + r1.GetSize() / 2.0f;
}

inline raylib::Vector2 raylib::getCenterRect(const raylib::Vector2& pos, const raylib::Vector2& dim) noexcept {
	return pos + dim / 2.0f;
}

inline raylib::Vector2 raylib::getSizeRect(const raylib::Rectangle& r1) noexcept {
	return Vector2(r1.width, r1.height);
}

inline constexpr bool raylib::containsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
	return (r2.x >= r1.x) && (r2.x + r2.width <= r1.x + r1.width) &&
		(r2.y >= r1.y) && (r2.y + r2.height <= r1.y + r1.height);
}

inline constexpr bool raylib::intersectsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
	return !(r1.x >= getRightRect(r2) || getRightRect(r1) <= r2.x || r1.y >= getBottomRect(r2) || getBottomRect(r1) <= r2.y);
}

inline constexpr bool raylib::overlapsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
	return (r1.x <= r2.x + r2.width && r1.x + r1.width >= r2.x &&
		r1.y <= r2.y + r2.height && r1.y + r1.height >= r2.y);
}

inline bool raylib::checkCollisionRotatedRectangles(const raylib::Rectangle r1, const raylib::Rectangle r2) {
	auto vecs1 = new raylib::Vector2[4];
	vecs1[0] = r1.GetPosition();
	vecs1[1] = raylib::Vector2(r1.GetX() + r1.GetWidth(), r1.GetY());
	vecs1[2] = raylib::Vector2(r1.GetX(), r1.GetY() + r1.GetHeight());
	vecs1[3] = r1.GetPosition() + r1.GetSize();

	auto vecs2 = new raylib::Vector2[4];
	vecs2[0] = r2.GetPosition();
	vecs2[1] = raylib::Vector2(r2.GetX() + r2.GetWidth(), r2.GetY());
	vecs2[2] = raylib::Vector2(r2.GetX(), r2.GetY() + r2.GetHeight());
	vecs2[3] = r2.GetPosition() + r2.GetSize();

	auto isCollision = new bool[4];
	for (int i = 0; i < 4; i++) {
		isCollision[i] = CheckCollisionPointPoly(vecs1[i], vecs2, 4);
	}

	delete[] vecs1;
	delete[] vecs2;

	for (int i = 0; i < 4; i++) {
		if (isCollision[i] == true) {
			delete[] isCollision;
			return true;
		}
	}
	delete[] isCollision;
	return false;
}

inline double raylib::derivative(RealFunc f, double x, std::vector<float> params) {
	//double h = sqrt(std::numeric_limits<double>::epsilon());
	double h = 0.01;
	return (f(x + h, params) - f(x - h, params)) / (2.0 * h);
}

inline double raylib::NewtonsMethod(RealFunc f, RealFuncDerivative d, double x0, double precision, std::vector<float> params) {
	double x = x0;
	for (size_t i = 0;; i++) {
		x = x - (f(x, params) / d(f, x, params));
		if (abs(f(x, params)) < precision) {
			return x;
		}
	}
}