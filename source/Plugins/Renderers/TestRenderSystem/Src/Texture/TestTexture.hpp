/* See LICENSE file in root folder */
#ifndef ___TRS_TEXTURE_H___
#define ___TRS_TEXTURE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Texture/TextureLayout.hpp>

namespace TestRender
{
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class TestTexture
		: public castor3d::TextureLayout
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 */
		TestTexture( TestRenderSystem & p_renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 */
		TestTexture( TestRenderSystem & p_renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, uint32_t mipmapCount );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		TestTexture( TestRenderSystem & p_renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, castor::PixelFormat p_format
			, castor::Size const & p_size );
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 */
		TestTexture( TestRenderSystem & p_renderSystem
			, castor3d::TextureType p_type
			, castor3d::AccessTypes const & cpuAccess
			, castor3d::AccessTypes const & gpuAccess
			, castor::PixelFormat p_format
			, castor::Point3ui const & p_size );
		/**
		 *\brief		Destructor.
		 */
		~TestTexture();
		/**
		*\brief		Forces mipmaps generation.
		*/
		void generateMipmaps()const;

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
	};
}

#endif
