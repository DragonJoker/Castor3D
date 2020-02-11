/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FilmGrainPostEffect___
#define ___C3D_FilmGrainPostEffect___

#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/ToTexture/RenderQuad.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Shader.hpp>

namespace film_grain
{
	class RenderQuad
		: public castor3d::RenderQuad
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
		explicit RenderQuad( castor3d::RenderSystem & renderSystem
			, VkExtent2D const & size );
		void update( castor::Nanoseconds const & time );

		inline castor3d::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_configUbo;
		}

		inline castor3d::UniformBuffer< Configuration > & getUbo()
		{
			return *m_configUbo;
		}

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	private:
		uint64_t m_time{ 0ull };
		VkExtent2D m_size;
		castor3d::UniformBufferUPtr< Configuration > m_configUbo;
		castor3d::SamplerSPtr m_sampler;
		ashes::ImagePtr m_noise;
		ashes::ImageView m_noiseView;
	};

	class PostEffect
		: public castor3d::PostEffect
	{
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
		void update( castor::Nanoseconds const & elapsedTime )override;
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doInitialise
		 */
		bool doInitialise( castor3d::RenderPassTimer const & timer )override;
		/**
		 *\copydoc		castor3d::PostEffect::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file, castor::String const & tabs ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_sampler;
		castor3d::PostEffectSurface m_surface;
		castor::PreciseTimer m_timer;
		ashes::RenderPassPtr m_renderPass;
		std::unique_ptr< RenderQuad > m_quad;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
