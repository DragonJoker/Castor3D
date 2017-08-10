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
#ifndef ___GL_TEXTURE_STORAGE_H___
#define ___GL_TEXTURE_STORAGE_H___

#include <Texture/TextureStorage.hpp>
#include "Common/GlHolder.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer.
	\remarks	Will use texture buffer objects if available, or pixel buffer objects if not.
	*/
	template< typename Traits >
	class GlTextureStorage
		: public castor3d::TextureStorage
		, public Holder
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	p_type			The storage texture type.
		 *\param[in]	p_layout		The parent layout.
		 *\param[in]	p_cpuAccess		The required CPU access.
		 *\param[in]	p_gpuAccess		The required GPU access.
		 */
		GlTextureStorage( OpenGl & p_gl
						  , GlRenderSystem & renderSystem
						  , castor3d::TextureStorageType p_type
						  , castor3d::TextureLayout & p_layout
						  , castor3d::AccessTypes const & p_cpuAccess
						  , castor3d::AccessTypes const & p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~GlTextureStorage();
		/**
		 *\copydoc		castor3d::TextureStorage::Bind
		 */
		void bind( uint32_t p_index )const override;
		/**
		 *\copydoc		castor3d::TextureStorage::Unbind
		 */
		void unbind( uint32_t p_index )const override;
		/**
		 *\copydoc		castor3d::TextureStorage::Lock
		 */
		uint8_t * lock( castor3d::AccessTypes const & p_lock )override;
		/**
		 *\copydoc		castor3d::TextureStorage::Unlock
		 */
		void unlock( bool p_modified )override;
		/**
		 *\copydoc		castor3d::TextureStorage::Lock
		 */
		uint8_t * lock( castor3d::AccessTypes const & p_lock, uint32_t p_index )override;
		/**
		 *\copydoc		castor3d::TextureStorage::Unlock
		 */
		void unlock( bool p_modified, uint32_t p_index )override;

		inline GlTextureStorageType getGlType()const
		{
			return m_glType;
		}

		inline GlRenderSystem * getGlRenderSystem()const
		{
			return m_glRenderSystem;
		}

	protected:
		//! The RenderSystem.
		GlRenderSystem * m_glRenderSystem;
		//! The storage type.
		GlTextureStorageType m_glType;
		//! The storage implementation
		Traits m_impl;
	};
}

#include "GlTextureStorage.inl"

#endif
