/* See LICENSE file in root folder */
#ifndef ___TRS_TEXTURE_STORAGE_H___
#define ___TRS_TEXTURE_STORAGE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Texture/TextureStorage.hpp>

namespace TestRender
{
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer.
	\remarks	Will use texture buffer objects if available, or pixel buffer objects if not.
	*/
	class TestTextureStorage
		: public castor3d::TextureStorage
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_type			The storage texture type.
		 *\param[in]	p_layout		The parent layout.
		 *\param[in]	p_cpuAccess		The required CPU access.
		 *\param[in]	p_gpuAccess		The required GPU access.
		 */
		TestTextureStorage(
			TestRenderSystem & p_renderSystem,
			castor3d::TextureStorageType p_type,
			castor3d::TextureLayout & p_layout,
			castor3d::AccessTypes const & p_cpuAccess,
			castor3d::AccessTypes const & p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~TestTextureStorage();
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
	};
}

#endif
