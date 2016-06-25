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
#ifndef ___GL_IMMUTABLE_TEXTURE_STORAGE_H___
#define ___GL_IMMUTABLE_TEXTURE_STORAGE_H___

#include "GlTextureStorage.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.9.0
	\date		22/05/2016
	\brief		Class used to handle texture storage buffer, using directly the texture (for static textures).
	*/
	class GlImmutableTextureStorage
		: public GlTextureStorage
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_image			The parent image.
		 *\param[in]	p_cpuAccess		The required CPU access.
		 *\param[in]	p_gpuAccess		The required GPU access.
		 */
		GlImmutableTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, Castor3D::TextureStorageType p_type, Castor3D::TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~GlImmutableTextureStorage();
		/**
		 *\copydoc		Castor3D::TextureStorage::Bind
		 */
		virtual bool Bind( uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unbind
		 */
		virtual void Unbind( uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		virtual uint8_t * Lock( uint32_t p_lock );
		/**
		 *\copydoc		Castor3D::TextureStorage::Unlock
		 */
		virtual void Unlock( bool p_modified );

	protected:
		/**
		 *\copydoc		Castor3D::TextureStorage::DoFill
		 */
		virtual void DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
	};
}

#endif
