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
		, uint32_t count )
		: engine{ engine }
		, device{ device }
		, overlaysData{ makeBuffer< OverlayUboConfiguration >( device
			, MaxPipelines
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, debugName + "Data" ) }
		, overlaysBuffer{ castor::makeArrayView( overlaysData->lock( 0u, ashes::WholeSize, 0u )
			, overlaysData->getCount() ) }
		, noTexDeclaration{ noTexDecl }
		, texDeclaration{ texDecl }
		, vertexBuffer{ device.renderSystem
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, debugName + "Vertex"
			, ashes::QueueShare{}
			, MaxPipelines * sizeof( VertexT ) * CountT }
		, descriptorPool{ descriptorLayout.createPool( 1u ) }
		, descriptorSet{ doCreateDescriptorSet( cameraUbo, descriptorLayout, debugName ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	template< typename OverlayT >
	OverlayVertexBufferIndexT< VertexT, CountT > OverlayVertexBufferPoolT< VertexT, CountT >::fill( castor::Size const & renderSize
		, OverlayT const & overlay
		, OverlayRenderNode & node
		, bool secondary )
	{
		OverlayVertexBufferIndexT< VertexT, CountT > result{ *this
			, node
			, index };

		if ( allocated <= ( MaxPipelines * ( CountT - 1u ) ) )
		{
			auto offset = allocated * sizeof( VertexT );
			auto count = overlay.fillBuffer( renderSize
				, &vertexBuffer.template getData< VertexT >( offset )
				, secondary );

			if ( count )
			{
				result.geometryBuffers.buffer = &vertexBuffer;
				result.geometryBuffers.offset = offset;
				result.geometryBuffers.range = count * sizeof( VertexT );
				result.geometryBuffers.count = count;
				allocated += count;
				++index;
			}
		}

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::upload( ashes::CommandBuffer const & cb )
	{
		if ( allocated )
		{
			if constexpr ( CountT != 6u && CountT != 48u )
			{
				vertexBuffer.markDirty( 0u
					, allocated * sizeof( VertexT )
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
				vertexBuffer.upload( cb );
			}

			allocated = 0u;
			index = 0u;
		}
	}

	template< typename VertexT, uint32_t CountT >
	ashes::DescriptorSetPtr OverlayVertexBufferPoolT< VertexT, CountT >::doCreateDescriptorSet( CameraUbo const & cameraUbo
		, ashes::DescriptorSetLayout const & descriptorLayout
		, std::string const & debugName )const
	{
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
