/*
See LICENSE file in root folder
*/
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
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
			, debugName ) }
		, overlaysBuffer{ castor::makeArrayView( overlaysData->lock( 0u, ashes::WholeSize, 0u )
			, overlaysData->getCount() ) }
		, noTexDeclaration{ noTexDecl }
		, texDeclaration{ texDecl }
		, buffer{ castor::makeUnique< VertexBufferPool >( device, debugName ) }
		, descriptorPool{ descriptorLayout.createPool( 1u ) }
		, descriptorSet{ doCreateDescriptorSet( cameraUbo, descriptorLayout, debugName ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	OverlayVertexBufferIndexT< VertexT, CountT > OverlayVertexBufferPoolT< VertexT, CountT >::allocate( OverlayRenderNode & node )
	{
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, []( ObjectBufferOffset const & lookup )
			{
				return lookup.getBufferChunk( SubmeshFlag::ePositions ).buffer == nullptr;
			} );

		if ( it == allocated.end() )
		{
			allocated.emplace_back( buffer->getBuffer< Quad >( 1u ) );
			it = std::next( allocated.begin(), ptrdiff_t( allocated.size() - 1u ) );
		}

		OverlayVertexBufferIndexT< VertexT, CountT > result{ *this
			, node
			, uint32_t( std::distance( allocated.begin(), it ) ) };
		ObjectBufferOffset & offsets = *it;

		result.geometryBuffers.noTexture.bufferOffset = offsets;
		result.geometryBuffers.noTexture.layouts.emplace_back( noTexDeclaration );

		result.geometryBuffers.textured.bufferOffset = offsets;
		result.geometryBuffers.textured.layouts.emplace_back( texDeclaration );

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::deallocate( OverlayVertexBufferIndexT< VertexT, CountT > const & index )
	{
		CU_Require( &index.pool == this );
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, [&index]( auto const & lookup )
			{
				return lookup.first == index.index;
			} );
		CU_Require( it != allocated.end() );
		buffer->putBuffer( *it );
		*it = {};
		index.geometryBuffers.noTexture.bufferOffset = {};
		index.geometryBuffers.noTexture.layouts.clear();
		index.geometryBuffers.textured.bufferOffset = {};
		index.geometryBuffers.textured.layouts.clear();
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::upload( ashes::CommandBuffer const & cb )
	{
		buffer->upload( cb );
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
