/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
