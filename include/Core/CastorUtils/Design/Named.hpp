/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_NAMED_H___
#define ___CASTOR_NAMED_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace castor
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
		explicit NamedBaseT( T const & name )
			: m_name( name )
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	named	L'objet à copier
		 */
		NamedBaseT( NamedBaseT const & named ) = default;
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	named	L'objet à déplacer
		 */
		NamedBaseT( NamedBaseT && named ) = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~NamedBaseT() = default;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	named	L'objet à copier
		 */
		NamedBaseT & operator=( NamedBaseT const & named ) = default;
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	named	L'objet à déplacer
		 */
		NamedBaseT & operator=( NamedBaseT && named ) = default;
		/**
		 *\~english
		 *\brief		Retrieves the name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom
		 *\return		Le nom
		 */
		T const & getName()const
		{
			return m_name;
		}

		void rename( T const & name )
		{
			m_name = name;
		}

	protected:
		T m_name;
	};
}

#endif
