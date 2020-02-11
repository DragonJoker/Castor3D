/*
See LICENSE file in root folder
*/
#ifndef ___CU_BLOCK_GUARD_H___
#define ___CU_BLOCK_GUARD_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include <functional>

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		11/12/2016
	\~english
	\brief		Allows to declare a scoped variable with an action on construction
				<br />and an action on destruction.
	\remarks	Useful to force execution of code when an exception is thrown.
	\b Example
	@code
		char * buffer = nullptr;
		{
			auto guard = makeBlockGuard( [&buffer, &size]()
			{
				buffer = new char[size + 1];
			},
			[&buffer]()
			{
				delete [] buffer;
			} );
			//
			// ... Code that might throw an exception ...
			//
		}
	@endcode
	\~french
	\brief		Permet de déclarer une variable de scope, avec une action à la construction
				<br />et une action à la destruction.
	\remarks	Utile pour forcer l'exécution de code, si une exception est lancée.
	\b Example
	@code
		char * buffer = nullptr;
		{
			auto guard = makeBlockGuard( [&buffer, &size]()
			{
				buffer = new char[size + 1];
			},
			[&buffer]()
			{
				delete [] buffer;
			} );
			//
			// ... Code pouvant lancer une exception ...
			//
		}
	@endcode
	*/
	template< typename CleanFunc >
	struct BlockGuard
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Executes the init action.
		 *\param[in]	init	The action executed on construction.
		 *\param[in]	clean	The action executed on destruction.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		Exécute l'action d'initialisation.
		 *\param[in]	init	L'action effectuée à la construction.
		 *\param[in]	clean	L'action effectuée lors de la destruction.
		*/
		template< typename InitFunc >
		BlockGuard( InitFunc init, CleanFunc clean )
			: m_clean( clean )
		{
			init();
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Executes the clean action.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Exécute l'action de nettoyage.
		 */
		~BlockGuard()
		{
			m_clean();
		}

	private:
		//!\~english	The clean action.
		//!\~french		L'action de nettoyage.
		CleanFunc m_clean;
	};
	/**
	 *\~english
	 *\brief		Helper function to declare a BlockGuard.
	 *\param[in]	init	The action executed on construction.
	 *\param[in]	clean	The action executed on destruction.
	 *\return		The block guard.
	 *\~french
	 *\brief		fonction d'aide à la construction d'un BlockGuard.
	 *\param[in]	init	L'action effectuée à la construction.
	 *\param[in]	clean	L'action effectuée lors de la destruction.
	 *\return		La garde.
	*/
	template< typename InitFunc, typename CleanFunc >
	BlockGuard< CleanFunc > makeBlockGuard( InitFunc init, CleanFunc clean )
	{
		return BlockGuard< CleanFunc >( init, clean );
	}
}

#endif
