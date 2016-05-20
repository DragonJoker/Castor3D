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
#ifndef ___GL_PBO_TEXTURE_STORAGE_H___
#define ___GL_PBO_TEXTURE_STORAGE_H___

#include "GlTextureStorage.hpp"

#include "OpenGl.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using pixel buffer objects.
	*/
	class GlPboTextureStorage
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
		GlPboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, Castor3D::eTEXTURE_TYPE p_type, Castor3D::TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~GlPboTextureStorage();
		/**
		 *\copydoc		Castor3D::TextureStorage::Bind
		 */
		virtual bool Bind( uint32_t p_index );
		/**
		 *\copydoc		Castor3D::TextureStorage::Unbind
		 */
		virtual void Unbind( uint32_t p_index );
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		virtual uint8_t * Lock( uint32_t p_lock );
		/**
		 *\copydoc		Castor3D::TextureStorage::Unlock
		 */
		virtual void Unlock( bool p_modified );

	private:
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoFill
		 */
		virtual void DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\brief		Uploads the image pixels in asynchonous mode.
		 */
		void DoUploadAsync();
		/**
		 *\brief		Uploads the image pixels in synchonous mode.
		 */
		void DoUploadSync();
		/**
		 *\brief		Downloads the image pixels in asynchronous mode.
		 */
		void DoDownloadAsync();
		/**
		 *\brief		Downloads the image pixels in synchronous mode.
		 */
		void DoDownloadSync();
		/**
		 *\brief		Uploads the image pixels to the current storage (PBO or direct texture).
		 */
		void DoUploadImage( uint32_t p_width, uint32_t p_height, OpenGl::PixelFmt const & p_format, uint8_t const * p_buffer );

	private:
		//! The pixel transfer buffer, used to upload the pixels.
		GlUploadPixelBufferUPtr m_uploadBuffer;
		//! The pixel transfer buffer, used to download the pixels.
		GlDownloadPixelBufferUPtr m_downloadBuffer;
	};
}

#endif
