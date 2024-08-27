#pragma once
#include <src/raylib-cpp.hpp>
#include <functional>
#include <Eigen/Dense>

namespace raylib {
	using RealFunc            = std::function<double(double, std::vector<float>)>;
	using RealFuncDerivative  = std::function<double(RealFunc, double, std::vector<float>)>;

	extern constexpr float getRightRect(const raylib::Rectangle& r1) noexcept;
	extern constexpr float getBottomRect(const raylib::Rectangle& r1) noexcept;
	extern Vector2 getTopLeftRect(const raylib::Rectangle& r1) noexcept;
	extern Vector2 getBottomRightRect(const raylib::Rectangle& r1) noexcept;
	extern Vector2 getCenterRect(const raylib::Rectangle& r1) noexcept;
	extern Vector2 getCenterRect(const raylib::Vector2& pos, const raylib::Vector2& dim) noexcept;
	extern Vector2 getSizeRect(const raylib::Rectangle& r1) noexcept;
	extern constexpr bool containsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2);
	extern constexpr bool intersectsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2);
	extern constexpr bool overlapsRect(const raylib::Rectangle& r1, const raylib::Rectangle& r2);
	extern bool checkCollisionRotatedRectangles(const raylib::Rectangle r1, const raylib::Rectangle r2);
	extern double derivative(RealFunc f, double x, std::vector<float> params);
	extern double NewtonsMethod(RealFunc f, RealFuncDerivative d, double x0, double precision, std::vector<float> params);//dont use
}