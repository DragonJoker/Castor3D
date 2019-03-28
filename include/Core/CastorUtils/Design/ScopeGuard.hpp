/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SCOPE_GUARD_H___
#define ___CASTOR_SCOPE_GUARD_H___

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		05/09/2016
	\~english
	\brief		Class used to execute code at scope exit.
	\~french
	\brief		Classe utilisée pour exécuter du code à la sortie d'un scope.
	*/
	template< typename ScopeExitFuncType >
	class ScopeGuard
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	function	La fonction.
		 */
		explicit ScopeGuard( ScopeExitFuncType const & function )
			: m_function{ function }
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor.
		 *\param[in]	rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	rhs	L'objet à copier.
		 */
		ScopeGuard( ScopeGuard const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Move Constructor.
		 *\param[in]	rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	rhs	L'objet à déplacer.
		 */
		ScopeGuard( ScopeGuard && rhs ) = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ScopeGuard()
		{
			m_function();
		}
		/**
		 *\~english
		 *\brief		Copy Constructor.
		 *\param[in]	rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	rhs	L'objet à copier.
		 */
		ScopeGuard & operator=( ScopeGuard const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Move Constructor.
		 *\param[in]	rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	rhs	L'objet à déplacer.
		 */
		ScopeGuard & operator=( ScopeGuard && rhs ) = default;

	protected:
		ScopeExitFuncType m_function;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a ScopeGuard.
	 *\param[in]	function	The function.
	 *\return		The ScopeGuard.
	 *\~french
	 *\brief		Fonction d'aide à la création d'un ScopeGuard.
	 *\param[in]	function	La fonction.
	 *\return		Le ScopeGuard.
	 */
	template< typename ScopeExitFuncType >
	ScopeGuard< ScopeExitFuncType > makeScopeGuard( ScopeExitFuncType const & function )
	{
		return ScopeGuard< ScopeExitFuncType >( function );
	}
}

#endif
