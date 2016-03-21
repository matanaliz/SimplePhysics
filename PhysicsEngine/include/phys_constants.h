#ifndef PHYS_CONSTANTS_H
#define PHYS_CONSTANTS_H

#include <phys_utils.h>

namespace physic
{
	const float kGravity = 9.81f;
	const float kBounceFactor = 0.75f;
	const float kAirDragFactor = 0.0f;
	const float kGroundFriction = 0.0f;

	const double kPi = 3.141592;

	const Point kWorldBotLeft = { 0, 0 };
	const Point kWorldTopRight = { 2048, 2048 };
}

#endif // PHYS_CONSTANTS_H