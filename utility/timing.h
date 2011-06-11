#ifndef TIMING_HEADER_GUARD
#define TIMING_HEADER_GUARD

#include <SFML/System/Clock.hpp>
#include "generics.h"

namespace timing
{
	////////////////////////////////////////////////////////////
	/// Generic timer
	////////////////////////////////////////////////////////////
	struct GenTimer{
	public:
		////////////////////////////////////////////////////////////
		/// default ctor
		////////////////////////////////////////////////////////////
		GenTimer() :
			Stamp(0.0f),
			Timer(0.0f)
		{
			//void
		}

		////////////////////////////////////////////////////////////
		/// ctor
		////////////////////////////////////////////////////////////
		GenTimer(float p_Stamp, float p_Timer) :
			Stamp(p_Stamp),
			Timer(p_Timer)
		{
			//void
		}

		////////////////////////////////////////////////////////////
		/// copy ctor
		////////////////////////////////////////////////////////////
		GenTimer(const GenTimer& p_Copy) :
			Stamp(p_Copy.Stamp),
			Timer(p_Copy.Timer)
		{
			//void
		}

	public:
		////////////////////////////////////////////////////////////
		/// Assignment operator
		////////////////////////////////////////////////////////////
		inline GenTimer& operator=(const GenTimer& p_Copy)
		{
			Stamp	= p_Copy.Stamp;
			Timer	= p_Copy.Timer;

			return *this;
		}

	public:
		////////////////////////////////////////////////////////////
		/// reset
		////////////////////////////////////////////////////////////
		inline void reset(void)
		{
			Stamp 	= 0.0f;
			Timer	= 0.0f;
		}

	public:

		float Stamp; //timestamp
		float Timer;
	};


	typedef GenericSingleton<sf::Clock>	GlobalTime;
}

#endif //#define TIMING_HEADER_GUARD