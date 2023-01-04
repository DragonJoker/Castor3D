#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
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

CU_ImplementExportedOwnedBy( castor3d::RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( castor3d::RenderDevice, RenderDevice )

namespace castor3d
{
	castor::String getName( FrustumCorner value )
	{
		switch ( value )
		{
		case FrustumCorner::eFarLeftBottom:
			return cuT( "far_left_bottom" );
		case FrustumCorner::eFarLeftTop:
			return cuT( "far_left_top" );
		case FrustumCorner::eFarRightTop:
			return cuT( "far_right_top" );
		case FrustumCorner::eFarRightBottom:
			return cuT( "far_right_bottom" );
		case FrustumCorner::eNearLeftBottom:
			return cuT( "near_left_bottom" );
		case FrustumCorner::eNearLeftTop:
			return cuT( "near_left_top" );
		case FrustumCorner::eNearRightTop:
			return cuT( "near_right_top" );
		case FrustumCorner::eNearRightBottom:
			return cuT( "near_right_bottom" );
		default:
			CU_Failure( "Unsupported FrustumCorner" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( FrustumPlane value )
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
			return castor::cuEmptyString;
		}
	}

	castor::String getName( PickNodeType value )
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
			return castor::cuEmptyString;
		}
	}

	castor::String getName( TargetType value )
	{
		switch ( value )
		{
		case TargetType::eWindow:
			return cuT( "window" );
		case TargetType::eTexture:
			return cuT( "texture" );
		default:
			CU_Failure( "Unsupported TargetType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( ViewportType value )
	{
		switch ( value )
		{
		case ViewportType::eUndefined:
			return cuT( "undefined" );
		case ViewportType::eOrtho:
			return cuT( "ortho" );
		case ViewportType::ePerspective:
			return cuT( "perspective" );
		case ViewportType::eFrustum:
			return cuT( "frustum" );
		default:
			CU_Failure( "Unsupported ViewportType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( RenderFilter value )
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
			return castor::cuEmptyString;
		}
	}

	ashes::Image makeImage( ashes::Device const & device
		, VkImage image
		, crg::ImageId data )
	{
		return ashes::Image{ device, image, data.data->info };
	}

	ashes::Image makeImage( ashes::Device const & device
		, Texture const & texture )
	{
		return makeImage( device
			, texture.image
			, texture.imageId );
	}

	ashes::ImageView makeImageView( ashes::Device const & device
		, ashes::Image const & image
		, VkImageView view
		, crg::ImageViewId data )
	{
		return ashes::ImageView{ data.data->info, view, &image };
	}

	ashes::ImageView makeTargetImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.targetView
			, texture.targetViewId );
	}

	ashes::ImageView makeSampledImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.sampledView
			, texture.sampledViewId );
	}

	ashes::ImageView makeWholeImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.wholeView
			, texture.wholeViewId );
	}

	//*********************************************************************************************

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, ashes::getAccessMask( srcLayout )
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, srcAccessFlags
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, VkAccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeVkStruct< VkImageMemoryBarrier >( srcAccessFlags
			, dstAccessMask
			, srcLayout
			, dstLayout
			, srcQueueFamily
			, dstQueueFamily
			, image
			, range );
	}

	//*************************************************************************************************

	uint32_t getSafeBandsSize( castor::Size const & size )
	{
#if C3D_DebugDisableSafeBands
		return 0u;
#else
		return castor::getNextPowerOfTwo( std::min( size.getWidth()
			, size.getHeight() ) / 10u );
#endif
	}
	
	uint32_t getSafeBandSize( castor::Size const & size )
	{
		return getSafeBandsSize( size ) / 2u;
	}

	castor::Size getSafeBandedSize( castor::Size const & size )
	{
		auto bandsSize = getSafeBandsSize( size );
		return { size.getWidth() + bandsSize
			, size.getHeight() + bandsSize };
	}

	VkExtent3D getSafeBandedExtent3D( castor::Size const & size )
	{
		return makeExtent3D( getSafeBandedSize( size ) );
	}

	castor::Angle getSafeBandedFovY( castor::Angle const & fovY
		, castor::Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		auto ratio = bandsSize / size.getHeight();
		return fovY + ( fovY * ratio );
	}

	float getSafeBandedAspect( float aspect
		, castor::Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		return float( ( aspect * float( size.getHeight() ) + bandsSize ) / ( float( size.getHeight() ) + bandsSize ) );
	}

	//*********************************************************************************************

	void printGraph( crg::RunnableGraph const & graph )
	{
		auto path = Engine::getEngineDirectory();
		{
			auto streams = crg::dot::displayTransitions( graph, { true, true, true, false } );
			std::ofstream file{ path / ( graph.getGraph()->getName() + "_transitions.dot" ) };
			file << streams.find( std::string{} )->second.str();
		}
		{
			auto streams = crg::dot::displayTransitions( graph, { true, true, true, true } );

			for ( auto & it : streams )
			{
				if ( !it.first.empty() )
				{
					std::ofstream file{ path / ( graph.getGraph()->getName() + "_transitions_" + it.first + ".dot" ) };
					file << it.second.str();
				}
			}
		}
		{
			auto streams = crg::dot::displayPasses( graph, { true, true, true, false } );
			std::ofstream file{ path / ( graph.getGraph()->getName() + "_passes.dot" ) };
			file << streams.find( std::string{} )->second.str();
		}
		{
			auto streams = crg::dot::displayPasses( graph, { true, true, true, true } );

			for ( auto & it : streams )
			{
				if ( !it.first.empty() )
				{
					std::ofstream file{ path / ( graph.getGraph()->getName() + "_passes_" + it.first + ".dot" ) };
					file << it.second.str();
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
