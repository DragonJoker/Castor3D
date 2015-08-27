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
#ifndef ___C3D_SAMPLER_RENDERER_H___
#define ___C3D_SAMPLER_RENDERER_H___

#include "Castor3DPrerequisites.hpp"

#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Sampler renderer
	\~french
	\brief		Rendereur de sampler
	*/
	class C3D_API SamplerRenderer
		:	public Renderer< Sampler, SamplerRenderer >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param		p_pRenderSystem	Le render system
		 */
		SamplerRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SamplerRenderer();
		/**
		 *\~english
		 *\brief		Initialises the sampler
		 *\return		\p true if ok
		 *\~french
		 *\brief		Initialise le sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the sampler
		 *\param[in]	p_eDimension	The texture dimension
		 *\param[in]	p_uiIndex		The sampler index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_eDimension	La dimension de la texture
		 *\param[in]	p_uiIndex		L'index du sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex ) = 0;
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\~french
		 *\brief		Enlève le sampler
		 */
		virtual void Unbind() = 0;
	};
}

#pragma warning( pop )

#endif
