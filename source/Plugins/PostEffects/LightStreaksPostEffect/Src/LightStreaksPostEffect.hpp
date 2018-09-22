/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaksPostEffect___
#define ___C3D_LightStreaksPostEffect___

#include "KawaseUbo.hpp"

#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

#include <GlslShader.hpp>

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
		virtual bool doInitialise( castor3d::RenderPassTimer const & timer );
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		virtual void doCleanup();
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		virtual bool doWriteInto( castor::TextFile & file, castor::String const & tabs );

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
		bool doInitialiseHiPassProgram();
		bool doInitialiseKawaseProgram();
		bool doInitialiseCombineProgram();
		bool doBuildCommandBuffer( castor3d::RenderPassTimer const & timer );

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
				, ashes::Format format
				, ashes::Extent2D const & size
				, ashes::RenderPass const & renderPass );
			castor3d::TextureLayoutSPtr image;
			ashes::FrameBufferPtr frameBuffer;
			std::vector< ashes::DescriptorSetPtr > descriptorSets;
		};

		struct Pipeline
		{
			Layout layout;
			std::vector< Surface > surfaces;
			ashes::PipelinePtr pipeline;
			glsl::Shader vertexShader;
			glsl::Shader pixelShader;
		};

		castor3d::SamplerSPtr m_linearSampler;
		castor3d::SamplerSPtr m_nearestSampler;
		ashes::RenderPassPtr m_renderPass;
		KawaseUbo m_kawaseUbo;
		std::vector< ashes::TextureViewPtr > m_hiPassViews;
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
