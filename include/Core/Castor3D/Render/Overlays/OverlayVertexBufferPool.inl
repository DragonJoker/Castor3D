/*
See LICENSE file in root folder
*/
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

namespace castor3d
{
	template< typename VertexT, uint32_t CountT >
	OverlayVertexBufferPoolT< VertexT, CountT >::OverlayVertexBufferPoolT( Engine & engine
		, std::string const & debugName
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, ashes::DescriptorSetLayout const & descriptorLayout
		, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
		, ashes::PipelineVertexInputStateCreateInfo const & texDecl
		, uint32_t count
		, OverlayTextBufferPoolUPtr textBuf )
		: engine{ engine }
		, device{ device }
		, cameraUbo{ cameraUbo }
		, descriptorLayout{ descriptorLayout }
		, name{ debugName }
		, overlaysData{ makeBuffer< OverlayUboConfiguration >( device
			, MaxPipelines
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, name + "Data" ) }
		, overlaysBuffer{ castor::makeArrayView( overlaysData->lock( 0u, ashes::WholeSize, 0u )
			, overlaysData->getCount() ) }
		, noTexDeclaration{ noTexDecl }
		, texDeclaration{ texDecl }
		, vertexBuffer{ device.renderSystem
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name + "Vertex"
			, ashes::QueueShare{}
			, MaxPipelines * sizeof( VertexT ) * CountT }
		, descriptorPool{ descriptorLayout.createPool( 1000u ) }
		, textBuffer{ std::move( textBuf ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	ashes::DescriptorSet const & OverlayVertexBufferPoolT< VertexT, CountT >::getDrawDescriptorSet( FontTexture const * fontTexture )
	{
		auto it = descriptorSets.emplace( fontTexture, nullptr ).first;

		if ( !it->second )
		{
			it->second = doCreateDescriptorSet( name, fontTexture );
		}

		return *it->second;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::fillComputeDescriptorSet( FontTexture const * fontTexture
		, ashes::DescriptorSetLayout const & descLayout
		, ashes::DescriptorSet & descriptorSet )const
	{
		if ( textBuffer && fontTexture )
		{
			textBuffer->fillDescriptorSet( fontTexture
				, descLayout
				, descriptorSet );
		}
	}

	template< typename VertexT, uint32_t CountT >
	OverlayTextBuffer const * OverlayVertexBufferPoolT< VertexT, CountT >::getTextBuffer( FontTexture const & fontTexture )const
	{
		if ( textBuffer )
		{
			return textBuffer->get( fontTexture );
		}

		return nullptr;
	}

	template< typename VertexT, uint32_t CountT >
	template< typename OverlayT >
	OverlayVertexBufferIndexT< VertexT, CountT > OverlayVertexBufferPoolT< VertexT, CountT >::fill( castor::Size const & renderSize
		, OverlayT const & overlay
		, OverlayRenderNode const & node
		, bool secondary
		, FontTexture const * fontTexture )
	{
		OverlayVertexBufferIndexT< VertexT, CountT > result{ *this
			, node
			, index };
		auto count = overlay.getCount( secondary );

		if ( !count
			|| allocated > ( MaxPipelines * CountT - count ) )
		{
			if ( count )
			{
				CU_Failure( ": Couldn't allocate overlay" );
				log::warn << name << ": Couldn't allocate overlay";
			}

			return result;
		}

		auto offset = allocated * sizeof( VertexT );
		result.geometryBuffers.buffer = &vertexBuffer;
		result.geometryBuffers.offset = offset;
		result.geometryBuffers.range = count * sizeof( VertexT );
		result.geometryBuffers.count = count;

		if constexpr ( isText )
		{
			if ( textBuffer && fontTexture )
			{
				result.textBuffer = textBuffer->fill( result.index, fontTexture, renderSize, overlay, node, secondary );
			}
		}

		allocated += count;
		++index;

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::upload( ashes::CommandBuffer const & cb )
	{
		if ( allocated )
		{
			allocated = 0u;
			index = 0u;
		}

		if ( textBuffer )
		{
			textBuffer->upload( cb );
		}
	}

	template< typename VertexT, uint32_t CountT >
	ashes::DescriptorSetPtr OverlayVertexBufferPoolT< VertexT, CountT >::doCreateDescriptorSet( std::string debugName
		, FontTexture const * fontTexture )const
	{
		if ( textBuffer && fontTexture )
		{
			debugName += fontTexture->getFontName();
		}

		auto result = descriptorPool->createDescriptorSet( debugName );
		engine.getMaterialCache().getPassBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		engine.getMaterialCache().getTexConfigBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
		engine.getMaterialCache().getTexAnimBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
		cameraUbo.createSizedBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eCamera ) ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlays ) )
			, *overlaysData
			, 0u
			, uint32_t( overlaysData->getCount() ) );
		result->update();
		return result;
	}
}
