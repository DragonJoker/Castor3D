/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PRECISE_TIMER_H___
#define ___CASTOR_PRECISE_TIMER_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"
#include <cstdint>
#include <chrono>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Millisecond precise timer representation
	\~french
	\brief		Représentation d'un timer précis à la milliseconde
	*/
	class PreciseTimer
	{
		typedef std::chrono::high_resolution_clock clock;

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
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~PreciseTimer();
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
