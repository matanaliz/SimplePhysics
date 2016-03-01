#ifndef PHYS_PLATFORM_H
#define PHYS_PLATFORM_H

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYS_API __declspec(dllexport)
#else
	#define PHYS_API __declspec(dllimport)
#endif

// TODO Add linux exports

#endif //PHYS_PLATFORM_H