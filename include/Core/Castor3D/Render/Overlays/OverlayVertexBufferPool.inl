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
#include "Castor3D/Shader/Ubos/FontUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace c3d
{
	template< typename VertexT, uint32_t CountT >
	OverlayVertexBufferPoolT< VertexT, CountT >::OverlayVertexBufferPoolT( Engine & engine
		, String const & debugName
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, RenderUbo const & renderUbo
		, ashes::DescriptorSetLayout const & descriptorLayout
		, uint32_t count
		, OverlayTextBufferPoolUPtr textBuf )
		: engine{ engine }
		, device{ device }
		, cameraUbo{ cameraUbo }
		, renderUbo{ renderUbo }
		, descriptorLayout{ descriptorLayout }
		, name{ debugName }
		, overlaysData{ makeBuffer< OverlayUboConfiguration >( device
			, engine.getGraphResourceCache()
			, MaxOverlayPipelines
			, BufferUsageFlags::eStorageBuffer
			, MemoryPropertyFlags::eHostVisible
			, name + cuT( "Data" ) ) }
		, overlaysBuffer{ makeArrayView( overlaysData->lock()
			, overlaysData->getCount() ) }
		, vertexBuffer{ device.renderSystem
			, engine.getGraphResourceCache()
			, BufferUsageFlags::eVertexBuffer | BufferUsageFlags::eStorageBuffer
			, MemoryPropertyFlags::eDeviceLocal
			, name + cuT( "Vertex" )
			, ashes::QueueShare{}
			, MaxOverlayPipelines * sizeof( VertexT ) * CountT }
		, descriptorPool{ descriptorLayout.createPool( 1000u ) }
		, textBuffer{ c3d::move( textBuf ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	OverlayVertexBufferPoolT< VertexT, CountT >::~OverlayVertexBufferPoolT()noexcept
	{
		overlaysData->unlock();
		overlaysData->destroy();
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayVertexBufferPoolT< VertexT, CountT >::clearDrawPipelineData( FontTexture const * fontTexture )
	{
		auto it = m_pipelines.find( fontTexture );

		if ( it != m_pipelines.end() )
		{
			for ( auto & pipelines : it->second )
			{
				m_retired.emplace_back( c3d::move( pipelines.second ) );
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
		auto [it, inserted] = pipelines.try_emplace( &pipeline );

		if ( inserted )
		{
			auto debugName = name + ( fontTexture ? cuT( "-" ) + fontTexture->getFontName() : String{} );
			debugName += cuT( "-" ) + makeString( pipeline.pipeline->getName() );
			auto & pipelineData = it->second;
			pipelineData.overlaysIDsBuffer = makeBuffer< uint32_t >( device
				, engine.getGraphResourceCache()
				, MaxOverlayPipelines
				, BufferUsageFlags::eStorageBuffer
				, MemoryPropertyFlags::eHostVisible
				, debugName + cuT( "-PipelineIDs" ) );
			pipelineData.overlaysIDs = makeArrayView( pipelineData.overlaysIDsBuffer->lock()
				, pipelineData.overlaysIDsBuffer->getCount() );
			pipelineData.indirectCommandsBuffer = makeBuffer< VkDrawIndirectCommand >( device
				, engine.getGraphResourceCache()
				, MaxOverlayPipelines
				, BufferUsageFlags::eIndirectBuffer
				, MemoryPropertyFlags::eHostVisible
				, debugName + cuT( "-IndirectCommands" ) );
			pipelineData.indirectCommands = makeArrayView( pipelineData.indirectCommandsBuffer->lock()
				, pipelineData.indirectCommandsBuffer->getCount() );
			pipelineData.descriptorSets = makeRawUnique< OverlayPipelineData::DescriptorSets >();

			auto & descs = *pipelineData.descriptorSets;
			descs.draw = doCreateDescriptorSet( debugName
				, fontTexture
				, *pipelineData.overlaysIDsBuffer->buffer );
			descs.all.push_back( *descs.draw );
			descs.all.push_back( *c3d::getEngine( device ).getTextureUnitCache().getDescriptorSet() );

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
	bool OverlayVertexBufferPoolT< VertexT, CountT >::fill( Size const & renderSize
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

		for ( auto & pipelineData : m_retired )
		{
			if ( pipelineData.overlaysIDsBuffer )
			{
				pipelineData.overlaysIDsBuffer->unlock();
				pipelineData.overlaysIDsBuffer->destroy();
			}
			if ( pipelineData.indirectCommandsBuffer )
			{
				pipelineData.indirectCommandsBuffer->unlock();
				pipelineData.indirectCommandsBuffer->destroy();
			}
		}
		m_retired.clear();
	}

	template< typename VertexT, uint32_t CountT >
	ashes::DescriptorSetPtr OverlayVertexBufferPoolT< VertexT, CountT >::doCreateDescriptorSet( String debugName
		, FontTexture const * fontTexture
		, ashes::BufferBase const & idsBuffer )const
	{
		if ( textBuffer && fontTexture )
		{
			debugName += fontTexture->getFontName();
		}

		auto result = descriptorPool->createDescriptorSet( toUtf8( debugName ) );
		engine.getMaterialCache().getPassBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		engine.getMaterialCache().getTexConfigBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
		engine.getMaterialCache().getTexAnimBuffer().createBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
		cameraUbo.createSizedBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eCamera ) ) );
		renderUbo.createSizedBinding( *result
			, descriptorLayout.getBinding( uint32_t( OverlayBindingId::eRender ) ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlaysSurfaces ) )
			, vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result->createBinding( descriptorLayout.getBinding( uint32_t( OverlayBindingId::eOverlays ) )
			, overlaysData->getBuffer()
			, 0u
			, uint32_t( overlaysData->getSize() ) );
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
