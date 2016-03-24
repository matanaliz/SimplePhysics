#include <phys_body.h>
#include <phys_constants.h>

using namespace physic;

class ShapeCircle : public IShape
{
public:
	explicit ShapeCircle(ShapeType) {};
	virtual ~ShapeCircle() = default;

	ShapeCircle(const ShapeCircle&) = delete;
	ShapeCircle& operator=(const ShapeCircle&) = delete;

	ShapeCircle(ShapeCircle&&);
	ShapeCircle& operator=(ShapeCircle&&) = delete;

	virtual ShapeType GetShapeType() const override;
	virtual int GetRadius() const override;

private:
	IShape::ShapeType m_shape;
	int m_radius;
};

ShapeCircle::ShapeCircle(ShapeCircle&& other)
	: m_shape(std::move(other.m_shape))
	, m_radius(std::move(other.m_radius))
{

}

IShape::ShapeType ShapeCircle::GetShapeType() const
{
	return m_shape;
}

int ShapeCircle::GetRadius() const
{
	return m_radius;
}

// TODO move BodyImpl to private include header
class BodyImpl : public IBody
{
public:
	BodyImpl();
	BodyImpl(IShape::ShapeType, Point, fVec2D, float);
	~BodyImpl() = default;
	BodyImpl(const BodyImpl&) = delete;
	BodyImpl& operator=(const BodyImpl&) = delete;

	BodyImpl(BodyImpl&&);
	BodyImpl& operator=(BodyImpl&&) = delete;

	virtual Point GetPosition() const override;
	virtual void SetPosition(const Point&) override;

	virtual Mass GetMass() const override;
	virtual void SetMass(Mass) override;

	virtual fVec2D GetVelocityVector() const override;
	virtual void SetVelocityVector(const fVec2D&) override;

	virtual float GetBounceFactor() const override;
	virtual void SetBounceFactor(float) override;

	virtual ShapePtr GetShape() const override;

private:
	// TODO get usage of mass and calculate impulses
	Point m_position;
	fVec2D m_velocity;
	Mass m_mass;

	std::shared_ptr<IShape> m_shape;

	// Leave "bounciness" to some "material"
	float m_bounceFactor;
};

BodyImpl::BodyImpl()
	: m_position(0.f, 0.f)
	, m_velocity(0.f, 0.f)
	, m_mass(1.f)
	, m_shape(std::make_shared<ShapeCircle>(ShapeCircle(IShape::ShapeType::Circle)))
	, m_bounceFactor(kBounceFactor)
{

}

BodyImpl::BodyImpl(IShape::ShapeType shape, Point pos, fVec2D vel, float mass)
	: m_position(pos)
	, m_velocity(vel)
	, m_mass(mass)
	, m_shape(std::make_shared<ShapeCircle>(ShapeCircle(shape)))
	, m_bounceFactor(kBounceFactor)
{}

BodyImpl::BodyImpl(BodyImpl&& other)
	: m_position(std::move(other.m_position))
	, m_velocity(std::move(other.m_velocity))
	, m_mass(std::move(other.m_mass))
	, m_shape(std::move(other.m_shape))
	, m_bounceFactor(std::move(other.m_bounceFactor))
{

}

Point BodyImpl::GetPosition() const
{
	return m_position;
}

void BodyImpl::SetPosition(const Point& val)
{
	m_position = val;
}

Mass BodyImpl::GetMass() const
{
	return m_mass;
}

void BodyImpl::SetMass(Mass mass)
{
	m_mass = mass;
}

fVec2D BodyImpl::GetVelocityVector() const
{
	return m_velocity;
}

void BodyImpl::SetVelocityVector(const fVec2D& val)
{
	m_velocity = val;
}

float BodyImpl::GetBounceFactor() const
{
	return m_bounceFactor;
}

void BodyImpl::SetBounceFactor(float bounceFactor)
{
	m_bounceFactor = bounceFactor;
}

ShapePtr BodyImpl::GetShape() const
{
	assert(nullptr != m_shape);
	return m_shape;
}

BodyPtr IBody::CreateBody(IShape::ShapeType shape, const Point& position, const fVec2D& velocity, float mass)
{
	return std::shared_ptr<IBody>(std::make_shared<BodyImpl>(BodyImpl(shape, position, velocity, mass)));
}