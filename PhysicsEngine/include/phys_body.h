#ifndef PHYS_BODY_H
#define PHYS_BODY_H

#include <phys_platform.h>
#include <phys_utils.h>

#include <memory>

namespace physic
{
	class IBody;
	using BodyPtr = std::shared_ptr<IBody>;

	class PHYS_API IShape
	{
	public:

		enum class ShapeType
		{
			Circle,
			Rectangle
		};

		IShape() = default;
		virtual ~IShape() = default;

		virtual ShapeType GetShapeType() const = 0;
		virtual int GetRadius() const = 0;
	};

	class PHYS_API IBody
	{
	public:

		virtual Point GetPosition() const = 0;
		virtual void SetPosition(const Point&) = 0;

		virtual fVec2D GetVelocityVector() const = 0;
		virtual void SetVelocityVector(const fVec2D&) = 0;

		virtual Mass GetMass() const = 0;
		virtual void SetMass(Mass) = 0;

		virtual float GetBounceFactor() const = 0;
		virtual void SetBounceFactor(float) = 0;

		static BodyPtr CreateBody(IShape::ShapeType shape, const Point& position, const fVec2D& velocity, float mass);

		IBody() = default;
		virtual ~IBody() = default;
	};
}

#endif // PHYS_BODY_H