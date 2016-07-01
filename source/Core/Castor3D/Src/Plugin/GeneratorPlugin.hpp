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
	\brief		Classe de plug-in de générateur procédural
	*/
	class GeneratorPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef Subdivider * CreateGeneratorFunction( TextureUnit * p_texture );
		typedef Subdivider * DestroyGeneratorFunction( Subdivider * p_pGenerator );

		typedef CreateGeneratorFunction * PCreateGeneratorFunction;
		typedef DestroyGeneratorFunction * PDestroyGeneratorFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API GeneratorPlugin( Castor::DynamicLibrarySPtr p_library, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeneratorPlugin();
		/**
		 *\~english
		 *\brief		Creates the Importer
		 *\param[in]	p_texture	The texture modified by the generator
		 *\return		The created ProceduralGenerator instance
		 *\~french
		 *\brief		Crée l'Importer
		 *\param[in]	p_texture	La texture modifiée par le générateur
		 *\return		L'instance de ProceduralGenerator créée
		 */
		C3D_API Subdivider * CreateGenerator( TextureUnit * p_texture );
		/**
		 *\~english
		 *\brief		Destroys the given ProceduralGenerator
		 *\param[in]	p_pGenerator	The ProceduralGenerator
		 *\~french
		 *\brief		Détruit le ProceduralGenerator donné
		 *\param[in]	p_pGenerator	Le ProceduralGenerator
		 */
		C3D_API void DestroyGenerator( Subdivider * p_pGenerator );

	private:
		PCreateGeneratorFunction m_pfnCreateGenerator;
		PDestroyGeneratorFunction m_pfnDestroyGenerator;
	};
}

#endif

