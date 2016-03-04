#include <phys_engine.h>
#include <phys_constants.h>

#include <algorithm>
#include <vector>

using namespace physic;

class EngineImpl : public IEngine
{
public:
	virtual void SetWorldSize(
		int left = 0, 
		int bot = 0, 
		int right = kWorldMarginRight, 
		int top = kWorldMarginTop) override;

	virtual void SetWorldConstants(
		float gravity = kGravity, 
		float air_density = kAirDragFactor, 
		float ground_friction = kGroundFriction) override;
	
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

	int m_botBorder;
	int m_leftBorder;
	int m_upBorder;
	int m_rightBorder;

	std::vector<BodyPtr> m_bodies;

	fVec2D m_gravity;
	float m_airDrag;
	float m_groundFricion;
};

void EngineImpl::SetWorldSize(int left, int bot, int right, int top)
{
	// Set world margins
	m_botBorder = bot;
	m_upBorder = top;
	m_rightBorder = right;
	m_leftBorder = left;
}

void EngineImpl::SetWorldConstants(float gravity, float air_drag, float ground_friction)
{
	// Set world margins
	m_gravity = fVec2D(gravity, fAngle(-90.));
	m_airDrag = air_drag;
	m_groundFricion = ground_friction;
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
		fVec2D velocity = body->GetVelocityVector();
		fVec2D position = body->GetPosition();
		const float mass = body->GetMass();
		const float kBounceFactor = body->GetBounceFactor();

		fVec2D ground_friction_force = { 0, 0 };

		// Check restrictions. Body will bounce at world margins.
		if (position.x >= m_rightBorder || position.x <= m_leftBorder)
		{
			velocity.x *= -kBounceFactor;
			position.x = Clip(position.x,
				static_cast<fVec2D::type>(m_leftBorder),
				static_cast<fVec2D::type>(m_rightBorder));
		}

		if (position.y >= m_upBorder || position.y <= m_botBorder)
		{
			// Applay ground frictions simulation
			if (position.y <= m_botBorder)
				// Vector of force is negative to velocity vector
				ground_friction_force = -m_groundFricion * EuclideanNorm(m_gravity) * mass * velocity
											/ EuclideanNorm(velocity);

			velocity.y *= -kBounceFactor;
			position.y = Clip(position.y,
				static_cast<fVec2D::type>(m_botBorder),
				static_cast<fVec2D::type>(m_upBorder));
		}

		//Apply air drag force
		fVec2D air_drag_force = m_airDrag * -velocity;
		// Summ of force fectors, gravity force and air drag force
		fVec2D force = m_gravity * mass + air_drag_force + ground_friction_force;
		fVec2D acceleration = force / mass;

		// Change position and velocity
		position += dt * (velocity + (0.5f * acceleration * dt));
		velocity += dt * acceleration;
		
		body->SetPosition(position);
		body->SetVelocityVector(velocity);
	}
}

EngineImpl::EngineImpl()
	: m_botBorder(0)
	, m_leftBorder(0)
	, m_upBorder(kWorldMarginTop)
	, m_rightBorder(kWorldMarginRight)
	, m_bodies()
	, m_gravity(0, -kGravity)
	, m_airDrag(kAirDragFactor)
	, m_groundFricion(kGroundFriction)
{

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