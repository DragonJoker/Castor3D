/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaksPostEffect___
#define ___C3D_LightStreaksPostEffect___

#include "KawaseUbo.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <ShaderAST/Shader.hpp>

namespace light_streaks
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		virtual ~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		virtual bool doInitialise( castor3d::RenderDevice const & device
			, castor3d::RenderPassTimer const & timer )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		virtual void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		virtual bool doWriteInto( castor::TextFile & file, castor::String const & tabs )override;

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const CombineMapScene;
		static castor::String const CombineMapKawase1;
		static castor::String const CombineMapKawase2;
		static castor::String const CombineMapKawase3;
		static castor::String const CombineMapKawase4;
		static constexpr uint32_t Count{ 4u };
		using SurfaceArray = std::array< castor3d::PostEffectSurface, Count >;

	private:
		castor3d::SamplerSPtr doCreateSampler( bool linear );
		bool doInitialiseHiPassProgram( castor3d::RenderDevice const & device );
		bool doInitialiseKawaseProgram( castor3d::RenderDevice const & device );
		bool doInitialiseCombineProgram( castor3d::RenderDevice const & device );
		bool doBuildCommandBuffer( castor3d::RenderDevice const & device
			, castor3d::RenderPassTimer const & timer );

	private:
		struct Layout
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout;
			ashes::DescriptorSetPoolPtr descriptorPool;
			ashes::PipelineLayoutPtr pipelineLayout;
		};

		struct Surface
		{
			Surface( castor3d::RenderSystem & renderSystem
				, castor3d::RenderDevice const & device
				, VkFormat format
				, VkExtent2D const & size
				, ashes::RenderPass const & renderPass
				, castor::String debugName );
			castor3d::TextureLayoutSPtr image;
			ashes::FrameBufferPtr frameBuffer;
			std::vector< ashes::DescriptorSetPtr > descriptorSets;
		};

		struct Pipeline
		{
			castor3d::ShaderModule vertexShader;
			castor3d::ShaderModule pixelShader;
			Layout layout;
			std::vector< Surface > surfaces;
			ashes::GraphicsPipelinePtr pipeline;
		};

		castor3d::SamplerSPtr m_linearSampler;
		castor3d::SamplerSPtr m_nearestSampler;
		ashes::RenderPassPtr m_shaderInputRenderPass;
		ashes::RenderPassPtr m_colorAttachRenderPass;
		KawaseUbo m_kawaseUbo;
		std::vector< ashes::ImageView > m_hiPassViews;
		ashes::VertexBufferPtr< castor3d::NonTexturedQuad > m_vertexBuffer;

		struct
		{
			Pipeline hiPass;
			Pipeline kawase;
			Pipeline combine;
		} m_pipelines;
	};
}

#endif
