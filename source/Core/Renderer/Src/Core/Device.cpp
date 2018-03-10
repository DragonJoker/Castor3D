/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/Device.hpp"

#include "Buffer/Buffer.hpp"
#include "Core/Renderer.hpp"
#include "Core/SwapChain.hpp"
#include "Image/Sampler.hpp"
#include "Miscellaneous/MemoryRequirements.hpp"
#include "Pipeline/PipelineLayout.hpp"
#include "Pipeline/VertexInputState.hpp"
#include "RenderPass/RenderPass.hpp"
#include "RenderPass/RenderPassCreateInfo.hpp"
#include "RenderPass/RenderSubpass.hpp"
#include "RenderPass/RenderSubpassState.hpp"

namespace renderer
{
	Device::Device( Renderer const & renderer
		, PhysicalDevice const & gpu
		, Connection const & connection )
		: m_renderer{ renderer }
		, m_gpu{ gpu }
	{
	}

	void Device::enable()const
	{
		doEnable();
		onEnabled( *this );
	}

	void Device::disable()const
	{
		onDisabled( *this );
		doDisable();
	}

	Mat4 Device::infinitePerspective( Angle fovy
		, float aspect
		, float zNear )const
	{
		float const range = tan( float( fovy ) / float( 2 ) ) * zNear;
		float const left = -range * aspect;
		float const right = range * aspect;
		float const bottom = -range;
		float const top = range;

		Mat4 result{ float{ 0 } };
		result[0][0] = ( float( 2 ) * zNear ) / ( right - left );
		result[1][1] = ( float( 2 ) * zNear ) / ( top - bottom );
		result[2][2] = -float( 1 );
		result[2][3] = -float( 1 );
		result[3][2] = -float( 2 ) * zNear;
		return result;
	}

	BufferBasePtr Device::createBuffer( uint32_t size
		, BufferTargets target
		, MemoryPropertyFlags flags )const
	{
		auto result = createBuffer( size, target );
		result->bindMemory( allocateMemory( result->getMemoryRequirements(), flags ) );
		return result;
	}

	TexturePtr Device::createTexture( ImageCreateInfo const & createInfo
		, MemoryPropertyFlags flags )const
	{
		auto result = createTexture( createInfo );
		result->bindMemory( allocateMemory( result->getMemoryRequirements(), flags ) );
		return result;
	}

	RenderPassPtr Device::createRenderPass( AttachmentDescriptionArray const & attaches
		, RenderSubpassPtrArray && subpasses
		, RenderSubpassState const & initialState
		, RenderSubpassState const & finalState )const
	{
		RenderPassCreateInfo createInfo;
		createInfo.attachments = attaches;
		createInfo.subpasses.reserve( subpasses.size() );
		createInfo.dependencies.reserve( 1 + subpasses.size() );
		auto pipelineStage = initialState.pipelineStage;
		auto access = initialState.access;
		uint32_t subpassIndex = ExternalSubpass;

		for ( auto const & subpass : subpasses )
		{
			auto const & state = subpass->getNeededState();

			if ( pipelineStage != state.pipelineStage
				|| access != state.access )
			{
				createInfo.dependencies.push_back(
				{
					subpassIndex,                                    // srcSubpass
					subpassIndex + 1,                                // dstSubpass
					pipelineStage,                                   // srcStageMask
					state.pipelineStage,                             // dstStageMask
					access,                                          // srcAccessMask
					state.access,                                    // dstAccessMask
					DependencyFlag::eByRegion                        // dependencyFlags
				} );
			}

			pipelineStage = state.pipelineStage;
			access = state.access;
			++subpassIndex;
			createInfo.subpasses.push_back( subpass->getDescription() );
		}

		if ( pipelineStage != finalState.pipelineStage
			|| access != finalState.access )
		{
			createInfo.dependencies.push_back(
			{
				subpassIndex,                                        // srcSubpass
				ExternalSubpass,                                     // dstSubpass
				pipelineStage,                                       // srcStageMask
				finalState.pipelineStage,                            // dstStageMask
				access,                                              // srcAccessMask
				finalState.access,                                   // dstAccessMask
				DependencyFlag::eByRegion                            // dependencyFlags
			} );
		}

		return createRenderPass( createInfo );
	}

	ClipDirection Device::getClipDirection()const
	{
		return m_renderer.getClipDirection();
	}

	PipelineLayoutPtr Device::createPipelineLayout()const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayout const & layout )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{ layout }
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( PushConstantRange const & pushConstantRange )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{ pushConstantRange } );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayout const & layout
		, PushConstantRange const & pushConstantRange )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{ layout }
			, PushConstantRangeCRefArray{ pushConstantRange } );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayoutCRefArray const & layouts )const
	{
		return createPipelineLayout( layouts
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( PushConstantRangeCRefArray const & pushConstantRanges )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{ pushConstantRanges } );
	}

	SamplerPtr Device::createSampler( WrapMode wrapS
		, WrapMode wrapT
		, WrapMode wrapR
		, Filter minFilter
		, Filter magFilter
		, MipmapMode mipFilter
		, float minLod
		, float maxLod
		, float lodBias
		, BorderColour borderColour
		, float maxAnisotropy
		, CompareOp compareOp )const
	{
		return createSampler( 
		{
			minFilter,
			magFilter,
			mipFilter,
			wrapR,
			wrapT,
			wrapS,
			lodBias,
			maxAnisotropy != 1.0f,
			maxAnisotropy,
			compareOp != CompareOp::eAlways,
			compareOp,
			minLod,
			maxLod,
			borderColour,
			false
		} );
	}
}
