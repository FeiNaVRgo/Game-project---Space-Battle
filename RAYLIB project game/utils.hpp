#pragma once
#include <src\raylib-cpp.hpp>
#include <string>
#include <numeric>
#include "GeometryCalc.hpp"
#include "globals.h"

namespace raylib {
	template<typename T>
	static inline std::string KeyInputToChar(T input) {
		std::string s;

		if (!(input >= 0 && input <=6)) {
			 s.append(1, char(input));
			return s;
		}
		     if (input == 0) { return s = "L_MOUSE"; }
		else if (input == 1) { return s = "R_MOUSE"; }
		else if (input == 2) { return s = "M_MOUSE"; }
		else if (input == 3) { return s = "SIDE_MOUSE"; }
		else if (input == 4) { return s = "EXTRA_MOUSE"; }
		else if (input == 5) { return s = "FORWARD_MOUSE"; }
		else if (input == 6) { return s = "BACK_MOUSE"; }

		return s = "ERROR";
	}

	extern bool IsKeyOrMousePressed(int key);
	extern bool IsKeyOrMouseDown(int key);
	extern bool IsKeyOrMouseReleased(int key);
	extern bool IsKeyOrMouseUp(int key);
	extern void PanAndZoom();
	extern void AABBcollisionResponse(raylib::Vector2& pos1, raylib::Vector2& pos2, raylib::Rectangle& b1, raylib::Rectangle& b2);
	extern raylib::Vector2 CenterText(raylib::Text text);
	extern float crossProductVector2(raylib::Vector2 v1, raylib::Vector2 v2);
    /**
    * @brief calculates point of intersection between two rays
    * @brief returns empty vector when there is no intersection
    * @brief only for 2d!
    */
	extern inline std::vector<raylib::Vector3> intersectsRayToRay(const raylib::Ray& q1, const raylib::Ray& q2);
}

