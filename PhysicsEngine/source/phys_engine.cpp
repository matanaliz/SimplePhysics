#include <phys_engine.h>
#include <phys_constants.h>

#include <algorithm>
#include <vector>
#include <iostream>

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

	// Return penetration distance if collision occurred, else 0.f
	float checkCollision(const BodyPtr&, const BodyPtr&) const;
	void solveCollision(const BodyPtr&, const BodyPtr&) const;
	fVec2D clipPositionToWorldMargins(const fVec2D&) const;

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
	static int search_collisions = 0;
	static int n2_collisions = 0;

	QuadTree<BodyPtr> tree(0, m_leftBorder, m_botBorder, m_rightBorder, m_upBorder);

	for (const auto& body : m_bodies)
		tree.insert(body);

	//std::vector<BodyPtr> sorted_x = m_bodies;
	//std::vector<BodyPtr> sorted_y = m_bodies;

	//std::sort(sorted_x.begin(), sorted_x.end(),
	//	[](const BodyPtr & a, const BodyPtr & b) -> bool
	//{
	//	return a->GetPosition().x < b->GetPosition().x;
	//});

	//std::sort(sorted_y.begin(), sorted_y.end(),
	//	[](const BodyPtr & a, const BodyPtr & b) -> bool
	//{
	//	return a->GetPosition().y < b->GetPosition().y;
	//});

	for (auto& body : m_bodies)
	{
		fVec2D velocity = body->GetVelocityVector();
		fVec2D position = body->GetPosition();

		position = clipPositionToWorldMargins(position);

		const float mass = body->GetMass();
		const float kBounceFactor = body->GetBounceFactor();

		auto colliding = tree.locate(body);
		for (auto collide : colliding)
		{
			search_collisions++;
			double peneration = checkCollision(body, collide);
			if (peneration > 0.f)
				solveCollision(body, collide);
		}


		//auto find = std::find(sorted_x.begin(), sorted_x.end(), body);
		//if (find != sorted_x.end())
		//{
		//	auto start = find;
		//	auto end = find;
		//	if (find != sorted_x.begin())
		//		--start;
		//	if (find != sorted_x.end())
		//		++end;

		//	for (auto collide = start; collide != end; ++collide)
		//	{
		//		search_collisions++;
		//		double peneration = checkCollision(body, *collide);
		//		if (peneration > 0.f)
		//			solveCollision(body, *collide);
		//	}
		//}

		//find = std::find(sorted_y.begin(), sorted_y.end(), body);
		//if (find != sorted_y.end())
		//{
		//	auto start = find;
		//	auto end = find;
		//	if (find != sorted_y.begin())
		//		--start;
		//	if (find != sorted_y.end())
		//		++end;

		//	for (auto collide = start; collide != end; ++collide)
		//	{
		//		search_collisions++;
		//		double peneration = checkCollision(body, *collide);
		//		if (peneration > 0.f)
		//			solveCollision(body, *collide);
		//	}
		//}

		// O(N^2) collision calculation. Change for quadtree.
		//for (auto& collide : m_bodies)
		//{
		//	n2_collisions++;
		//	double peneration = checkCollision(body, collide);
		//	if (peneration > 0.f)
		//		solveCollision(body, collide);
		//}

		fVec2D ground_friction_force = { 0, 0 };

		// Check restrictions. Body will bounce at world margins.
		if (position.x >= m_rightBorder || position.x <= m_leftBorder)
			velocity.x *= -kBounceFactor;

		if (position.y >= m_upBorder || position.y <= m_botBorder)
		{
			// Apply ground frictions simulation
			if (position.y <= m_botBorder)
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

	std::cout << "Search collisions: " << search_collisions << std::endl;
	std::cout << "n2 collisions: " << n2_collisions << std::endl;
	search_collisions = 0;
	n2_collisions = 0;
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

float EngineImpl::checkCollision(const BodyPtr& body, const BodyPtr& collide) const
{
	if (body == collide)
		return 0.f;

	fVec2D position = body->GetPosition();
	fVec2D collide_position = collide->GetPosition();

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
	fVec2D position = body->GetPosition();
	const float mass = body->GetMass();

	fVec2D collide_velocity = collide->GetVelocityVector();
	fVec2D collide_position = collide->GetPosition();
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

fVec2D EngineImpl::clipPositionToWorldMargins(const fVec2D& pos) const
{
	return { Clip(pos.x,
				static_cast<fVec2D::type>(m_leftBorder),
				static_cast<fVec2D::type>(m_rightBorder)),
			 Clip(pos.y,
				static_cast<fVec2D::type>(m_botBorder),
				static_cast<fVec2D::type>(m_upBorder)) };
}

IEngine* IEngine::Instance()
{
	static EngineImpl _instance;
	return &_instance;
}