/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNIQUE_H___
#define ___CASTOR_UNIQUE_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Design/UnicityException.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	template< class T >
	class Unique
		: private NonCopyable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Throws an exception if the instance is already initialised.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		Lance une exception si l'instance est déjà initialisée.
		 */
		inline explicit Unique( T * pThis )
		{
			if ( !doGetInstance() )
			{
				doGetInstance() = pThis;
			}
			else
			{
				CU_UnicityError( UnicityError::eAnInstance, typeid( T ).name() );
			}
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Unique()
		{
			doGetInstance() = nullptr;
		}

	private:
		/**
		 *\~english
		 *\return		The unique instance, nullptr if none.
		 *\~french
		 *\return		L'instance unique, nullptr s'il n'y en a pas.
		 */
		static inline T *& doGetInstance()
		{
			static T * instance = nullptr;
			return instance;
		}
	};
}

#endif
