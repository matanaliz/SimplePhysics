#include <phys_engine.h>
#include <phys_constants.h>
#include <phys_quadtree.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

using namespace physic;

class EngineImpl : public IEngine
{
public:
	virtual void SetWorldBorders(
		Point bot_left = kWorldBotLeft,
		Point top_right = kWorldTopRight) override;

	virtual void SetWorldConstants(
		float gravity = kGravity, 
		float air_density = kAirDragFactor, 
		float ground_friction = kGroundFriction) override;
	
	virtual void AddBody(BodyPtr&) override;
	virtual void RemoveBody(const BodyPtr&) override;
	virtual void Step(double dt) override;

	EngineImpl();
	virtual ~EngineImpl() = default;

	EngineImpl(const EngineImpl&) = delete;
	EngineImpl& operator=(const EngineImpl&) = delete;
	EngineImpl(EngineImpl&&) = delete;
	EngineImpl& operator=(EngineImpl&&) = delete;

private:

	//bool checkBodyOnBorder()
	bool checkCollision(const BodyPtr&, const BodyPtr&) const;

	void solveCollision(BodyPtr&, BodyPtr&) const;

	Point clipPointToWorldBorder(const Point&) const;

	Point m_botLeft;
	Point m_topRight;

	std::vector<BodyPtr> m_bodies;

	fVec2D m_gravity;
	float m_airDrag;
	float m_groundFricion;
};

void EngineImpl::SetWorldBorders(Point bot_left, Point top_right)
{
	// Set world margins
	m_botLeft = bot_left;
	m_topRight = top_right;
}

void EngineImpl::SetWorldConstants(float gravity, float air_drag, float ground_friction)
{
	m_gravity = fVec2D(gravity, fAngle(-90.));
	m_airDrag = air_drag;
	m_groundFricion = ground_friction;
}

void EngineImpl::AddBody(BodyPtr& body)
{
	assert(nullptr != body);
	m_bodies.push_back(body);
}

void EngineImpl::RemoveBody(const BodyPtr& body)
{
	assert(nullptr != body);
	m_bodies.erase(std::remove(std::begin(m_bodies), std::end(m_bodies), body), std::end(m_bodies));
}

void EngineImpl::Step(double dt)
{
	// Fill spatial quadtree
	QuadTree<BodyPtr> tree(0, m_botLeft, m_topRight);
	for (const auto& body : m_bodies)
	{
		tree.insert(body);
	}

	for (auto& body : m_bodies)
	{
		Point position = body->GetPosition();
		
		// TODO clip position in here?
		position = clipPointToWorldBorder(position);
		body->SetPosition(position);

		// Broad phase of collision detection:
		// Look up for neighbours in quadrant
		auto& colliding = tree.locate(body);
		if (!colliding.empty())
			for (auto& collide : colliding)
			{
				// Narrow phase of collision detection
 				if (checkCollision(body, collide))
					solveCollision(body, collide);
			}

		// TODO Clean this up
		// Check restrictions. Body will bounce at world margins.
		const Mass mass = body->GetMass();
		const float kBounceFactor = body->GetBounceFactor();
		fVec2D velocity = body->GetVelocityVector();

		if (position.x >= m_topRight.x || position.x <= m_botLeft.x)
			velocity.x *= -kBounceFactor;
		
		fVec2D ground_friction_force = { 0, 0 };
		if (position.y >= m_topRight.y || position.y <= m_botLeft.y)
		{
			// Apply ground frictions simulation
			if (position.y <= m_botLeft.y)
				// Vector of force is negative to velocity vector
				if (m_groundFricion > 0.f)
					ground_friction_force = -m_groundFricion * EuclideanNorm(m_gravity) * mass.mass * velocity
												/ EuclideanNorm(velocity);

			velocity.y *= -kBounceFactor;
		}

		body->SetVelocityVector(velocity);

		//Apply air drag force
		const fVec2D air_drag_force = m_airDrag * -velocity;
		// Sum of force vectors, gravity force and air drag force
		const fVec2D force = m_gravity * mass.mass + air_drag_force + ground_friction_force;

		body->ApplyForce(force);

		// Run all the calculations for current body
		body->Update(dt);
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

bool EngineImpl::checkCollision(const BodyPtr& body, const BodyPtr& collide) const
{
	assert(nullptr != body);
	assert(nullptr != collide);

	if (body == collide)
		return false;

	// Get size from body interface
	static unsigned radius = 20;
	const fVec2D distance = collide->GetPosition() - body->GetPosition();
	return (distance.x * distance.x) + (distance.y * distance.y) <= radius * radius;
}

void EngineImpl::solveCollision(BodyPtr& body, BodyPtr& collide) const
{
	assert(nullptr != body);
	assert(nullptr != collide);

	const fVec2D velocity = body->GetVelocityVector();
	const fVec2D collide_velocity = collide->GetVelocityVector();

	// Calculate vector between body centers
	const fVec2D collision_vector = collide->GetPosition() - body->GetPosition();
	const fVec2D collision_normal = Normalized(collision_vector);

	const fVec2D relative_velocity = collide_velocity - velocity;
	const double length_relative = DotProduct(relative_velocity, collision_normal);

	// Objects moving in different directions, nothing to solve
	if (length_relative > 0.f)
		return;

	const Mass mass = body->GetMass();
	const Mass collide_mass = collide->GetMass();

	const double length_impulse = -(1.f + kBounceFactor) * length_relative / (mass.inv_mass + collide_mass.inv_mass);

	const fVec2D impulse = length_impulse * collision_normal;

	body->ApplyImpulse(-(impulse));
	collide->ApplyImpulse(impulse);
}

Point EngineImpl::clipPointToWorldBorder(const Point& pos) const
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