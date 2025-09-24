/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SCOPE_GUARD_H___
#define ___CASTOR_SCOPE_GUARD_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace c3d
{
	class ScopeGuard
	{
	public:
		using ScopeExitFuncType = Function< void() >;
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
		ScopeGuard( ScopeGuard && rhs )noexcept
			: m_function{ rhs.m_function }
		{
			rhs.m_function = {};
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ScopeGuard()noexcept
		{
			if ( m_function )
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
		ScopeGuard & operator=( ScopeGuard && rhs )noexcept
		{
			m_function = rhs.m_function;
			rhs.m_function = {};

			return *this;
		}

	private:
		Function< void() > m_function;
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
	ScopeGuard makeScopeGuard( ScopeExitFuncType const & function )
	{
		return ScopeGuard{ function };
	}
}

#endif
