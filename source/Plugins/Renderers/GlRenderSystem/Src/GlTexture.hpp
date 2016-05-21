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
#ifndef ___GL_TEXTURE_H___
#define ___GL_TEXTURE_H___

#include "GlObject.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class GlTexture
		: private Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
		, public Castor3D::TextureLayout
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 */
		GlTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem, Castor3D::eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~GlTexture();
		/**
		 *\copydoc		Castor3D::TextureLayout::Create
		 */
		virtual bool Create();
		/**
		 *\copydoc		Castor3D::TextureLayout::Destroy
		 */
		virtual void Destroy();
		/**
		*\brief		Forces mipmaps generation.
		*/
		void GenerateMipmaps()const;
		/**
		*\return	The OpenGL texture dimension.
		*/
		eGL_TEXDIM GetGlDimension()const
		{
			return m_glDimension;
		}

	private:
		/**
		 *\copydoc		Castor3D::TextureLayout::DoBind
		 */
		virtual bool DoBind( uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureLayout::DoUnbind
		 */
		virtual void DoUnbind( uint32_t p_index )const;

	public:
		using ObjectType::IsValid;
		using ObjectType::GetGlName;
		using ObjectType::GetOpenGl;

	private:
		//! The RenderSystem.
		GlRenderSystem * m_glRenderSystem;
		//! The OpenGL texture dimension.
		eGL_TEXDIM m_glDimension;
	};
}

#endif
