/*
See LICENSE file in root folder
*/
#ifndef ___GL_GPU_ONLY_TEXTURE_STORAGE_H___
#define ___GL_GPU_ONLY_TEXTURE_STORAGE_H___

#include "Texture/GlTextureStorage.hpp"

#include "Common/OpenGl.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle GPU only (no CPU access) texture storage.
	*/
	class GlGpuOnlyTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		explicit GlGpuOnlyTextureStorageTraits( castor3d::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlGpuOnlyTextureStorageTraits();
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
	};
}

#endif
