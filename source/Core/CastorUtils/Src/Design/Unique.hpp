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
#ifndef ___CASTOR_UNIQUE_H___
#define ___CASTOR_UNIQUE_H___

#include "UnicityException.hpp"
#include "NonCopyable.hpp"

namespace Castor
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
	public:
		/**
		 *\~english
		 *\remarks		Throws an exception if instance is not initialised.
		 *\return		The unique instance.
		 *\~french
		 *\remarks		Lance une exception si l'instance n'est pas initialisée.
		 *\return		L'instance unique.
		 */
		static inline T & GetInstance()
		{
			if ( !DoGetInstance() )
			{
				UNICITY_ERROR( eUNICITY_ERROR_NO_INSTANCE, typeid( T ).name() );
			}

			return *DoGetInstance();
		}

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
			if ( !DoGetInstance() )
			{
				DoGetInstance() = p_this;
			}
			else
			{
				UNICITY_ERROR( eUNICITY_ERROR_AN_INSTANCE, typeid( T ).name() );
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
			DoGetInstance() = nullptr;
		}

	private:
		/**
		 *\~english
		 *\return		The unique instance, nullptr if none.
		 *\~french
		 *\return		L'instance unique, nullptr s'il n'y en a pas.
		 */
		static inline T *& DoGetInstance()
		{
			static T * l_pInstance = nullptr;
			return l_pInstance;
		}
	};
}

#endif
