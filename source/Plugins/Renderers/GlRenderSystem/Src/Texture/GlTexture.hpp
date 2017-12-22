/*
See LICENSE file in root folder
*/
#ifndef ___GL_TEXTURE_H___
#define ___GL_TEXTURE_H___

#include "Common/GlObject.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class GlTexture
		: private Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
		, public castor3d::TextureLayout
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 */
		GlTexture( OpenGl & p_gl
			, GlRenderSystem & renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 */
		GlTexture( OpenGl & p_gl
			, GlRenderSystem & renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, uint32_t mipmapCount );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		GlTexture( OpenGl & p_gl
			, GlRenderSystem & renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, castor::PixelFormat p_format
			, castor::Size const & p_size );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		GlTexture( OpenGl & p_gl
			, GlRenderSystem & renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, castor::PixelFormat p_format
			, castor::Point3ui const & p_size );
		/**
		 *\brief		Destructor.
		 */
		~GlTexture();
		/**
		*\brief		Forces mipmaps generation.
		*/
		void generateMipmaps()const;
		/**
		*\return	The OpenGL texture dimension.
		*/
		GlTexDim getGlDimension()const
		{
			return m_glDimension;
		}

	private:
		/**
		 *\copydoc		castor3d::TextureLayout::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::TextureLayout::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::TextureLayout::doBind
		 */
		void doBind( uint32_t p_index )const override;
		/**
		 *\copydoc		castor3d::TextureLayout::doUnbind
		 */
		void doUnbind( uint32_t p_index )const override;

	public:
		using ObjectType::isValid;
		using ObjectType::getGlName;
		using ObjectType::getOpenGl;

	private:
		//! The RenderSystem.
		GlRenderSystem * m_glRenderSystem;
		//! The OpenGL texture dimension.
		GlTexDim m_glDimension;
	};
}

#endif
