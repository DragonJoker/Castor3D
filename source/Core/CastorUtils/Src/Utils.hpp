/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_SYSTEM_UTILS___
#define ___CASTOR_SYSTEM_UTILS___

#include "CastorUtilsPrerequisites.hpp"

#include <ctime>

namespace Castor
{
	namespace System
	{
		/**
		 *\~english
		 *\brief		Retrieves the CPU count (physical + cores)
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre de CPU (physiques + coeurs)
		 *\return		Le compte
		 */
		CU_API uint8_t GetCPUCount();
		/**
		 *\~english
		 *\brief		Suspends the execution of the calling thread for a given time
		 *\param[in]	p_uiTime	The suspend time in milliseconds
		 *\~french
		 *\brief		Suspend l'exécution du thread courant pour un temps donné
		 *\param[in]	p_uiTime	Le temps de suspension en millisecondes
		 */
		CU_API void Sleep( uint32_t p_uiTime );
		/**
		 *\~english
		 *\brief		Retrieves the given screen size
		 *\param[in]	p_screen	The screen index
		 *\param[out]	p_size		Receives the screen size
		 *\return		\p true if the size was retrieved
		 *\~french
		 *\brief		Récupère la taille de l'écran donné
		 *\param[in]	p_screen	L'indice de l'écran
		 *\param[out]	p_size		Reçoit la taille
		 *\return		\p true si la taille a été récupérée
		 */
		CU_API bool GetScreenSize( uint32_t p_screen, Size & p_size );
		/**
		 *\~english
		 *\brief		Retrieves the last system error text
		 *\return		The error text
		 *\~french
		 *\brief		Récupère le texte de la dernière erreur système
		 *\return		Le texte
		 */
		CU_API String GetLastErrorText();
	}
	/**
	 *\see			localtime
	 */
	void Localtime( std::tm * p_tm, time_t const * p_pTime );
	/**
	 *\~english
	 *\param[in]	p_flags	The flags.
	 *\param[in]	p_flag	The flag looked for.
	 *\return		\p true if p_flags contain p_flag.
	 *\~french
	 *\param[in]	p_flags	Les indicateurs.
	 *\param[in]	p_flag	L'indicateur recherché.
	 *\return		\p true si p_flags contient p_flag.
	 */
	template< typename T, typename U >
	inline bool CheckFlag( T p_flags, U p_flag )
	{
		return ( p_flags & p_flag ) == p_flag;
	}
}

#endif
