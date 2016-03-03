#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <phys_platform.h>
#include <phys_body.h>

namespace physic
{
	class PHYS_API IEngine
	{
	public:
		virtual void SetWorldConstrains(float gravity, int bot, int top, int xmax, int ymax) = 0;
		virtual void AddBody(const BodyPtr&) = 0;
		virtual void RemoveBody(const BodyPtr&) = 0;
		virtual void Step(double dt) = 0;

		static IEngine* Instance();
	protected:
		IEngine();
		virtual ~IEngine();
	};
} // namespace physic

#endif // PHYS_ENGINE_H