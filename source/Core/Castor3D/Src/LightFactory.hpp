/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_LIGHT_FACTORY_H___
#define ___C3D_LIGHT_FACTORY_H___

#include "LightCategory.hpp"

#include <Factory.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		24/11/2014
	\~english
	\brief		The light source factory
	\~french
	\brief		La fabrique de sources lumineuse
	*/
	class C3D_API LightFactory
		:	public Castor::Factory< LightCategory, eLIGHT_TYPE >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		LightFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~LightFactory();
		/**
		 *\~english
		 *\brief		Registers all objects types
		 *\~french
		 *\brief		Enregistre tous les types d'objets
		 */
		virtual void Initialise();
#if !CASTOR_HAS_VARIADIC_TEMPLATES
		/**
		 *\~english
		 *\brief		Creates an object from a key
		 *\param[in]	p_key	The object type
		 *\return		The created object
		 *\~french
		 *\brief		Crée un objet à partir d'une clef (type d'objet)
		 *\param[in]	p_key	Le type d'objet
		 *\return		L'objet créé
		 */
		ObjPtr Create( eLIGHT_TYPE const & p_key )
		{
			ObjPtr l_return;
			ObjMap::iterator l_it = m_registered.find( p_key );

			if ( l_it != m_registered.end() )
			{
				l_return = l_it->second();
			}

			return l_return;
		}
#endif
	};
}

#endif
