/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_PBO_TEXTURE_STORAGE_H___
#define ___C3DGLES3_PBO_TEXTURE_STORAGE_H___

#include "GlES3RenderSystemPrerequisites.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using pixel buffer objects.
	*/
	class GlES3PboTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		GlES3PboTextureStorageTraits( Castor3D::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlES3PboTextureStorageTraits();
		/**
		 *\copydoc		Castor3D::TextureStorage::Bind
		 */
		void Bind( Castor3D::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unbind
		 */
		void Unbind( Castor3D::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		uint8_t * Lock( Castor3D::TextureStorage & p_storage, Castor3D::AccessTypes const & p_lock, uint32_t p_index );
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
		GlES3UploadPixelBufferUPtr m_uploadBuffer;
		//! The pixel transfer buffer, used to download the pixels.
		GlES3DownloadPixelBufferUPtr m_downloadBuffer;
	};
}

#endif
