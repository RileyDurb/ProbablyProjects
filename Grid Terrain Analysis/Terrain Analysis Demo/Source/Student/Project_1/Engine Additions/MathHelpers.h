//
// MathHelpers.h - Helper functions for math
//
#pragma once
#include <pch.h>


class MathHelpers
{
public:
	static float GetFacingYaw(Vec3 myPos, Vec3 theirPos)
	{
		Vec3 directionToThem = theirPos - myPos;
		directionToThem.Normalize();
		return std::atan2(directionToThem.x, directionToThem.z);
	}
};

