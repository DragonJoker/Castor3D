/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GPU_ONLY_TEXTURE_STORAGE_H___
#define ___C3DGLES3_GPU_ONLY_TEXTURE_STORAGE_H___

#include "Texture/GlES3TextureStorage.hpp"

#include "Common/OpenGlES3.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle GPU only (no CPU access) texture storage.
	*/
	class GlES3GpuOnlyTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		GlES3GpuOnlyTextureStorageTraits( Castor3D::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlES3GpuOnlyTextureStorageTraits();
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
	};
}

#endif
