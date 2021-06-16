/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FilmGrainPostEffect___
#define ___C3D_FilmGrainPostEffect___

#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/Passes/RenderQuad.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderAST/Shader.hpp>

#include <array>

namespace film_grain
{
	class PostEffect
		: public castor3d::PostEffect
	{
	private:
		struct Configuration
		{
			castor::Point2f m_pixelSize;
			float m_noiseIntensity;
			float m_exposure;
			float m_time;
		};

	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::update
		 */
		void update( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

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
			, crg::FramePass const & previousPass )override;
		/**
		 *\copydoc		castor3d::PostEffect::doCleanup
		 */
		void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs ) override;

	public:
		static castor::String Type;
		static castor::String Name;
		static uint32_t constexpr NoiseMapCount = 6u;

	private:
		castor3d::SamplerSPtr m_sampler;
		castor::PreciseTimer m_timer;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		uint64_t m_time{ 0ull };
		castor3d::UniformBufferOffsetT< Configuration > m_configUbo;
		std::array< castor::Image, NoiseMapCount > m_noiseImages;
		crg::ImageId m_noiseImg;
		crg::ImageViewId m_noiseView;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::FramePass * m_pass{};
	};
}

#endif
