/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_NON_COPYABLE_H___
#define ___CASTOR_NON_COPYABLE_H___

namespace castor
{
	class NonCopyable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		NonCopyable() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~NonCopyable() {}

	private:
		/**
		 *\~english
		 *\brief		Private copy constructor, to forbid copy by construction
		 *\~french
		 *\brief		Constructeur par copie privé, afin d'en interdire la possiblité
		 */
		NonCopyable( NonCopyable const & ) = delete;
		/**
		 *\~english
		 *\brief		Private copy assignment operator, to forbid assignment copy
		 *\~french
		 *\brief		Operateur d'affectation par copie privé, afin d'en interdire la possibilité
		 */
		NonCopyable & operator =( NonCopyable const & ) = delete;
	};
}

#endif
