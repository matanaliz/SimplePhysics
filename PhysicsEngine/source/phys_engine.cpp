#include <phys_engine.h>
#include <phys_constants.h>

#include <algorithm>
#include <vector>
#include <iostream>

using namespace physic;

class EngineImpl : public IEngine
{
public:
	virtual void SetWorldMargins(
		Point bot_left = kWorldBotLeft,
		Point top_right = kWorldTopRight) override;

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

	// Return penetration distance if collision occurred, else 0.f
	float checkCollision(const BodyPtr&, const BodyPtr&) const;
	void solveCollision(const BodyPtr&, const BodyPtr&) const;
	Point clipPointToWorldMargins(const Point&) const;

	Point m_botLeft;
	Point m_topRight;

	//int m_botBorder;
	//int m_leftBorder;
	//int m_upBorder;
	//int m_rightBorder;

	std::vector<BodyPtr> m_bodies;

	fVec2D m_gravity;
	float m_airDrag;
	float m_groundFricion;
};

void EngineImpl::SetWorldMargins(Point bot_left, Point top_right)
{
	// Set world margins
	m_botLeft = bot_left;
	m_topRight = top_right;
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
	QuadTree<BodyPtr> tree(0, m_botLeft.x, m_botLeft.y, m_topRight.x, m_topRight.y);

	for (const auto& body : m_bodies)
		tree.insert(body);

	for (auto& body : m_bodies)
	{
		fVec2D velocity = body->GetVelocityVector();
		Point position = body->GetPosition();

		position = clipPointToWorldMargins(position);

		const float mass = body->GetMass();
		const float kBounceFactor = body->GetBounceFactor();

		auto colliding = tree.locate(body);
		for (auto collide : colliding)
		{
			//search_collisions++;
			double peneration = checkCollision(body, collide);
			if (peneration > 0.f)
				solveCollision(body, collide);
		}

		fVec2D ground_friction_force = { 0, 0 };

		// Check restrictions. Body will bounce at world margins.
		if (position.x >= m_topRight.x || position.x <= m_botLeft.x)
			velocity.x *= -kBounceFactor;

		if (position.y >= m_topRight.y || position.y <= m_botLeft.y)
		{
			// Apply ground frictions simulation
			if (position.y <= m_botLeft.y)
				// Vector of force is negative to velocity vector
				if (m_groundFricion != 0.f)
					ground_friction_force = -m_groundFricion * EuclideanNorm(m_gravity) * mass * velocity
												/ EuclideanNorm(velocity);

			velocity.y *= -kBounceFactor;
		}

		//Apply air drag force
		fVec2D air_drag_force = m_airDrag * -velocity;
		// Sum of force vectors, gravity force and air drag force
		fVec2D force = m_gravity * mass + air_drag_force + ground_friction_force;
		fVec2D acceleration = force / mass;

		// Change position and velocity
		position += Round(dt * (velocity + (0.5f * acceleration * dt)));
		velocity += Round(dt * acceleration);
		
		body->SetPosition(position);
		body->SetVelocityVector(velocity);
	}
}

EngineImpl::EngineImpl()
	: m_botLeft(kWorldBotLeft)
	, m_topRight(kWorldTopRight)
	, m_bodies()
	, m_gravity(0, -kGravity)
	, m_airDrag(kAirDragFactor)
	, m_groundFricion(kGroundFriction)
{

}

float EngineImpl::checkCollision(const BodyPtr& body, const BodyPtr& collide) const
{
	if (body == collide)
		return 0.f;

	Point position = body->GetPosition();
	Point collide_position = collide->GetPosition();

	// Get size from body interface
	static unsigned radius = 10;
	fVec2D diff = collide_position - position;
	float penetration = 0.f;

	if ((diff.x * diff.x) + (diff.y * diff.y) <= radius * radius)
		penetration = std::sqrt((radius * radius) - ((diff.x * diff.x) + (diff.y * diff.y)));

	return penetration;
}

void EngineImpl::solveCollision(const BodyPtr& body, const BodyPtr& collide) const
{
	fVec2D velocity = body->GetVelocityVector();
	Point position = body->GetPosition();
	const float mass = body->GetMass();

	fVec2D collide_velocity = collide->GetVelocityVector();
	Point collide_position = collide->GetPosition();
	const float collide_mass = collide->GetMass();

	fVec2D collision_vector = collide_position - position;
	fVec2D collision_normal = Normalized(collision_vector);

	fVec2D relative_vel = collide_velocity - velocity;
	double length_relative = DotProduct(relative_vel, collision_normal);

	// Objects moving in different directions, nothing to solve
	if (length_relative > 0.f + 0.5f)
		return;

	// TODO get inverted mass from body
	double impulse_val = -(1.f + kBounceFactor) * length_relative;
	impulse_val /= (1 / mass) + (1 / collide_mass);

	fVec2D impulse = impulse_val * collision_normal;

	fVec2D new_vel = velocity - impulse * (1 / mass);
	fVec2D new_collide_vel = collide_velocity + impulse * (1 / collide_mass);

	body->SetVelocityVector(new_vel);
	collide->SetVelocityVector(new_collide_vel);
}

Point EngineImpl::clipPointToWorldMargins(const Point& pos) const
{
	return { Clip(pos.x,
				static_cast<Point::type>(m_botLeft.x),
				static_cast<Point::type>(m_topRight.x)),
			 Clip(pos.y,
			 static_cast<Point::type>(m_botLeft.y),
			 static_cast<Point::type>(m_topRight.y)) };
}

IEngine* IEngine::Instance()
{
	static EngineImpl _instance;
	return &_instance;
}