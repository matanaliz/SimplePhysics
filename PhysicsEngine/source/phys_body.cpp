#include <phys_body.h>
#include <phys_constants.h>

#include <numeric>

using namespace physic;

class ShapeBox : public IShape
{
public:
	explicit ShapeBox() : m_shape(ShapeType::Rectangle), m_radius(0) {};
	virtual ~ShapeBox() = default;

	ShapeBox(const ShapeBox&) = delete;
	ShapeBox& operator=(const ShapeBox&) = delete;

	ShapeBox(ShapeBox&&);
	ShapeBox& operator=(ShapeBox&&) = delete;

	virtual ShapeType GetShapeType() const override;
	virtual Point GetCenter() const override;
	virtual int GetRadius() const override;
	virtual fVec2D GetNormalVector() const override;

	virtual bool Collide(IShape* other) override;
	
	void collideCircle(IShape* other);
	void collideRectangle(IShape* other);
	void collidePolygon(IShape* other);

private:
	IShape::ShapeType m_shape;
	int m_radius;
	fVec2D m_normal;
	Point m_center;

	int m_width;
	int m_height;
};

IShape::ShapeType ShapeBox::GetShapeType() const
{
	return m_shape;
}

Point ShapeBox::GetCenter() const
{
	return m_center;
}

fVec2D ShapeBox::GetNormalVector() const
{
	return m_normal;
}

bool ShapeBox::Collide(IShape* other)
{
	assert(other != nullptr);

	const IShape::ShapeType shape = other->GetShapeType();
	switch (shape)
	{
	case IShape::ShapeType::Circle:
		collideCircle(other);
		break;
	case IShape::ShapeType::Rectangle:
		collideRectangle(other);
		break;
	case IShape::ShapeType::Polygon:
		collidePolygon(other);
		break;
	default:
		break;
	}

	return false;
}

void ShapeBox::collideCircle(IShape* other)
{

}

void ShapeBox::collideRectangle(IShape* other)
{

}

void ShapeBox::collidePolygon(IShape* other)
{

}

class ShapeCircle : public IShape
{
public:
	explicit ShapeCircle() : m_shape(ShapeType::Circle), m_radius(0) {};
	virtual ~ShapeCircle() = default;

	ShapeCircle(const ShapeCircle&) = delete;
	ShapeCircle& operator=(const ShapeCircle&) = delete;

	ShapeCircle(ShapeCircle&&);
	ShapeCircle& operator=(ShapeCircle&&) = delete;

	virtual ShapeType GetShapeType() const override;
	virtual Point GetCenter() const override;
	virtual int GetRadius() const override;
	virtual fVec2D GetNormalVector() const override;

	virtual bool Collide(IShape* other) override;

private:
	IShape::ShapeType m_shape;
	int m_radius;
	fVec2D m_normal;
	Point m_center;
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

Point ShapeCircle::GetCenter() const
{
	return m_center;
}

int ShapeCircle::GetRadius() const
{
	return m_radius;
}

fVec2D ShapeCircle::GetNormalVector() const
{
	return { 0, 0 };
}

bool ShapeCircle::Collide(IShape* other)
{
	return false;
}

// TODO move BodyImpl to private include header
class BodyImpl : public IBody
{
public:
	BodyImpl() = delete;
	BodyImpl(IShape::ShapeType, Point, fVec2D, float);
	~BodyImpl() = default;
	BodyImpl(const BodyImpl&) = delete;
	BodyImpl& operator=(const BodyImpl&) = delete;

	BodyImpl(BodyImpl&&);
	BodyImpl& operator=(BodyImpl&&) = delete;

	virtual Point GetPosition() const override;
	virtual void SetPosition(const Point&) override;

	virtual Mass GetMass() const override;
	virtual void SetMass(const Mass&) override;

	virtual fVec2D GetVelocityVector() const override;
	virtual void SetVelocityVector(const fVec2D&) override;

	virtual float GetBounceFactor() const override;
	virtual void SetBounceFactor(float) override;

	virtual void ApplyForce(const fVec2D&) override;
	virtual void ApplyImpulse(const fVec2D&) override;

	virtual void Update(float dt) override;

	virtual ShapePtr GetShape() const override;

private:
	// TODO get usage of mass and calculate impulses
	Point m_position;
	fVec2D m_velocity;
	Mass m_mass;

	std::vector<fVec2D> m_forces;
	std::vector<fVec2D> m_impulses;

	std::shared_ptr<IShape> m_shape;

	// Leave "bounciness" to some "material"
	float m_bounceFactor;
};

BodyImpl::BodyImpl(IShape::ShapeType shape, Point pos, fVec2D vel, float mass)
	: m_position(pos)
	, m_velocity(vel)
	, m_mass(mass)
	, m_forces()
	, m_impulses()
	, m_shape(std::make_shared<ShapeCircle>(ShapeCircle()))
	, m_bounceFactor(kBounceFactor)
{}

BodyImpl::BodyImpl(BodyImpl&& other)
	: m_position(std::move(other.m_position))
	, m_velocity(std::move(other.m_velocity))
	, m_mass(std::move(other.m_mass))
	, m_forces(std::move(other.m_forces))
	, m_impulses(std::move(other.m_impulses))
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

void BodyImpl::SetMass(const Mass& mass)
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

void BodyImpl::ApplyForce(const fVec2D& force)
{
	m_forces.push_back(force);
}

void BodyImpl::ApplyImpulse(const fVec2D& impulse)
{
	m_impulses.push_back(impulse);
}

void BodyImpl::Update(float dt)
{
	// Summarize all impulses applied to this body on this step
	const fVec2D impulses = std::accumulate(std::begin(m_impulses), std::end(m_impulses), fVec2D(0, 0));

	// Summarize all forces applied to this body on this step
	const fVec2D forces = std::accumulate(std::begin(m_forces), std::end(m_forces), fVec2D(0, 0));
	const fVec2D acceleration = forces * m_mass.inv_mass;

	m_position += dt * m_velocity + 0.5f * acceleration * dt * dt;
	m_velocity += dt * acceleration + impulses * m_mass.inv_mass;

	m_impulses.clear();
	m_forces.clear();
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