/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FilmGrainPostEffect___
#define ___C3D_FilmGrainPostEffect___

#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
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
			castor::Point2f pixelSize;
			float noiseIntensity{ 1.0f };
			float exposure{ 1.0f };
			float time{ 0.0f };
		};

	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect()override;
		static castor3d::PostEffectUPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::ConfigurationVisitorBase & visitor )override;
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
		bool doInitialise( castor3d::RenderDevice const & device
			, castor3d::Texture const & source
			, castor3d::Texture const & target
			, crg::FramePass const & previousPass )override;
		/**
		 *\copydoc		castor3d::PostEffect::doCleanup
		 */
		void doCleanup( castor3d::RenderDevice const & device )override;
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
		static castor::MbString Name;
		static uint32_t constexpr NoiseMapCount = 6u;

	private:
		castor::PreciseTimer m_timer;
		castor3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		castor::Milliseconds m_time{ 0ull };
		uint32_t m_timeIndex{ 0u };
		Configuration m_config;
		castor3d::UniformBufferOffsetT< Configuration > m_configUbo;
		castor::Array< castor::Image, NoiseMapCount > m_noiseImages;
		crg::ImageId m_noiseImg;
		crg::ImageViewId m_noiseView;
		crg::FramePass * m_pass{};
		bool m_firstUpdate{ true };
	};
}

#endif
