/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_NON_COPYABLE_H___
#define ___CASTOR_NON_COPYABLE_H___

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\date		08/12/2011
	\~english
	\brief		The non-copyable concept implementation
	\remark		Forbids a class which derivates from this one to be copied, either way
	\~french
	\brief		Implémentation du concept de non-copiable
	\remark		Interdit la copie des classes dérivant de celle-ci, de quelque manière que ce soit
	*/
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
