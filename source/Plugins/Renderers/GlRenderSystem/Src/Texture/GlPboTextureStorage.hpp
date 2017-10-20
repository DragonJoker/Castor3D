/*
See LICENSE file in root folder
*/
#ifndef ___GL_PBO_TEXTURE_STORAGE_H___
#define ___GL_PBO_TEXTURE_STORAGE_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain doremus.
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
		GlPboTextureStorageTraits( castor3d::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlPboTextureStorageTraits();
		/**
		 *\copydoc		castor3d::TextureStorage::Bind
		 */
		void bind( castor3d::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		castor3d::TextureStorage::Unbind
		 */
		void unbind( castor3d::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		castor3d::TextureStorage::Lock
		 */
		uint8_t * lock( castor3d::TextureStorage & p_storage, castor3d::AccessTypes const & p_lock, uint32_t p_index );
		/**
		 *\copydoc		castor3d::TextureStorage::Unlock
		 */
		void unlock( castor3d::TextureStorage & p_storage, bool p_modified, uint32_t p_index );
		/**
		*\copydoc		castor3d::TextureStorage::Fill
		*/
		void fill( castor3d::TextureStorage & p_storage, castor3d::TextureImage const & p_image );

	private:
		/**
		 *\brief		Uploads the image pixels in asynchonous mode.
		 */
		void doUploadAsync( castor3d::TextureStorage & p_storage, castor3d::TextureImage const & p_image );
		/**
		 *\brief		Uploads the image pixels in synchonous mode.
		 */
		void doUploadSync( castor3d::TextureStorage & p_storage, castor3d::TextureImage const & p_image );
		/**
		 *\brief		downloads the image pixels in asynchronous mode.
		 */
		void doDownloadAsync( castor3d::TextureStorage & p_storage, castor3d::TextureImage & p_image );
		/**
		 *\brief		downloads the image pixels in synchronous mode.
		 */
		void doDownloadSync( castor3d::TextureStorage & p_storage, castor3d::TextureImage & p_image );
		/**
		 *\brief		Uploads the image pixels to the given storage (PBO or direct texture).
		 */
		void doUploadImage( castor3d::TextureStorage & p_storage, castor3d::TextureImage const & p_image, uint8_t const * p_buffer );

	private:
		//! The parent storage.
		castor3d::TextureStorage & m_storage;
		//! The pixel transfer buffer, used to upload the pixels.
		GlUploadPixelBufferUPtr m_uploadBuffer;
		//! The pixel transfer buffer, used to download the pixels.
		GlDownloadPixelBufferUPtr m_downloadBuffer;
	};
}

#endif
