/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BloomPostEffect___
#define ___C3D_BloomPostEffect___

#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>

namespace Bloom
{
	static const uint32_t FILTER_COUNT = 4;

	class BloomPostEffect
		: public castor3d::PostEffect
	{
	public:
		BloomPostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		bool initialise()override;
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		void cleanup()override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file )override;

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const CombineMapPasses;
		static castor::String const CombineMapScene;
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		castor3d::SamplerSPtr doCreateSampler( bool linear );
		bool doInitialiseHiPass();
		bool doInitialiseCombine();
		bool doBuildCommandBuffer();

	private:
		castor3d::SamplerSPtr m_linearSampler;
		castor3d::SamplerSPtr m_nearestSampler;
		renderer::RenderPassPtr m_renderPass;

		struct Layout
		{
			renderer::DescriptorSetLayoutPtr descriptorLayout;
			renderer::DescriptorSetPoolPtr descriptorPool;
			renderer::PipelineLayoutPtr pipelineLayout;
		};

		struct Surface
		{
			Surface( renderer::Texture const & texture
				, uint32_t mipLevel
				, renderer::Extent2D const & size
				, renderer::RenderPass const & renderPass );
			renderer::TextureViewPtr view;
			renderer::FrameBufferPtr frameBuffer;
			renderer::DescriptorSetPtr descriptorSet;
		};

		struct Pipeline
		{
			Layout layout;
			renderer::TexturePtr image;
			std::vector< Surface > surfaces;
			renderer::PipelinePtr pipeline;
		};

		struct
		{
			Pipeline hiPass;
			Pipeline combine;
		} m_pipelines;

		renderer::TextureViewPtr m_hiPassMipView;
		std::vector< castor3d::GaussianBlurSPtr > m_blurs;
		renderer::VertexBufferPtr< castor3d::NonTexturedQuad > m_vertexBuffer;
		castor3d::NonTexturedQuad m_buffer;
		uint32_t m_size;
	};
}

#endif
