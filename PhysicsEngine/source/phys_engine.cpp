#include <phys_engine.h>
#include <phys_constants.h>

#include <algorithm>
#include <chrono>
#include <vector>

using namespace physic;

class EngineImpl : public IEngine
{
public:
	virtual void SetWorldConstrains(
		int left = 0, 
		int bot = 0, 
		int right = kWorldMarginRight, 
		int top = kWorldMarginTop) override;
	virtual void SetGravity(float gravity = kGravity) override;
	virtual void AddBody(const BodyPtr&) override;
	virtual void RemoveBody(const BodyPtr&) override;
	virtual void Step(double dt) override;

	EngineImpl();
	virtual ~EngineImpl() = default;

	EngineImpl(const EngineImpl&) = delete;
	EngineImpl& operator=(const EngineImpl&) = delete;
	EngineImpl(EngineImpl&&) = delete;
	EngineImpl& operator=(EngineImpl&&) = delete;

private:

	void checkWorldRestricitons(BodyPtr&);

	int m_botBorder;
	int m_leftBorder;
	int m_upBorder;
	int m_rightBorder;

	std::vector<BodyPtr> m_bodies;

	fVec2D m_gravity;
};

void EngineImpl::SetWorldConstrains(int left, int bot, int right, int top)
{
	// Set world margins
	m_botBorder = bot;
	m_upBorder = top;
	m_rightBorder = right;
	m_leftBorder = left;
}

void EngineImpl::SetGravity(float gravity /*= kGravity*/)
{
	m_gravity = fVec2D(gravity, fAngle(-90.));
}

void EngineImpl::AddBody(const BodyPtr& body)
{
	m_bodies.push_back(body);
}

void EngineImpl::RemoveBody(const BodyPtr& body)
{
	m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), body), m_bodies.end());
}

void EngineImpl::Step(double dt)
{
	for (auto& body : m_bodies)
	{
		/* TODO Check body mass
				More intensive calculations can be done here like air density, etc */
		fVec2D velocity = body->GetVelocityVector();
		fVec2D position = body->GetPosition();

		position += dt * (velocity + dt * (m_gravity / 2));
		velocity += dt * m_gravity;
		
		body->SetPosition(position);
		body->SetVelocityVector(velocity);
		
		/* TODO This can be optimized with some sparce grid and check only wold margin quadrants.
		also this can be paralleled, collision detection in quadrants etc. */
		checkWorldRestricitons(body);
	}
}

EngineImpl::EngineImpl()
	: m_botBorder(0)
	, m_leftBorder(0)
	, m_upBorder(kWorldMarginTop)
	, m_rightBorder(kWorldMarginRight)
	, m_bodies()
	, m_gravity(0, -kGravity)
{

}

void EngineImpl::checkWorldRestricitons(BodyPtr& body)
{
	float kBounceConstant = body->GetBounceFactor();
	
	fVec2D pos = body->GetPosition();

	// Check restrictions. Body will bounce at world margins.
	fVec2D vel = body->GetVelocityVector();
	if (pos.x >= m_rightBorder || pos.x < m_leftBorder)
	{
		vel.x *= -kBounceConstant;
		pos.x = Clip(pos.x, 
			static_cast<fVec2D::type>(m_leftBorder), 
			static_cast<fVec2D::type>(m_rightBorder));
	}

	if (pos.y >= m_upBorder || pos.y < m_botBorder)
	{
		vel.y *= -kBounceConstant;
		pos.y = Clip(pos.y, 
			static_cast<fVec2D::type>(m_botBorder), 
			static_cast<fVec2D::type>(m_upBorder));
	}
		
	body->SetVelocityVector(vel);
	body->SetPosition(pos);
}

IEngine* IEngine::Instance()
{
	static EngineImpl _instance;
	return &_instance;
}

IEngine::IEngine()
{

}

IEngine::~IEngine()
{

}