/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CombinePass_HPP___
#define ___C3D_CombinePass_HPP___

#include "PassesModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <ashes/common/Optional.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <memory>
#include <utility>

namespace castor3d
{
	struct CombinePassConfig
		: RenderQuadConfig
	{
		ashes::Optional< TextureLayoutSPtr > resultTexture{ ashes::nullopt };
		ashes::Optional< VkImageLayout > lhsLayout{ ashes::nullopt };
		ashes::Optional< VkImageLayout > rhsLayout{ ashes::nullopt };
	};

	class CombinePass
	{
		friend class CombinePassBuilder;

	protected:
		C3D_API CombinePass( Engine & engine
			, castor::String const & prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule const & vertexShader
			, ShaderModule const & pixelShader
			, ashes::ImageView const & lhsView
			, ashes::ImageView const & rhsView
			, CombinePassConfig config );

	public:
		C3D_API void accept( PipelineVisitorBase & visitor );
		C3D_API ashes::Semaphore const & combine( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;

		inline TextureLayout const & getResult()const
		{
			return *m_image;
		}

	public:
		static castor::String const LhsMap;
		static castor::String const RhsMap;

	private:
		class CombineQuad
			: public RenderQuad
		{
		public:
			explicit CombineQuad( Engine & engine
				, castor::String const & prefix
				, ashes::ImageView const & lhsView
				, RenderQuadConfig const & config );

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet )override;

		private:
			ashes::ImageView m_lhsView;
			SamplerSPtr m_lhsSampler;
		};

	private:
		Engine & m_engine;
		ShaderModule const & m_vertexShader;
		ShaderModule const & m_pixelShader;
		ashes::ImageView const & m_lhsView;
		ashes::ImageView const & m_rhsView;
		CombinePassConfig m_config;
		castor::String m_prefix;
		TextureLayoutSPtr m_image;
		ashes::ImageView m_view;
		RenderPassTimerSPtr m_timer;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		CombineQuad m_quad;
	};

	class CombinePassBuilder
	{
	public:
		inline CombinePassBuilder()
		{
		}

		inline CombinePassBuilder & texcoordConfig( RenderQuadConfig::Texcoord const & config )
		{
			m_config.texcoordConfig = config;
			return *this;
		}

		inline CombinePassBuilder & range( VkImageSubresourceRange const & range )
		{
			m_config.range = range;
			return *this;
		}

		inline CombinePassBuilder & resultTexture( TextureLayoutSPtr resultTexture )
		{
			m_config.resultTexture = std::move( resultTexture );
			return *this;
		}

		inline CombinePassBuilder & lhsViewLayout( VkImageLayout layout )
		{
			m_config.lhsLayout = layout;
			return *this;
		}

		inline CombinePassBuilder & rhsViewLayout( VkImageLayout layout )
		{
			m_config.rhsLayout = layout;
			return *this;
		}

		inline CombinePassUPtr build( Engine & engine
			, castor::String const & prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule const & vertexShader
			, ShaderModule const & pixelShader
			, ashes::ImageView const & lhsView
			, ashes::ImageView const & rhsView )
		{
			return std::unique_ptr< CombinePass >( new CombinePass
				{
					engine,
					prefix,
					outputFormat,
					outputSize,
					vertexShader,
					pixelShader,
					lhsView,
					rhsView,
					std::move( m_config ),
				} );
		}

	private:
		CombinePassConfig m_config;
	};
}

#endif
