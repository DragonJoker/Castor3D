/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CombinePass_HPP___
#define ___C3D_CombinePass_HPP___

#include "PassesModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

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
	template< template< typename ValueT > typename WrapperT >
	struct CombinePassConfigT
		: rq::ConfigT< WrapperT >
	{
		WrapperT< TextureLayoutSPtr > resultTexture;
		WrapperT< VkImageLayout > lhsLayout;
		WrapperT< VkImageLayout > rhsLayout;
		WrapperT< IntermediateView > tex3DResult;
	};

	using CombinePassConfig = CombinePassConfigT< ashes::Optional >;
	using CombinePassConfigData = CombinePassConfigT< rq::RawTypeT >;

	class CombinePass
	{
		friend class CombinePassBuilder;

	protected:
		C3D_API CombinePass( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule const & vertexShader
			, ShaderModule const & pixelShader
			, IntermediateViewArray const & lhsViews
			, IntermediateView const & rhsView
			, CombinePassConfig config );

	public:
		C3D_API ~CombinePass();
		C3D_API void update( CpuUpdater & updater );
		C3D_API void accept( PipelineVisitorBase & visitor );
		C3D_API ashes::Semaphore const & combine( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;

		TextureLayout const & getResult()const
		{
			return *m_image;
		}

	public:
		static castor::String const LhsMaps;
		static castor::String const RhsMap;

	private:
		class CombineQuad
			: public RenderQuad
		{
		public:
			CombineQuad( Engine & engine
				, RenderDevice const & device
				, castor::String const & prefix
				, IntermediateViewArray const & lhsViews
				, IntermediateView const & rhsView
				, ShaderModule const & vertexShader
				, ShaderModule const & pixelShader
				, ashes::RenderPass const & renderPass
				, VkExtent2D const & outputSize
				, rq::Config config );
			~CombineQuad();

		private:
			IntermediateViewArray m_lhsViews;
			SamplerSPtr m_lhsSampler;
		};

	private:
		Engine & m_engine;
		ShaderModule const & m_vertexShader;
		ShaderModule const & m_pixelShader;
		TextureLayoutSPtr m_image;
		ashes::ImageView m_view;
		IntermediateViewArray m_lhsBarrierViews;
		IntermediateViewArray m_lhsViews;
		IntermediateView m_rhsBarrierView;
		IntermediateView m_rhsView;
		CombinePassConfig m_config;
		castor::String m_prefix;
		RenderPassTimerSPtr m_timer;
		CommandsSemaphoreArray m_commands;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		CombineQuad m_quad;
		uint32_t m_index{ 0u };
	};

	class CombinePassBuilder
		: public RenderQuadBuilderT< CombinePassConfig, CombinePassBuilder >
	{
	public:
		CombinePassBuilder()
		{
		}

		CombinePassBuilder & resultTexture( TextureLayoutSPtr resultTexture )
		{
			m_config.resultTexture = std::move( resultTexture );
			return *this;
		}

		CombinePassBuilder & lhsViewLayout( VkImageLayout layout )
		{
			m_config.lhsLayout = layout;
			return *this;
		}

		CombinePassBuilder & rhsViewLayout( VkImageLayout layout )
		{
			m_config.rhsLayout = layout;
			return *this;
		}

		CombinePassBuilder & tex3DResult( ashes::ImageView result )
		{
			m_config.tex3DResult = { "Texture3DTo2DResult"
				, result
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			return *this;
		}

		CombinePassUPtr build( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, VkFormat outputFormat
			, VkExtent2D const & outputSize
			, ShaderModule const & vertexShader
			, ShaderModule const & pixelShader
			, IntermediateViewArray const & lhsViews
			, IntermediateView const & rhsView )
		{
			return castor::UniquePtr< CombinePass >( new CombinePass
				{
					engine,
					device,
					prefix,
					outputFormat,
					outputSize,
					vertexShader,
					pixelShader,
					lhsViews,
					rhsView,
					std::move( m_config ),
				} );
		}

	private:
		using RenderQuadBuilderT< CombinePassConfig, CombinePassBuilder >::build;
	};
}

#endif
