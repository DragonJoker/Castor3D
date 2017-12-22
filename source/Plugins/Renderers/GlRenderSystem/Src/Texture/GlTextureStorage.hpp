/*
See LICENSE file in root folder
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
		 *\param[in]	cpuAccess		The required CPU access.
		 *\param[in]	gpuAccess		The required GPU access.
		 */
		GlTextureStorage( OpenGl & p_gl
			, GlRenderSystem & renderSystem
			, castor3d::TextureStorageType p_type
			, castor3d::TextureLayout & p_layout
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess );
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
