﻿/*
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
#ifndef ___C3D_DIVIDER_PLUGIN_H___
#define ___C3D_DIVIDER_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Divider Plugin class
	\~french
	\brief		Classe de plugin de subdivision
	*/
	class C3D_API DividerPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef Subdivider * CreateDividerFunction();
		typedef void DestroyDividerFunction( Subdivider * p_pDivider );
		typedef Castor::String GetDividerTypeFunction();

		typedef CreateDividerFunction * PCreateDividerFunction;
		typedef DestroyDividerFunction * PDestroyDividerFunction;
		typedef GetDividerTypeFunction * PGetDividerTypeFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 *\param[in]	p_engine	Le moteur
		 */
		DividerPlugin( Castor::DynamicLibrarySPtr p_pLibrary, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DividerPlugin();
		/**
		 *\~english
		 *\brief		Creates the Subdivider
		 *\return		The created Subdivider instance
		 *\~french
		 *\brief		Crée le Subdivider
		 *\return		L'instance de Subdivider créée
		 */
		Subdivider * CreateDivider();
		/**
		 *\~english
		 *\brief		Destroys the given Subdivider
		 *\param[in]	p_pDivider	The Subdivider
		 *\~french
		 *\brief		Détruit le Subdivider donné
		 *\param[in]	p_pDivider	Le Subdivider
		 */
		void DestroyDivider( Subdivider * p_pDivider );
		/**
		 *\~english
		 *\brief		Tells the divider short name
		 *\~french
		 *\brief		Donne le nom court du diviseur
		 */
		Castor::String GetDividerType();

	private:
		PCreateDividerFunction m_pfnCreateDivider;
		PDestroyDividerFunction m_pfnDestroyDivider;
		PGetDividerTypeFunction m_pfnGetDividerType;
	};
}

#endif

