#include "phys_engine.h"

#include <algorithm>
#include <chrono>
#include <vector>

class EngineImpl : public IEngine
{
public:
	virtual void SetWorldConstrains(float gravity, int left, int bot, int right, int top) override;
	virtual void AddBody(BodyPtr) override;
	virtual void RemoveBody(BodyPtr) override;
	virtual void Step(float dt) override;

	EngineImpl() {};
	virtual ~EngineImpl() {};
	EngineImpl(const EngineImpl&) = delete;
	EngineImpl& operator=(const EngineImpl&) = delete;

private:

	void checkWorldRestricitons(BodyPtr&);

	using BodyVector = std::vector<BodyPtr>;
	int m_botBorder;
	int m_leftBorder;
	int m_upBorder;
	int m_rightBorder;

	BodyVector m_bodies;

	fVec2D m_gravity;
};

void EngineImpl::SetWorldConstrains(float gravity, int left, int bot, int right, int top)
{
	m_gravity = fVec2D(0, -gravity);

	m_botBorder = bot;
	m_upBorder = top;
	m_rightBorder = right;
	m_leftBorder = left;
}

void EngineImpl::AddBody(BodyPtr body)
{
	m_bodies.push_back(body);
}

void EngineImpl::RemoveBody(BodyPtr body)
{
	m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), body), m_bodies.end());
}

void EngineImpl::Step(float dt)
{
	for (auto& body : m_bodies)
	{
		fVec2D velocity = body->GetVelocityVector();
		fVec2D position = body->GetPosition();

		velocity += m_gravity * dt;
		position += velocity * dt;

		body->SetPosition(position);
		body->SetVelocityVector(velocity);

		checkWorldRestricitons(body);
	}
}

void EngineImpl::checkWorldRestricitons(BodyPtr& body)
{
	fVec2D pos = body->GetPosition();
	fVec2D vel = body->GetVelocityVector();
	if (pos.x >= m_rightBorder || pos.x < m_leftBorder)
		body->SetVelocityVector(fVec2D(-vel.x, vel.y));

	if (pos.y >= m_upBorder || pos.y < m_botBorder)
		body->SetVelocityVector(fVec2D(vel.x, -vel.y));
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