/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_TEXTURE_STORAGE_H___
#define ___C3DGLES3_TEXTURE_STORAGE_H___

#include <Texture/TextureStorage.hpp>
#include "Common/GlES3Holder.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer.
	\remarks	Will use texture buffer objects if available, or pixel buffer objects if not.
	*/
	template< typename Traits >
	class GlES3TextureStorage
		: public Castor3D::TextureStorage
		, public Holder
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
		GlES3TextureStorage( OpenGlES3 & p_gl
						  , GlES3RenderSystem & p_renderSystem
						  , Castor3D::TextureStorageType p_type
						  , Castor3D::TextureLayout & p_layout
						  , Castor3D::AccessTypes const & p_cpuAccess
						  , Castor3D::AccessTypes const & p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~GlES3TextureStorage();
		/**
		 *\copydoc		Castor3D::TextureStorage::Bind
		 */
		void Bind( uint32_t p_index )const override;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unbind
		 */
		void Unbind( uint32_t p_index )const override;
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		uint8_t * Lock( Castor3D::AccessTypes const & p_lock )override;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unlock
		 */
		void Unlock( bool p_modified )override;
		/**
		 *\copydoc		Castor3D::TextureStorage::Lock
		 */
		uint8_t * Lock( Castor3D::AccessTypes const & p_lock, uint32_t p_index )override;
		/**
		 *\copydoc		Castor3D::TextureStorage::Unlock
		 */
		void Unlock( bool p_modified, uint32_t p_index )override;

		inline GlES3TextureStorageType GetGlES3Type()const
		{
			return m_glType;
		}

		inline GlES3RenderSystem * GetGlES3RenderSystem()const
		{
			return m_glRenderSystem;
		}

	protected:
		//! The RenderSystem.
		GlES3RenderSystem * m_glRenderSystem;
		//! The storage type.
		GlES3TextureStorageType m_glType;
		//! The storage implementation
		Traits m_impl;
	};
}

#include "GlES3TextureStorage.inl"

#endif
