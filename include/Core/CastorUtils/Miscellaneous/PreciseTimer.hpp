/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PRECISE_TIMER_H___
#define ___CASTOR_PRECISE_TIMER_H___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include <cstdint>
#include <chrono>

namespace castor
{
	class PreciseTimer
	{
		using clock = std::chrono::high_resolution_clock;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API PreciseTimer();
		/**
		 *\~english
		 *\return		The time elapsed since the last call.
		 *\~french
		 *\return		Le temps écoulé depuis le dernier appel.
		 */
		CU_API Nanoseconds getElapsed();

	private:
		clock::time_point doGetElapsed()const;

	private:
		clock::time_point m_savedTime;
	};
}

#endif
