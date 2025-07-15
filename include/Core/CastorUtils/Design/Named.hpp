/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_NAMED_H___
#define ___CASTOR_NAMED_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace c3d
{
	template< typename T >
	class NamedBaseT
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom
		 */
		explicit NamedBaseT( T name )noexcept
			: m_name{ c3d::move( name ) }
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom
		 *\return		Le nom
		 */
		T const & getName()const noexcept
		{
			return m_name;
		}

		void rename( T name )noexcept
		{
			m_name = c3d::move( name );
		}

	protected:
		T m_name;
	};
}

#endif
