#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <phys_platform.h>
#include <phys_body.h>

namespace physic
{
	class PHYS_API IEngine
	{
	public:
		virtual void SetWorldMargins(Point bottom_left, Point top_right) = 0;
		virtual void SetWorldConstants(float gravity, float air_drag, float ground_friction) = 0;
		virtual void AddBody(BodyPtr&) = 0;
		virtual void RemoveBody(const BodyPtr&) = 0;
		virtual void Step(double dt) = 0;

		static IEngine* Instance();
	protected:
		IEngine() = default;
		virtual ~IEngine() = default;
	};
} // namespace physic

#endif // PHYS_ENGINE_H