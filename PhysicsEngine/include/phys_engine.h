#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <phys_platform.h>
#include <phys_body.h>

class PHYS_API IEngine
{
public:
	virtual void SetWorldConstrains(float gravity, int bot, int top, int xmax, int ymax) = 0;
	virtual void AddBody(BodyPtr) = 0;
	virtual void RemoveBody(BodyPtr) = 0;
	virtual void Step(float dt) = 0;

	static IEngine* Instance();
protected:
	IEngine();
	virtual ~IEngine();
};

#endif // PHYS_ENGINE_H