/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_TBO_TEXTURE_STORAGE_H___
#define ___C3DGLES3_TBO_TEXTURE_STORAGE_H___

#include "GlES3RenderSystemPrerequisites.hpp"
#include "Buffer/GlES3BufferBase.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using texture buffer objects.
	*/
	class GlES3TboTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		GlES3TboTextureStorageTraits( Castor3D::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlES3TboTextureStorageTraits();
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
		//! The texture buffer, used to hold the texture pixels.
		GlES3BufferBase< uint8_t > m_glBuffer;
		//! The internal pixel format.
		GlES3Internal m_glInternal;
	};
}

#endif
