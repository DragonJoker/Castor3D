/*
See LICENSE file in root folder
*/
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace castor3d
{
	template< typename VertexT, uint32_t CountT >
	OverlayVertexBufferPoolT< VertexT, CountT >::OverlayVertexBufferPoolT( Engine & engine
		, castor::String const & debugName
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, HdrConfigUbo const & hdrConfigUbo
		, ashes::DescriptorSetLayout const & descriptorLayout
		, uint32_t count
		, OverlayTextBufferPoolUPtr textBuf )
		: engine{ engine }
		, device{ device }
		, cameraUbo{ cameraUbo }
		, hdrConfigUbo{ hdrConfigUbo }
		, descriptorLayout{ descriptorLayout }
		, name{ debugName }
		, overlaysData{ makeBuffer< OverlayUboConfiguration >( device
			, MaxOverlayPipelines
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, name + cuT( "Data" ) ) }
		, overlaysBuffer{ castor::makeArrayView( overlaysData->lock( 0u, ashes::WholeSize, 0u )
			, overlaysData->getCount() ) }
		, vertexBuffer{ device.renderSystem
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name + cuT( "Vertex" )
			, ashes::QueueShare{}
			, MaxOverlayPipelines * sizeof( VertexT ) * CountT }
		, descriptorPool{ descriptorLayout.createPool( 1000u ) }
		, textBuffer{ castor::move( textBuf ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::clearDrawPipelineData( FontTexture const * fontTexture )
	{
		auto it = m_pipelines.find( fontTexture );

		if ( it != m_pipelines.end() )
		{
			for ( auto & pipelines : it->second )
			{
				m_retired.emplace_back( castor::move( pipelines.second ) );
			}

			it->second.clear();
		}
	}

	template< typename VertexT, uint32_t CountT >
	OverlayPipelineData & OverlayVertexBufferPoolT< VertexT, CountT >::getDrawPipelineData( OverlayDrawPipeline const & pipeline
		, FontTexture const * fontTexture
		, ashes::DescriptorSet const * textDescriptorSet )
	{
		auto & pipelines = m_pipelines.emplace( fontTexture, PipelineDataMap{} ).first->second;
		auto [it, res] = pipelines.try_emplace( &pipeline );
		auto debugName = name + ( fontTexture ? cuT( "-" ) + fontTexture->getFontName() : castor::String{} );
		debugName += cuT( "-" ) + castor::makeString( pipeline.pipeline->getName() );

		if ( res )
		{
			auto & pipelineData = it->second;
			pipelineData.overlaysIDsBuffer = makeBuffer< uint32_t >( device
				, MaxOverlayPipelines
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, debugName + cuT( "-PipelineIDs" ) );
			pipelineData.overlaysIDs = castor::makeArrayView( pipelineData.overlaysIDsBuffer->lock( 0u, ashes::WholeSize, 0u )
				, pipelineData.overlaysIDsBuffer->getCount() );
			pipelineData.indirectCommandsBuffer = makeBuffer< VkDrawIndirectCommand >( device
				, MaxOverlayPipelines
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, debugName + cuT( "-IndirectCommands" ) );
			pipelineData.indirectCommands = castor::makeArrayView( pipelineData.indirectCommandsBuffer->lock( 0u, ashes::WholeSize, 0u )
				, pipelineData.indirectCommandsBuffer->getCount() );
			pipelineData.descriptorSets = castor::make_unique< OverlayPipelineData::DescriptorSets >();

			auto & descs = *pipelineData.descriptorSets;
			descs.draw = doCreateDescriptorSet( debugName
				, fontTexture
				, pipelineData.overlaysIDsBuffer->getBuffer() );
			descs.all.push_back( *descs.draw );
			descs.all.push_back( *device.renderSystem.getEngine()->getTextureUnitCache().getDescriptorSet() );

			if ( textDescriptorSet )
			{
				descs.all.push_back( *textDescriptorSet );
			}
		}

		return it->second;
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
	bool OverlayVertexBufferPoolT< VertexT, CountT >::fill( castor::Size const & renderSize
		, OverlayT const & overlay
		, OverlayDrawData & data
		, bool secondary
		, FontTexture const * fontTexture )noexcept
	{
		auto & pipelines = m_pipelines.emplace( fontTexture, PipelineDataMap{} ).first->second;
		auto it = pipelines.find( &data.node->pipeline );

		if ( it == pipelines.end() )
		{
			log::error << name << ": Overlay render node not found" << std::endl;
			CU_Failure( "Overlay render node not found" );
			return false;
		}

		auto & pipelineData = it->second;
		auto pipelineIndex = pipelineData.count;
		++pipelineData.count;
		auto count = overlay.getCount( secondary );

		if ( !count
			|| allocated > ( MaxOverlayPipelines * CountT - count ) )
		{
			if ( count )
			{
				log::error << name << ": Couldn't allocate overlay" << std::endl;
				CU_Failure( ": Couldn't allocate overlay" );
			}

			return false;
		}

		data.overlayIndex = index;
		data.pipelineIndex = pipelineIndex;
		data.pipelineData = &pipelineData;
		data.indirectData = &pipelineData.indirectCommands[pipelineIndex];

		pipelineData.overlaysIDs[pipelineIndex] = data.overlayIndex;
		data.indirectData->firstVertex = allocated;
		data.indirectData->vertexCount = count;
		data.indirectData->firstInstance = 0u;
		data.indirectData->instanceCount = 1u;

		if constexpr ( isText )
		{
			if ( textBuffer && fontTexture )
			{
				data.textBuffer = textBuffer->fill( data.overlayIndex, fontTexture, overlay );
			}
		}

		allocated += count;
		++index;

		return true;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::upload( UploadData & uploader )
	{
		if ( allocated )
		{
			for ( auto & pipelines : m_pipelines )
			{
				for ( auto & it : pipelines.second )
				{
					it.second.count = 0u;
				}
			}

			allocated = 0u;
			index = 0u;
		}

		if ( textBuffer )
		{
			textBuffer->upload( uploader );
		}

		m_retired.clear();
	}

	template< typename VertexT, uint32_t CountT >
	ashes::DescriptorSetPtr OverlayVertexBufferPoolT< VertexT, CountT >::doCreateDescriptorSet( castor::String debugName
		, FontTexture const * fontTexture
		, ashes::BufferBase const & idsBuffer )const
	{
		if ( textBuffer && fontTexture )
		{
			debugName += fontTexture->getFontName();
		}

		auto result = descriptorPool->createDescriptorSet( castor::toUtf8( debugName ) );
		engine.getMaterialCache().getPassBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		engine.getMaterialCache().getTexConfigBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
		engine.getMaterialCache().getTexAnimBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
		cameraUbo.createSizedBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eCamera ) ) );
		hdrConfigUbo.createSizedBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eHdrConfig ) ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlaysSurfaces ) )
			, vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlays ) )
			, *overlaysData
			, 0u
			, uint32_t( overlaysData->getCount() ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlaysIDs ) )
			, idsBuffer
			, 0u
			, uint32_t( idsBuffer.getSize() ) );

		if ( fontTexture && textBuffer )
		{
			fontTexture->getFontUbo().createSizedBinding( *result
				, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlaysFont ) ) );
		}

		result->update();
		return result;
	}
}
