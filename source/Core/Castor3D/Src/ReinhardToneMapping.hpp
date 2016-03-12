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
#ifndef ___C3D_REINHARD_TONE_MAPPING_H___
#define ___C3D_REINHARD_TONE_MAPPING_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Implements Reinhard tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Reinhard
	*/
	class ReinhardToneMapping
		: Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	p_engine	The engine.
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API ReinhardToneMapping( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ReinhardToneMapping();
		/**
		 *\~english
		 *\brief		Initialises Reinhard tone mapping shader.
		 *\~french
		 *\brief		Initialise le shader de mappage de ton de Reinhard.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Applies the tone mapping for given HDR texture.
		 *\param[in]	p_texture	The HDR texture.
		 *\~english
		 *\brief		Applique le mappage de ton pour la texture HDR donnée.
		 *\param[in]	p_texture	La texture HDR.
		 */
		C3D_API void Apply( Castor::Size const & p_size, Texture const & p_texture );

	protected:
		//!\~english The Reinhard tone mapping shader program.	\~french Le shader de mappage de ton de Reinhard
		ShaderProgramSPtr m_program;
	};
}

#endif

