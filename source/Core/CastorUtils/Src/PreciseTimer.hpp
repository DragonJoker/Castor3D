/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_PRECISE_TIMER_H___
#define ___CASTOR_PRECISE_TIMER_H___

#include "NonCopyable.hpp"
#include "PlatformConfig.hpp"
#include <cstdint>
#include <chrono>

namespace Castor
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
		: public Castor::NonCopyable
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
		 *\return		The time elapsed since the last call, in seconds
		 *\~french
		 *\return		Le temps écoulé depuis le dernier appel, en secondes
		 */
		CU_API double TimeS();
		/**
		 *\~english
		 *\return		The time elapsed since the last call, in milliseconds
		 *\~french
		 *\return		Le temps écoulé depuis le dernier appel, en millisecondes
		 */
		CU_API double TimeMs();

	private:
		clock::time_point DoGetTime()const;

	private:
		clock::time_point m_savedTime;
	};
}

#endif
