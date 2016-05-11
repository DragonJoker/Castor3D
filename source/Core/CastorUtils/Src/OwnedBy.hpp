/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_OWNED_BY_H___
#define ___CASTOR_OWNED_BY_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/10/2015
	\~english
	\brief		Class used to have an object owned by another one.
	\~french
	\brief		Classe permettant d'avoir un objet controlé par un autre.
	*/
	template< class Owner >
	class OwnedBy
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_owner		The owner object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_owner		L'objet propriétaire.
		 */
		inline OwnedBy( Owner & p_owner )
			: m_owner( p_owner )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~OwnedBy()
		{
		}
		/**
		 *\~english
		 *\return		The owner object.
		 *\~french
		 *\brief		L'objet propriétaire.
		 */
		inline Owner * GetOwner()const
		{
			return &m_owner;
		}

	private:
		Owner & m_owner;
	};

	/**
	 *\~english
	 *\brief		Helper macro to declare an exported OwnedBy specialisation.
	 *\remarks		Must be used in namespace Castor.
	 *\~french
	 *\brief		Macro pour déclarer une spécialisation exportée de OwnedBy.
	 *\remarks		Doit être utilisée dans le namespace Castor.
	 */
#	define DECLARED_EXPORTED_OWNED_BY( exp, owner, name )\
	template<>\
	class exp OwnedBy< owner >\
	{\
	private:\
		OwnedBy & operator=( OwnedBy< owner > const & p_rhs ) = delete;\
	public:\
		OwnedBy( owner & p_owner );\
		OwnedBy( OwnedBy< owner > const & p_rhs ) = default;\
		OwnedBy( OwnedBy< owner > && p_rhs ) = default;\
		OwnedBy & operator=( OwnedBy< owner > && p_rhs ) = default;\
		~OwnedBy() = default;\
		owner * Get##name()const;\
	private:\
		owner & m_owner;\
	}

	/**
	 *\~english
	 *\brief		Helper macro to implement an OwnedBy specialisation.
	 *\remarks		Must be used in namespace Castor.
	 *\~french
	 *\brief		Macro pour implémenter une spécialisation de OwnedBy.
	 *\remarks		Doit être utilisée dans le namespace Castor.
	 */
#	define IMPLEMENT_EXPORTED_OWNED_BY( owner, name )\
	OwnedBy< owner >::OwnedBy( owner & p_owner )\
		: m_owner( p_owner )\
	{\
	}\
	owner * OwnedBy< owner >::Get##name()const\
	{\
		return &m_owner;\
	}
}

#endif
