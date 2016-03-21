#ifndef PHYS_PLATFORM_H
#define PHYS_PLATFORM_H

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYS_API __declspec(dllexport)
#else
	#define PHYS_API __declspec(dllimport)
#endif

#ifdef _MSC_VER

// Implementation is intentionally forbidding macroized keywords
#define _ALLOW_KEYWORD_MACROS 
// No MSVC does constexpr fully yet
#pragma push_macro("constexpr")
#define constexpr 

// MSVC 2013 workarounds
#if _MSC_VER <= 1800

#pragma push_macro("noexcept")
#define noexcept  

#endif // _MSC_VER <= 1800

#endif // _MSC_VER

// TODO Add linux exports

#endif //PHYS_PLATFORM_H