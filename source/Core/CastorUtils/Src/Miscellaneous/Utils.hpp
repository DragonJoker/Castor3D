/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	 *\param[in]	p_value	The value.
	 *\param[in]	p_flag	The flag looked for.
	 *\return		\p true if p_flags contain p_flag.
	 *\~french
	 *\param[in]	p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur recherché.
	 *\return		\p true si p_flags contient p_flag.
	 */
	template< typename T, typename U >
	inline bool CheckFlag( T const & p_value, U const & p_flag )
	{
		static_assert( sizeof( T ) == sizeof( U ), "Can't check flags for different size parameters" );
		return U( p_value & T( p_flag ) ) == p_flag;
	}
	/**
	 *\~english
	 *\brief		Adds a flag to the given value.
	 *\param[in,out]p_value	The value.
	 *\param[in]	p_flag	The flag to add.
	 *\~french
	 *\brief		Ajoute un indicateur à la valeur donnée.
	 *\param[in,out]p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur à ajouter.
	 */
	template< typename T, typename U >
	inline void AddFlag( T & p_value, U const & p_flag )
	{
		static_assert( sizeof( T ) == sizeof( U ), "Can't add flags for different size parameters" );
		p_value |= T( p_flag );
	}
	/**
	 *\~english
	 *\brief		Removes a flag from the given value.
	 *\param[in,out]p_value	The value.
	 *\param[in]	p_flag	The flag to remove.
	 *\~french
	 *\brief		Enlève un indicateur de la valeur donnée.
	 *\param[in,out]p_value	La valeur.
	 *\param[in]	p_flag	L'indicateur à enlever.
	 */
	template< typename T, typename U >
	inline void RemFlag( T & p_value, U const & p_flag )
	{
		static_assert( sizeof( T ) == sizeof( U ), "Can't remove flags for different size parameters" );
		p_value &= ~T( p_flag );
	}
}

#endif
