#include <phys_body.h>
#include <phys_constants.h>

using namespace physic;

class ShapeCircle : public IShape
{
public:
	ShapeCircle(ShapeType) {};
	ShapeType GetShapeType() const override { return ShapeType::Circle; };
	int GetRadius() const override { return 10; };
};

// TODO move BodyImpl to private include header
class BodyImpl : public IBody
{
public:
	BodyImpl();
	BodyImpl(IShape::ShapeType, Point, fVec2D, float);
	~BodyImpl() = default;
	BodyImpl(const BodyImpl&) = delete;
	BodyImpl& operator=(const BodyImpl&) = delete;

	BodyImpl(BodyImpl&&) = delete;
	BodyImpl& operator=(BodyImpl&&) = delete;

	virtual Point GetPosition() const override;
	virtual void SetPosition(const Point&) override;

	virtual Mass GetMass() const override;
	virtual void SetMass(Mass) override;

	virtual fVec2D GetVelocityVector() const override;
	virtual void SetVelocityVector(const fVec2D&) override;

	virtual float GetBounceFactor() const override;
	virtual void SetBounceFactor(float) override;

private:
	// TODO get usage of mass and calculate impulses
	Point m_position;
	fVec2D m_velocity;
	Mass m_mass;

	std::unique_ptr<IShape> m_shape;

	// Leave "bounciness" to some "material"
	float m_bounceFactor;
};

BodyImpl::BodyImpl()
	: m_position(0.f, 0.f)
	, m_velocity(0.f, 0.f)
	, m_mass(1.f)
	, m_shape(new (std::nothrow) ShapeCircle(IShape::ShapeType::Circle))
	, m_bounceFactor(kBounceFactor)
{

}

BodyImpl::BodyImpl(IShape::ShapeType shape, Point pos, fVec2D vel, float mass)
	: m_position(pos)
	, m_velocity(vel)
	, m_mass(mass)
	, m_shape(new (std::nothrow) ShapeCircle(shape))
	, m_bounceFactor(kBounceFactor)
{}

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

BodyPtr IBody::CreateBody(IShape::ShapeType shape, const Point& position, const fVec2D& velocity, float mass)
{
	auto ptr = std::shared_ptr<IBody>(new (std::nothrow) BodyImpl(shape, position, velocity, mass));
	assert(nullptr != ptr);

	return ptr;
}