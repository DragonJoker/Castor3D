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
#ifndef ___C3D_BLEND_STATE_MANAGER_H___
#define ___C3D_BLEND_STATE_MANAGER_H___

#include "Manager.hpp"
#include "BlendState.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		BlendState manager.
	\~french
	\brief		Gestionnaire de BlendState.
	*/
	class BlendStateManager
		: public Manager< Castor::String, BlendState >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API BlendStateManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~BlendStateManager();
		/**
		 *\~english
		 *\brief		Creates and returns a BlendState, given a name
		 *\remark		If a BlendState with the same name exists, none is created
		 *\param[in]	p_name	The BlendState name
		 *\return		The created or existing BlendState
		 *\~french
		 *\brief		Crée et renvoie un BlendState, avec le nom donné
		 *\remark		Si un BlendState avec le même nom existe, aucun n'est créé
		 *\param[in]	p_name	Le nom du BlendState
		 *\return		Le BlendState créé ou existant
		 */
		C3D_API BlendStateSPtr Create( Castor::String const & p_name );
	};
}

#endif
