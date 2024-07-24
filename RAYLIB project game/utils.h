#pragma once
#include <src\raylib-cpp.hpp>
#include <string>
#include <numeric>

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
        if (IsKeyPressed(KEY_ONE)) GLOBALS::zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) GLOBALS::zoomMode = 1;

        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / GLOBALS::camera.zoom);
            GLOBALS::camera.target = Vector2Add(GLOBALS::camera.target, delta);
        }

        if (GLOBALS::zoomMode == 0)
        {
            // Zoom based on mouse wheel
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), GLOBALS::camera);

                // Set the offset to where the mouse is
                GLOBALS::camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                GLOBALS::camera.target = mouseWorldPos;

                // Zoom increment
                float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
                if (wheel < 0) scaleFactor = 1.0f / scaleFactor;
                GLOBALS::camera.zoom = Clamp(GLOBALS::camera.zoom * scaleFactor, 0.125f, 64.0f);
            }
        }
        else
        {
            // Zoom based on mouse left click
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), GLOBALS::camera);

                // Set the offset to where the mouse is
                GLOBALS::camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                GLOBALS::camera.target = mouseWorldPos;
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                // Zoom increment
                float deltaX = GetMouseDelta().x;
                float scaleFactor = 1.0f + (0.01f * fabsf(deltaX));
                if (deltaX < 0) scaleFactor = 1.0f / scaleFactor;
                GLOBALS::camera.zoom = Clamp(GLOBALS::camera.zoom * scaleFactor, 0.125f, 64.0f);
            }
        }
    }
    //doesn work oh my god
    static inline void AABBcollisionResponse(raylib::Vector2& vel1, raylib::Vector2& vel2, const raylib::Rectangle& b1, const raylib::Rectangle& b2) {
        raylib::Vector2 normal;
        
        float xInvEntry, yInvEntry;
        float xInvExit, yInvExit;

        // find the distance between the objects on the near and far sides for both x and y 
        if (vel1.x > 0.0f)
        {
            xInvEntry = b2.x - (b1.x + b1.width);
            xInvExit = (b2.x + b2.width) - b1.x;
        }
        else
        {
            xInvEntry = (b2.x + b2.width) - b1.x;
            xInvExit = b2.x - (b1.x + b1.width);
        }

        if (vel1.y > 0.0f)
        {
            yInvEntry = b2.y - (b1.y + b1.height);
            yInvExit = (b2.y + b2.height) - b1.y;
        }
        else
        {
            yInvEntry = (b2.y + b2.height) - b1.y;
            yInvExit = b2.y - (b1.y + b1.height);
        }
        
        float xEntry, yEntry;
        float xExit, yExit;

        if (vel1.x == 0.0f)
        {
            xEntry = -std::numeric_limits<float>::infinity();
            xExit = std::numeric_limits<float>::infinity();
        }
        else
        {
            xEntry = xInvEntry / vel1.x;
            xExit = xInvExit / vel1.y;
        }

        if (vel1.y == 0.0f)
        {
            yEntry = -std::numeric_limits<float>::infinity();
            yExit = std::numeric_limits<float>::infinity();
        }
        else
        {
            yEntry = yInvEntry / vel1.x;
            yExit = yInvExit / vel1.y;
        }

        if (xEntry > yEntry)
        {
            if (xInvEntry < 0.0f)
            {
                normal.x = 1.0f;
                normal.y = 0.0f;
            }
            else
            {
                normal.x = -1.0f;
                normal.y = 0.0f;
            }
        }
        else
        {
            if (yInvEntry < 0.0f)
            {
                normal.x = 0.0f;
                normal.y = 1.0f;
            }
            else
            {
                normal.x = 0.0f;
                normal.y = -1.0f;
            }
        }

        raylib::Vector2 vj = -Vector2DotProduct(vel1, normal);
        vel1.x += vj.x * normal.x;
        vel1.y += -vj.y * normal.y;
    }
}
