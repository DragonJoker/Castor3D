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
		: public c3d::PostEffect
	{
	private:
		struct Configuration
		{
			c3d::Point2f pixelSize;
			float noiseIntensity{ 1.0f };
			float exposure{ 1.0f };
			float time{ 0.0f };
		};

	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
		~PostEffect()override;
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
		/**
		 *\copydoc		c3d::PostEffect::accept
		 */
		void accept( c3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		c3d::PostEffect::setParameters
		 */
		void setParameters( c3d::Parameters parameters )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_pass );
			return *m_pass;
		}

	private:
		/**
		 *\copydoc		c3d::PostEffect::doInitialise
		 */
		bool doInitialise( c3d::RenderDevice const & device
			, c3d::Texture const & source
			, c3d::Texture const & target
			, crg::FramePass const & previousPass )override;
		/**
		 *\copydoc		c3d::PostEffect::doCleanup
		 */
		void doCleanup( c3d::RenderDevice const & device )override;
		/**
		 *\copydoc		c3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( c3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		c3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( c3d::StringStream & file, c3d::String const & tabs ) override;

	public:
		static c3d::String Type;
		static c3d::MbString Name;
		static uint32_t constexpr NoiseMapCount = 6u;

	private:
		c3d::PreciseTimer m_timer;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Milliseconds m_time{ 0ull };
		uint32_t m_timeIndex{ 0u };
		Configuration m_config;
		c3d::UniformBufferOffsetT< Configuration > m_configUbo;
		c3d::Array< c3d::Image, NoiseMapCount > m_noiseImages;
		crg::ImageId m_noiseImg;
		crg::ImageViewId m_noiseView;
		crg::FramePass * m_pass{};
		bool m_firstUpdate{ true };
	};
}

#endif
