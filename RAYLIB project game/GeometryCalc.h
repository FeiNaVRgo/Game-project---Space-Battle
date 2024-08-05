#pragma once
#include <src\raylib-cpp.hpp>
#include <functional>
#include <Eigen/Dense>

namespace raylib {
	using RealFunc            = std::function<double(double, std::vector<float>)>;
	using RealFuncDerivative  = std::function<double(RealFunc, double, std::vector<float>)>;

	constexpr float getRightRect(const raylib::Rectangle& r1) noexcept {
		return r1.x + r1.width;
	}

	constexpr float getBottomRect(const raylib::Rectangle& r1) noexcept {
		return r1.y + r1.height;
	}

	inline Vector2 getTopLeftRect(const raylib::Rectangle& r1) noexcept {
		return Vector2(r1.x, r1.y);
	}

	inline Vector2 getBottomRightRect(const raylib::Rectangle& r1) noexcept {
		return Vector2(r1.x, r1.y) + Vector2(r1.width, r1.height);
	}

	inline Vector2 getCenterRect(const raylib::Rectangle& r1) noexcept {
		return Vector2(r1.x, r1.y) + Vector2(r1.width / 2.0f, r1.height / 2.0f);
	}

	inline Vector2 getSizeRect(const raylib::Rectangle& r1) noexcept {
		return Vector2(r1.width, r1.height);
	}

	static constexpr bool containsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
		return (r2.x >= r1.x) && (r2.x + r2.width <= r1.x + r1.width) &&
			(r2.y >= r1.y) && (r2.y + r2.height <= r1.y + r1.height);
	}

	static constexpr bool intersectsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
		return !(r1.x >= getRightRect(r2) || getRightRect(r1) <= r2.x || r1.y >= getBottomRect(r2) || getBottomRect(r1) <= r2.y);
	}

	static constexpr bool overlapsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2) {
		return (r1.x <= r2.x + r2.width && r1.x + r1.width >= r2.x &&
			r1.y <= r2.y + r2.height && r1.y + r1.height >= r2.y);
	}

	static inline bool checkCollisionRotatedRectangles(const raylib::Rectangle r1, const raylib::Rectangle r2) {
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

	static inline double derivative(RealFunc f, double x, std::vector<float> params) {
		//double h = sqrt(std::numeric_limits<double>::epsilon());
		double h = 0.01;
		return (f(x + h, params) - f(x - h, params)) / (2.0 * h);
	}

	/**
	* @brief so slow that program freezes
	*/
	static inline double NewtonsMethod(RealFunc f, RealFuncDerivative d, double x0, double precision, std::vector<float> params) {
		double x = x0;
		for (size_t i = 0;; i++) {
			x = x - (f(x, params) / d(f, x, params));
			if (abs(f(x, params)) < precision) {
				return x;
			}
		}
	}
}