/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GrayScalePostEffect___
#define ___C3D_GrayScalePostEffect___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

namespace grayscale
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect()override;
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;
		/**
		 *\copydoc		castor3d::PostEffect::setParameters
		 */
		void setParameters( castor3d::Parameters parameters )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_pass );
			return *m_pass;
		}

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		crg::ImageViewId const * doInitialise( castor3d::RenderDevice const & device
			, crg::FramePass const & previousPass ) override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup( castor3d::RenderDevice const & device ) override;
		/**
		 *\copydoc		castor3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::UniformBufferOffsetT< castor::Point3f > m_configUbo;
		castor::ChangeTracked< castor::Point3f > m_factors{ castor::Point3f{ 0.2126f, 0.7152f, 0.0722f } };
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::FramePass * m_pass{};
	};
}

#endif
