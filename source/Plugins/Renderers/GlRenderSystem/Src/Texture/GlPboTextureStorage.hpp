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
#ifndef ___GL_PBO_TEXTURE_STORAGE_H___
#define ___GL_PBO_TEXTURE_STORAGE_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using pixel buffer objects.
	*/
	class GlPboTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		GlPboTextureStorageTraits( Castor3D::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlPboTextureStorageTraits();
		/**
		 *\copydoc		Castor3D::TextureStorage::Bind
		 */
		bool Bind( Castor3D::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unbind
		 */
		void Unbind( Castor3D::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		uint8_t * Lock( Castor3D::TextureStorage & p_storage, Castor3D::AccessType p_lock, uint32_t p_index );
		/**
		 *\copydoc		Castor3D::TextureStorage::Unlock
		 */
		void Unlock( Castor3D::TextureStorage & p_storage, bool p_modified, uint32_t p_index );
		/**
		*\copydoc		Castor3D::TextureStorage::Fill
		*/
		void Fill( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage const & p_image );

	private:
		/**
		 *\brief		Uploads the image pixels in asynchonous mode.
		 */
		void DoUploadAsync( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage const & p_image );
		/**
		 *\brief		Uploads the image pixels in synchonous mode.
		 */
		void DoUploadSync( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage const & p_image );
		/**
		 *\brief		Downloads the image pixels in asynchronous mode.
		 */
		void DoDownloadAsync( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage & p_image );
		/**
		 *\brief		Downloads the image pixels in synchronous mode.
		 */
		void DoDownloadSync( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage & p_image );
		/**
		 *\brief		Uploads the image pixels to the given storage (PBO or direct texture).
		 */
		void DoUploadImage( Castor3D::TextureStorage & p_storage, Castor3D::TextureImage const & p_image, uint8_t const * p_buffer );

	private:
		//! The parent storage.
		Castor3D::TextureStorage & m_storage;
		//! The pixel transfer buffer, used to upload the pixels.
		GlUploadPixelBufferUPtr m_uploadBuffer;
		//! The pixel transfer buffer, used to download the pixels.
		GlDownloadPixelBufferUPtr m_downloadBuffer;
	};
}

#endif
