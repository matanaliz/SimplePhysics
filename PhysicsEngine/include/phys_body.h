#ifndef PHYS_BODY_H
#define PHYS_BODY_H

#include <phys_platform.h>
#include <phys_utils.h>

#include <memory>

namespace physic
{
	class IBody;
	using BodyPtr = std::shared_ptr<IBody>;

	class IShape;
	using ShapePtr = std::shared_ptr<IShape>;

	class PHYS_API IShape
	{
	public:

		enum class ShapeType
		{
			Circle,
			Rectangle,
			Polygon
		};

		IShape() = default;
		virtual ~IShape() = default;

		virtual ShapeType GetShapeType() const = 0;
		virtual Point GetCenter() const = 0;
		virtual int GetRadius() const = 0;
		virtual fVec2D GetNormalVector() const = 0;
		virtual bool Collide(IShape*) = 0;
	};

	class PHYS_API IBody
	{
	public:

		virtual Point GetPosition() const = 0;
		virtual void SetPosition(const Point&) = 0;

		virtual fVec2D GetVelocityVector() const = 0;
		virtual void SetVelocityVector(const fVec2D&) = 0;

		virtual Mass GetMass() const = 0;
		virtual void SetMass(const Mass&) = 0;

		virtual float GetBounceFactor() const = 0;
		virtual void SetBounceFactor(float) = 0;

		virtual void ApplyForce(const fVec2D&) = 0;
		virtual void ApplyImpulse(const fVec2D&) = 0;

		// TODO change time type from float to dedicated
		virtual void Update(float dt) = 0;

		// TODO move this to entity
		virtual ShapePtr GetShape() const = 0;

		static BodyPtr CreateBody(IShape::ShapeType shape, const Point& position, const fVec2D& velocity, float mass);

		IBody() = default;
		virtual ~IBody() = default;
	};
}

#endif // PHYS_BODY_H