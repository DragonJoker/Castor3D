/*
See LICENSE file in root folder
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
	void getLocaltime( std::tm *& p_tm, time_t const * p_pTime );
}

#endif
