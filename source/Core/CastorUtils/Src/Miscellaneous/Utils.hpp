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

namespace castor
{
	namespace System
	{
		/**
		 *\~english
		 *\brief		Suspends the execution of the calling thread for a given time
		 *\param[in]	p_uiTime	The suspend time in milliseconds
		 *\~french
		 *\brief		Suspend l'exécution du thread courant pour un temps donné
		 *\param[in]	p_uiTime	Le temps de suspension en millisecondes
		 */
		CU_API void sleep( uint32_t p_uiTime );
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
		CU_API bool getScreenSize( uint32_t p_screen, Size & p_size );
		/**
		 *\~english
		 *\brief		Retrieves the last system error text
		 *\return		The error text
		 *\~french
		 *\brief		Récupère le texte de la dernière erreur système
		 *\return		Le texte
		 */
		CU_API String getLastErrorText();
	}
	/**
	 *\see			localtime
	 */
	void getLocaltime( std::tm * p_tm, time_t const * p_pTime );
}

#endif
