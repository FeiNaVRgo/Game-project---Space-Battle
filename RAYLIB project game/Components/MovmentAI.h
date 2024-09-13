#pragma once
#include <vector>
#include <src/raylib-cpp.hpp>
#include "../ECS.h"
#include "../GeometryCalc.hpp"


class MovmentAI {
public:
	bool DistanceH;
	bool DistanceW;


	void chase(const raylib::Vector2& pos, const raylib::Vector2& posToChase, raylib::Vector2& velocity, float& angle, float speed, float seeRadius = std::numeric_limits<float>::infinity());

	/**
	* @brief a * sin (x * PI / b)
	* @param params -  0 for b, 1 for a
	*/
	static double sinwaveExactFunc(double x, std::vector<float> params);

	/**
	* @brief y = a * sin (xb)
	* @param params -  0 for b, 1 for a
	*/
	static double sinwaveFunc(double x, std::vector<float> params);

	/**
	* @brief y = ax^2 + bx
	* @param params - 0 for a, 1 for b
	*/
	static double parabolicFunc(double x, std::vector<float> params);

	/**
	* @brief y = ax^2 + bx
	* @param params -  0 for T, 1 for H
	*/
	static double parabolicExactFunc(double x, std::vector<float> params);

	/**
	* @brief y = ax
	* @brief if using angle of sprite multiply angle by DEG2RAD macro
	* @param 0 for a
	*/
	static double straightLineFunc(double x, std::vector<float> params);

	/**
	* @brief makes bullet follow path of given function
	*/
	void moveInLineOfFunc(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f);

	/**
	* @brief makes bullet follow certain path
	* @brief first argument of params must by maxX
	* @brief still in development
	* TODO
	*/
	void moveInLineOfFuncAndGoBack(float speed, ECS::Entity anchor, std::vector<float> params, raylib::RealFunc f, raylib::Vector2 whereToDestroy);
};
