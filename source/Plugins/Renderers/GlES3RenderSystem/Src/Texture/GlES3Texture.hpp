/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_TEXTURE_H___
#define ___C3DGLES3_TEXTURE_H___

#include "Common/GlES3Object.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlES3Render
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class GlES3Texture
		: private Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
		, public Castor3D::TextureLayout
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 */
		GlES3Texture(
			OpenGlES3 & p_gl,
			GlES3RenderSystem & p_renderSystem,
			Castor3D::TextureType p_type,
			Castor3D::AccessTypes const & p_cpuAccess,
			Castor3D::AccessTypes const & p_gpuAccess );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		GlES3Texture(
			OpenGlES3 & p_gl,
			GlES3RenderSystem & p_renderSystem,
			Castor3D::TextureType p_type,
			Castor3D::AccessTypes const & p_cpuAccess,
			Castor3D::AccessTypes const & p_gpuAccess,
			Castor::PixelFormat p_format,
			Castor::Size const & p_size );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		GlES3Texture(
			OpenGlES3 & p_gl,
			GlES3RenderSystem & p_renderSystem,
			Castor3D::TextureType p_type,
			Castor3D::AccessTypes const & p_cpuAccess,
			Castor3D::AccessTypes const & p_gpuAccess,
			Castor::PixelFormat p_format,
			Castor::Point3ui const & p_size );
		/**
		 *\brief		Destructor.
		 */
		~GlES3Texture();
		/**
		*\brief		Forces mipmaps generation.
		*/
		void GenerateMipmaps()const;
		/**
		*\return	The OpenGL texture dimension.
		*/
		GlES3TexDim GetGlES3Dimension()const
		{
			return m_glDimension;
		}

	private:
		/**
		 *\copydoc		Castor3D::TextureLayout::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::TextureLayout::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::TextureLayout::DoBind
		 */
		void DoBind( uint32_t p_index )const override;
		/**
		 *\copydoc		Castor3D::TextureLayout::DoUnbind
		 */
		void DoUnbind( uint32_t p_index )const override;

	public:
		using ObjectType::IsValid;
		using ObjectType::GetGlES3Name;
		using ObjectType::GetOpenGlES3;

	private:
		//! The RenderSystem.
		GlES3RenderSystem * m_glRenderSystem;
		//! The OpenGL texture dimension.
		GlES3TexDim m_glDimension;
	};
}

#endif
