/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNIQUE_H___
#define ___CASTOR_UNIQUE_H___

#include "UnicityException.hpp"
#include "NonCopyable.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		08/12/2011
	\~english
	\brief		Representation of a Unique instance class
	\remarks	If another instance is to be created, an exception is thrown
	\~french
	\brief		Représentation d'un classe à instance unique
	\remarks	Si une seconde instance est créée, une exception est lancée
	*/
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
		inline Unique( T * p_this )
		{
			if ( !doGetInstance() )
			{
				doGetInstance() = p_this;
			}
			else
			{
				UNICITY_ERROR( UnicityError::eAnInstance, typeid( T ).name() );
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
			static T * pInstance = nullptr;
			return pInstance;
		}
	};
}

#endif
