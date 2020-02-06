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
#ifndef ___C3D_GENERATOR_PLUGIN_H___
#define ___C3D_GENERATOR_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Generator Plugin class
	\~french
	\brief		Classe de plugin de générateur procédural
	*/
	class C3D_API GeneratorPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef Subdivider * CreateGeneratorFunction( TextureUnit * p_pTexture );
		typedef Subdivider * DestroyGeneratorFunction( Subdivider * p_pGenerator );

		typedef CreateGeneratorFunction *	PCreateGeneratorFunction;
		typedef DestroyGeneratorFunction *	PDestroyGeneratorFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		GeneratorPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GeneratorPlugin();
		/**
		 *\~english
		 *\brief		Creates the Importer
		 *\param[in]	p_pTexture	The texture modified by the generator
		 *\return		The created ProceduralGenerator instance
		 *\~french
		 *\brief		Crée l'Importer
		 *\param[in]	p_pTexture	La texture modifiée par le générateur
		 *\return		L'instance de ProceduralGenerator créée
		 */
		Subdivider * CreateGenerator( TextureUnit * p_pTexture );
		/**
		 *\~english
		 *\brief		Destroys the given ProceduralGenerator
		 *\param[in]	p_pGenerator	The ProceduralGenerator
		 *\~french
		 *\brief		Détruit le ProceduralGenerator donné
		 *\param[in]	p_pGenerator	Le ProceduralGenerator
		 */
		void DestroyGenerator( Subdivider * p_pGenerator );

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		GeneratorPlugin( GeneratorPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		GeneratorPlugin( GeneratorPlugin && p_plugin );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		GeneratorPlugin & operator =( GeneratorPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		GeneratorPlugin & operator =( GeneratorPlugin && p_plugin );

	private:
		PCreateGeneratorFunction m_pfnCreateGenerator;
		PDestroyGeneratorFunction m_pfnDestroyGenerator;
	};
}

#endif

