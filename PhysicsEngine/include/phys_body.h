#ifndef PHYS_BODY_H
#define PHYS_BODY_H

#include <phys_platform.h>
#include <phys_utils.h>

#include <memory>

namespace physic
{
	class IBody;
	using BodyPtr = std::shared_ptr<IBody>;

	class PHYS_API IBody
	{
	public:

		virtual fVec2D GetPosition() const = 0;
		virtual void SetPosition(const fVec2D&) = 0;

		virtual float GetMass() const = 0;
		virtual void SetMass(float) = 0;

		virtual fVec2D GetVelocityVector() const = 0;
		virtual void SetVelocityVector(const fVec2D&) = 0;

		virtual float GetBounceFactor() const = 0;
		virtual void SetBounceFactor(float) = 0;

		static BodyPtr GetBody();

		IBody() = default;
		virtual ~IBody() = default;
	};
}

#endif // PHYS_BODY_H