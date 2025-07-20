#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Image/Image.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/DotExport.hpp>

CU_ImplementExportedOwnedBy( RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( RenderDevice, RenderDevice )

namespace c3d
{
	//*********************************************************************************************

	DeviceCounts::DeviceCounts( RenderDevice const & device )
	{
		bufferAllocated = device.bufferPool->getAllocationStats();
		vertexAllocated = device.vertexPools->getAllocationStats();
		indexAllocated = device.indexPools->getAllocationStats();
		geometryAllocated = device.geometryPools->getAllocationStats();
		uboAllocated = device.uboPool->getAllocationStats();
		uboAllocations = device.uboPool->listAllocations();
	}

	//*********************************************************************************************

	String getName( Corner value )
	{
		switch ( value )
		{
		case Corner::eFarLeftBottom:
			return cuT( "far_left_bottom" );
		case Corner::eFarLeftTop:
			return cuT( "far_left_top" );
		case Corner::eFarRightTop:
			return cuT( "far_right_top" );
		case Corner::eFarRightBottom:
			return cuT( "far_right_bottom" );
		case Corner::eNearLeftBottom:
			return cuT( "near_left_bottom" );
		case Corner::eNearLeftTop:
			return cuT( "near_left_top" );
		case Corner::eNearRightTop:
			return cuT( "near_right_top" );
		case Corner::eNearRightBottom:
			return cuT( "near_right_bottom" );
		default:
			CU_Failure( "Unsupported Corner" );
			return cuEmptyString;
		}
	}

	String getName( UpscalingPerfQualityMode value )
	{
		switch ( value )
		{
		case UpscalingPerfQualityMode::eMaxPerf:
			return cuT( "max_performance" );
		case UpscalingPerfQualityMode::eBalanced:
			return cuT( "balanced" );
		case UpscalingPerfQualityMode::eMaxQuality:
			return cuT( "max_quality" );
		case UpscalingPerfQualityMode::eUltraPerformance:
			return cuT( "ultra_performance" );
		case UpscalingPerfQualityMode::eUltraQuality:
			return cuT( "ultra_quality" );
		case UpscalingPerfQualityMode::eDLAA:
			return cuT( "dlaa" );
		default:
			CU_Failure( "Unsupported UpscalingPerfQualityMode" );
			return cuEmptyString;
		}
	}

	String getName( UpscalingRenderPreset v )
	{
		switch ( v )
		{
		case UpscalingRenderPreset::eDefault:
			return cuT( "default" );
		case UpscalingRenderPreset::eA:
			return cuT( "preset_a" );
		case UpscalingRenderPreset::eB:
			return cuT( "preset_b" );
		case UpscalingRenderPreset::eC:
			return cuT( "preset_c" );
		case UpscalingRenderPreset::eD:
			return cuT( "preset_d" );
		case UpscalingRenderPreset::eE:
			return cuT( "preset_e" );
		case UpscalingRenderPreset::eF:
			return cuT( "preset_f" );
		case UpscalingRenderPreset::eJ:
			return cuT( "preset_j" );
		case UpscalingRenderPreset::eK:
			return cuT( "preset_k" );
		default:
			CU_Failure( "Unsupported UpscalingRenderPreset" );
			return cuEmptyString;
		}
	}

	String getName( FrustumPlane value )
	{
		switch ( value )
		{
		case FrustumPlane::eNear:
			return cuT( "near" );
		case FrustumPlane::eFar:
			return cuT( "far" );
		case FrustumPlane::eLeft:
			return cuT( "left" );
		case FrustumPlane::eRight:
			return cuT( "right" );
		case FrustumPlane::eTop:
			return cuT( "top" );
		case FrustumPlane::eBottom:
			return cuT( "bottom" );
		default:
			CU_Failure( "Unsupported FrustumPlane" );
			return cuEmptyString;
		}
	}

	String getName( PickNodeType value )
	{
		switch ( value )
		{
		case PickNodeType::eNone:
			return cuT( "none" );
		case PickNodeType::eSubmesh:
			return cuT( "submesh" );
		case PickNodeType::eBillboard:
			return cuT( "billboard" );
		default:
			CU_Failure( "Unsupported PickNodeType" );
			return cuEmptyString;
		}
	}

	String getName( TargetType value )
	{
		switch ( value )
		{
		case TargetType::eWindow:
			return cuT( "window" );
		case TargetType::eTexture:
			return cuT( "texture" );
		default:
			CU_Failure( "Unsupported TargetType" );
			return cuEmptyString;
		}
	}

	String getName( ViewportType value )
	{
		switch ( value )
		{
		case ViewportType::eUndefined:
			return cuT( "undefined" );
		case ViewportType::eOrtho:
			return cuT( "ortho" );
		case ViewportType::ePerspective:
			return cuT( "perspective" );
		case ViewportType::eInfinitePerspective:
			return cuT( "infinite_perspective" );
		case ViewportType::eFrustum:
			return cuT( "frustum" );
		default:
			CU_Failure( "Unsupported ViewportType" );
			return cuEmptyString;
		}
	}

	String getName( RenderFilter value )
	{
		switch ( value )
		{
		case RenderFilter::eNone:
			return cuT( "none" );
		case RenderFilter::eAlphaBlend:
			return cuT( "alpha_blend" );
		case RenderFilter::eAlphaTest:
			return cuT( "alpha_test" );
		case RenderFilter::eTransmission:
			return cuT( "transmission" );
		case RenderFilter::eOpaque:
			return cuT( "opaque" );
		default:
			CU_Failure( "Unsupported RenderFilter" );
			return cuEmptyString;
		}
	}

	ashes::Image makeImage( ashes::Device const & device
		, VkImage image
		, crg::ImageId data )
	{
		return ashes::Image{ device, image
			, ashes::ImageCreateInfo{ convert( data.data->info ) } };
	}

	ashes::ImageView makeImageView( ashes::Image const & image
		, VkImageView view
		, crg::ImageViewId data )
	{
		return ashes::ImageView{ convert( data.data->info ), view, &image };
	}

	ashes::ImageView makeTargetImageView( Texture const & texture )
	{
		return makeImageView( *texture.image
			, texture.targetView
			, texture.targetViewId );
	}

	ashes::ImageView makeSampledImageView( Texture const & texture )
	{
		return makeImageView( *texture.image
			, texture.sampledView
			, texture.sampledViewId );
	}

	ashes::ImageView makeWholeImageView( Texture const & texture )
	{
		return makeImageView( *texture.image
			, texture.wholeView
			, texture.wholeViewId );
	}

	//*********************************************************************************************

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout srcLayout
		, ImageLayout dstLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, getAccessMask( srcLayout )
			, getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, srcAccessFlags
			, getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, AccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeVkStruct< VkImageMemoryBarrier >( getAccessFlags( srcAccessFlags )
			, getAccessFlags( dstAccessMask )
			, convert( srcLayout )
			, convert( dstLayout )
			, srcQueueFamily
			, dstQueueFamily
			, image
			, convert( range ) );
	}

	void memoryBarrier( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, ashes::BufferBase const & buffer
		, crg::BufferSubresourceRange const & range
		, AccessState after
		, AccessState before )
	{
		buffer.makeMemoryTransitionBarrier( getAccessFlags( before.access )
			, getPipelineStageFlags( before.pipelineStage )
			, VK_QUEUE_FAMILY_IGNORED
			, VK_QUEUE_FAMILY_IGNORED );
		context.memoryBarrier( commandBuffer
			, buffer
			, range
			, after.access
			, after.pipelineStage
			, before );
	}

	//*************************************************************************************************

	uint32_t getSafeBandsSize( Size const & size )
	{
#if C3D_DebugDisableSafeBands
		return 0u;
#else
		return getNextPowerOfTwo( std::min( size.getWidth()
			, size.getHeight() ) / 10u );
#endif
	}
	
	uint32_t getSafeBandSize( Size const & size )
	{
		return getSafeBandsSize( size ) / 2u;
	}

	Size getSafeBandedSize( Size const & size )
	{
		auto bandsSize = getSafeBandsSize( size );
		return { size.getWidth() + bandsSize
			, size.getHeight() + bandsSize };
	}

	Extent3D getSafeBandedExtent3D( Size const & size )
	{
		return makeExtent3D( getSafeBandedSize( size ) );
	}

	Angle getSafeBandedFovY( Angle const & fovY
		, Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		auto ratio = bandsSize / size.getHeight();
		return fovY + ( fovY * ratio );
	}

	float getSafeBandedAspect( float aspect
		, Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		return float( ( aspect * float( size.getHeight() ) + bandsSize ) / ( float( size.getHeight() ) + bandsSize ) );
	}

	//*********************************************************************************************

	void printGraph( crg::RunnableGraph const & graph )
	{
		auto name = File::normaliseFileName( makeString( graph.getGraph()->getName() ) );
		auto graphsDir = Engine::getEngineDirectory() / cuT( "Graphs" );

		if ( !File::directoryExists( graphsDir ) )
		{
			File::directoryCreate( graphsDir );
		}

		{
			auto path = graphsDir / cuT( "Transitions" );

			if ( !File::directoryExists( path ) )
			{
				File::directoryCreate( path );
			}

			{
				auto streams = crg::dot::displayTransitions( graph, { true, true, true, false } );
				std::ofstream file{ path / ( name + cuT( ".dot" ) ) };
				file << streams.find( MbString{} )->second.str();
			}
			{
				auto streams = crg::dot::displayTransitions( graph, { true, true, false, false } );
				std::ofstream file{ path / ( cuT( "flat_" ) + name + cuT( ".dot" ) ) };
				file << streams.find( MbString{} )->second.str();
			}
			{
				auto streams = crg::dot::displayTransitions( graph, { true, true, true, true } );

				for ( auto const & [str, strm] : streams )
				{
					if ( !str.empty() )
					{
						std::ofstream file{ path / ( name + cuT( "_" ) + makeString( str ) + cuT( ".dot" ) ) };
						file << strm.str();
					}
				}
			}
		}
		{
			auto path = graphsDir / cuT( "Passes" );

			if ( !File::directoryExists( path ) )
			{
				File::directoryCreate( path );
			}

			{
				auto streams = crg::dot::displayPasses( graph, { true, true, true, false } );
				std::ofstream file{ path / ( name + cuT( ".dot" ) ) };
				file << streams.find( MbString{} )->second.str();
			}
			{
				auto streams = crg::dot::displayPasses( graph, { true, true, false, false } );
				std::ofstream file{ path / ( cuT( "flat_" ) + name + cuT( ".dot" ) ) };
				file << streams.find( MbString{} )->second.str();
			}
			{
				auto streams = crg::dot::displayPasses( graph, { true, true, true, true } );

				for ( auto const & [str, strm] : streams )
				{
					if ( !str.empty() )
					{
						std::ofstream file{ path / ( name + cuT( "_" ) + makeString( str ) + cuT( ".dot" ) ) };
						file << strm.str();
					}
				}
			}
		}
	}

	//*********************************************************************************************

	ashes::WriteDescriptorSet makeImageViewDescriptorWrite( VkImageView const & view
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE };
		result.imageInfo.push_back( { VkSampler{}, view, VK_IMAGE_LAYOUT_GENERAL } );
		return result;
	}

	ashes::WriteDescriptorSet makeImageViewDescriptorWrite( VkImageView const & view
		, VkSampler const & sampler
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
		result.imageInfo.push_back( { sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		return result;
	}

	ashes::WriteDescriptorSet makeDescriptorWrite( ashes::ImageView const & view
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE };
		result.imageInfo.push_back( { VkSampler{}, view, VK_IMAGE_LAYOUT_GENERAL } );
		return result;
	}

	ashes::WriteDescriptorSet makeDescriptorWrite( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
		result.imageInfo.push_back( { sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		return result;
	}

	ashes::WriteDescriptorSet makeDescriptorWrite( ashes::UniformBuffer const & buffer
		, uint32_t dstBinding
		, VkDeviceSize elemOffset
		, VkDeviceSize elemRange
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		result.bufferInfo.push_back( { buffer.getBuffer()
			, buffer.getAlignedSize() * elemOffset
			, buffer.getAlignedSize() * elemRange } );
		return result;
	}

	ashes::WriteDescriptorSet makeDescriptorWrite( ashes::BufferBase const & storageBuffer
		, uint32_t dstBinding
		, VkDeviceSize byteOffset
		, VkDeviceSize byteRange
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( { storageBuffer
			, byteOffset
			, byteRange } );
		return result;
	}

	ashes::WriteDescriptorSet makeDescriptorWrite( ashes::BufferBase const & buffer
		, ashes::BufferView const & view
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, ( ( buffer.getUsage() & VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
				? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
				: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ), };
		result.bufferInfo.push_back( { buffer
			, view.getOffset()
			, view.getRange() } );
		result.texelBufferView.push_back( view );
		return result;
	}

	//*********************************************************************************************
}
