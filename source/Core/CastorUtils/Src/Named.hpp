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
#ifndef ___CASTOR_NAMED_H___
#define ___CASTOR_NAMED_H___

#include "StringUtils.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Class for named elements
	\remark		The name type is a template argument so anything can be a name for this class (default is Castor::String)
	\~french
	\brief		Classe de base pour les éléments nommés
	\remark		Le nom est un argument template, ainsi n'importe quoi peut être un nom pour cette classe (même si c'est Castor::String par défaut)
	*/
	template< typename T = String >
	class NamedBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom
		 */
		NamedBase( T const & p_name )
			: m_name( p_name )
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_named	L'objet à copier
		 */
		NamedBase( NamedBase const & p_named )
			: m_name( p_named.m_name )
		{
		}
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_named	L'objet à déplacer
		 */
		NamedBase( NamedBase && p_named )
			: m_name()
		{
			m_name = std::move( p_named.m_name );
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~NamedBase()
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_named	L'objet à copier
		 */
		NamedBase & operator=( NamedBase const & p_named )
		{
			m_name = p_named.m_name;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_named	L'objet à déplacer
		 */
		NamedBase & operator=( NamedBase && p_named )
		{
			if ( this != &p_named )
			{
				m_name = std::move( p_named.m_name );
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Retrieves the name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom
		 *\return		Le nom
		 */
		inline T const & GetName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Sets the name
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Définit le nom
		 *\param[in]	p_name	Le nom
		 */
		inline void SetName( T const & p_name )
		{
			m_name = p_name;
		}

	protected:
		T m_name;
	};

	//!\~english A typedef for NamedBase with String as a name	\~french Un typedef pour NamedBase avec String comme type de nom (le plus classique, quoi)
	typedef NamedBase< String > Named;
}

#endif
