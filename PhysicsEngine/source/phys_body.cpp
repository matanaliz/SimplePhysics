#include <phys_body.h>

// TODO move BodyImpl ot private include header
class BodyImpl : public IBody
{
public:
	BodyImpl() = default;
	~BodyImpl() = default;
	BodyImpl(const BodyImpl&) = default;
	BodyImpl& operator=(const BodyImpl&) = default;

	// TODO implement move ctors
	BodyImpl(BodyImpl&&);
	BodyImpl& operator=(BodyImpl&&);

	virtual fVec2D GetPosition() const override;
	virtual void SetPosition(const fVec2D&) override;

	virtual float GetMass() const override;
	virtual void SetMass(float) override;

	virtual fVec2D GetVelocityVector() const override;
	virtual void SetVelocityVector(const fVec2D&) override;

private:

	float m_mass;
	fVec2D m_position;
	fVec2D m_velocityVector;
};

fVec2D BodyImpl::GetPosition() const
{
	return m_position;
}

void BodyImpl::SetPosition(const fVec2D& val)
{
	m_position = val;
}

float BodyImpl::GetMass() const
{
	return m_mass;
}

void BodyImpl::SetMass(float mass)
{
	m_mass = mass;
}

fVec2D BodyImpl::GetVelocityVector() const
{
	return m_velocityVector;
}

void BodyImpl::SetVelocityVector(const fVec2D& val)
{
	m_velocityVector = val;
}

BodyPtr IBody::GetBody()
{
	return std::shared_ptr<IBody>(new (std::nothrow) BodyImpl());
}