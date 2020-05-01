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

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

namespace castor3d
{
	class CombinePass
	{
	public:
		C3D_API CombinePass( Engine & engine
			, castor::String prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule vertexShader
			, ShaderModule pixelShader
			, ashes::ImageView const & lhsView
			, ashes::ImageView const & rhsView
			, TextureLayoutSPtr resultTexture );
		C3D_API CombinePass( Engine & engine
			, castor::String prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule vertexShader
			, ShaderModule pixelShader
			, ashes::ImageView const & lhsView
			, ashes::ImageView const & rhsView );
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
				, ashes::ImageView const & lhsiew );

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet )override;

		private:
			ashes::ImageView m_lhsView;
			SamplerSPtr m_lhsSampler;
		};

	private:
		Engine & m_engine;
		castor::String m_prefix;
		TextureLayoutSPtr m_image;
		ashes::ImageView m_view;
		RenderPassTimerSPtr m_timer;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		CombineQuad m_quad;
	};
}

#endif
