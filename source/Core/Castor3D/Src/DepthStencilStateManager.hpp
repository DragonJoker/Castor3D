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
#ifndef ___C3D_DEPTH_STENCIL_STATE_MANAGER_H___
#define ___C3D_DEPTH_STENCIL_STATE_MANAGER_H___

#include "Manager.hpp"
#include "DepthStencilState.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		DepthStencilState manager.
	\~french
	\brief		Gestionnaire de DepthStencilState.
	*/
	class DepthStencilStateManager
		: public Manager< Castor::String, DepthStencilState, Engine >
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
		C3D_API DepthStencilStateManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~DepthStencilStateManager();
		/**
		 *\~english
		 *\brief		Creates and returns a DepthStencilState, given a name
		 *\remark		If a DepthStencilState with the same name exists, none is created
		 *\param[in]	p_name	The DepthStencilState name
		 *\return		The created or existing DepthStencilState
		 *\~french
		 *\brief		Crée et renvoie un DepthStencilState, avec le nom donné
		 *\remark		Si un DepthStencilState avec le même nom existe, aucun n'est créé
		 *\param[in]	p_name	Le nom du DepthStencilState
		 *\return		Le DepthStencilState créé ou existant
		 */
		C3D_API DepthStencilStateSPtr Create( Castor::String const & p_name );
	};
}

#endif
