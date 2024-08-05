#pragma once
#include <src\raylib-cpp.hpp>
#include <string>
#include <numeric>
#include "GeometryCalc.h"

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

	static inline bool IsKeyOrMousePressed(int key) {
		if (key >= 0 && key <= 6) {
			return IsMouseButtonPressed(key);
		}
		return IsKeyPressed(key);
	}

	static inline bool IsKeyOrMouseDown(int key) {
		if (key >= 0 && key <= 6) {
			return IsMouseButtonDown(key);
		}
		return IsKeyDown(key);
	}

	static inline bool IsKeyOrMouseReleased(int key) {
		if (key >= 0 && key <= 6) {
			return IsMouseButtonReleased(key);
		}
		return IsKeyReleased(key);
	}

	static inline bool IsKeyOrMouseUp(int key) {
		if (key >= 0 && key <= 6) {
			return IsMouseButtonUp(key);
		}
		return IsKeyUp(key);
	}

    static inline void PanAndZoom() {
        if (IsKeyPressed(KEY_ONE)) G::zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) G::zoomMode = 1;

        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / G::camera.zoom);
            G::camera.target = Vector2Add(G::camera.target, delta);
        }

        if (G::zoomMode == 0)
        {
            // Zoom based on mouse wheel
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), G::camera);

                // Set the offset to where the mouse is
                G::camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                G::camera.target = mouseWorldPos;

                // Zoom increment
                float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
                if (wheel < 0) scaleFactor = 1.0f / scaleFactor;
                G::camera.zoom = Clamp(G::camera.zoom * scaleFactor, 0.125f, 64.0f);
            }
        }
        else
        {
            // Zoom based on mouse left click
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), G::camera);

                // Set the offset to where the mouse is
                G::camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                G::camera.target = mouseWorldPos;
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                // Zoom increment
                float deltaX = GetMouseDelta().x;
                float scaleFactor = 1.0f + (0.01f * fabsf(deltaX));
                if (deltaX < 0) scaleFactor = 1.0f / scaleFactor;
                G::camera.zoom = Clamp(G::camera.zoom * scaleFactor, 0.125f, 64.0f);
            }
        }
    }

    static inline void AABBcollisionResponse(raylib::Vector2& pos1, raylib::Vector2& pos2, raylib::Rectangle& b1, raylib::Rectangle& b2) {
        auto posr = getCenterRect(b1) - getCenterRect(b2);
        raylib::Rectangle intersection = b1.GetCollision(b2);
        auto size = intersection.GetSize();

        if (size.x < size.y) {
            if (posr.x > 0) {
                pos1.x += size.x;
                b1.x += size.x;
            }
            else {
                pos1.x -= size.x;
                b1.x -= size.x;
            }
        }
        else if(size.x > size.y) {
            if (posr.y > 0) {
                pos1.y += size.y;
                b1.y += size.y;
            }
            else {
                pos1.y -= size.y;
                b1.y -= size.y;
            }
        }
    }


    static inline raylib::Vector2 CenterText(raylib::Text text) {
        auto dimensions = text.MeasureEx();

        return dimensions * 0.5f;
    }

    static inline float crossProductVector2(raylib::Vector2 v1, raylib::Vector2 v2) {
        return v1.x * v2.y - v1.y * v2.x;
    }

    /**
    * @brief calculates point of intersection between two rays
    * @brief returns empty vector when there is no intersection
    * @brief only for 2d!
    */
    static inline std::vector<raylib::Vector3> intersectsRayToRay(const raylib::Ray& q1, const raylib::Ray& q2) {
        const auto origin_diff = raylib::Vector3(q2.position) - raylib::Vector3(q1.position);

        const auto cp1 = raylib::crossProductVector2(raylib::Vector2(q1.direction.x, q1.direction.y), raylib::Vector2(q2.direction.x, q2.direction.y));

        const auto cp2 = raylib::crossProductVector2(raylib::Vector2(origin_diff.x, origin_diff.y), raylib::Vector2(q2.direction.x, q2.direction.y));

        if (cp1 == 0) // Early rejection
        {
            if (cp2 == 0)
                return { raylib::Vector3(q1.position) }; // co-linear
            else
                return {}; // parallel
        }

        const auto cp3 = raylib::crossProductVector2(raylib::Vector2(origin_diff.x, origin_diff.y), raylib::Vector2(q1.direction.x, q1.direction.y));
        const auto t1 = cp2 / cp1; // distance along q1 to intersection
        const auto t2 = cp3 / cp1; // distance along q2 to intersection

        if (t1 >= 0 && t2 >= 0)
            return { raylib::Vector3(q1.position) + raylib::Vector3(q1.direction) * t1 }; // Intersection, both rays positive
        else
            return {}; // Intersection, but behind a rays origin, so not really an intersection in context
    }
}

