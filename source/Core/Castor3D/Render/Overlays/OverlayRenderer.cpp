/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOverlaySurface.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/RecordContext.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, OverlayRenderer )

#pragma GCC diagnostic ignored "-Wclass-memaccess"

namespace castor3d
{
	//*********************************************************************************************

	namespace ovrlrend
	{
		enum class OverlayTexture : uint32_t
		{
			eNone = 0x00,
			eText = 0x01,
			eColour = 0x02,
			eOpacity = 0x04,
		};

		struct OverlayBlendComponents
			: public shader::BlendComponents
		{
			OverlayBlendComponents( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: shader::BlendComponents{ writer
					, std::move( expr )
					, enabled }
			{
			}

			OverlayBlendComponents( shader::Materials const & materials )
				: OverlayBlendComponents{ *materials.getWriter()
					, makeInit( *materials.getWriter(), materials )
					, true }
			{
			}

			template< ast::var::Flag FlagT >
			OverlayBlendComponents( shader::Materials const & materials
				, shader::Material const & material
				, shader::OverlaySurfaceT< FlagT > const & surface )
				: OverlayBlendComponents{ *materials.getWriter()
					, makeInit( *materials.getWriter(), materials, material, surface )
					, true }
			{
			}

			SDW_DeclStructInstance( , OverlayBlendComponents );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, sdw::expr::ExprList & inits )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_OverlayBlendComponents" );
				shader::BlendComponents::fillType( *result, materials, inits );
				return result;
			}

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials )
			{
				sdw::expr::ExprList inits;
				return makeType( cache, materials, inits );
			}

			template< ast::var::Flag FlagT >
			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, shader::Material const & material
				, shader::OverlaySurfaceT< FlagT > const & surface
				, sdw::expr::ExprList & inits )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_OverlayBlendComponents" );
				shader::BlendComponents::fillType( *result, materials, material, surface, nullptr, inits );
				return result;
			}

			template< ast::var::Flag FlagT >
			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, shader::Materials const & materials
				, shader::Material const & material
				, shader::OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList inits;
				return makeType( cache, materials, material, surface, inits );
			}

		private:
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter & writer
				, shader::Materials const & materials )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, initializers );
				return sdw::makeAggrInit( type, std::move( initializers ) );
			}

			template< ast::var::Flag FlagT >
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter & writer
				, shader::Materials const & materials
				, shader::Material const & material
				, shader::OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, material, surface, initializers );
				return sdw::makeAggrInit( type, std::move( initializers ) );
			}
		};

		static uint32_t makeKey( PassComponentRegister const & passComponents
			, TextureCombine const & textures
			, bool text )
		{
			auto tex{ uint32_t( text ? OverlayTexture::eText : OverlayTexture::eNone ) };

			if ( hasAny( textures, passComponents.getColourMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eColour );
			}

			if ( hasAny( textures, passComponents.getOpacityMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eOpacity );
			}

			uint32_t result{ tex << 8 };
			result |= uint32_t( textures.configCount );
			return result;
		}

		static void memoryBarrier( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, ashes::BufferBase const & buffer
			, crg::BufferSubresourceRange const & range
			, crg::AccessState after
			, crg::AccessState before )
		{
			buffer.makeMemoryTransitionBarrier( before.access
				, before.pipelineStage
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED );
			context.memoryBarrier( commandBuffer
				, buffer
				, range
				, after.access
				, after.pipelineStage
				, before );
		}

		static void registerComputeBufferCommands( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, OverlayRenderer::TextPipeline const & pipeline
			, OverlayRenderer::TextPipelineSet const & set )
		{
			auto & textBuffer = set.textBuffer;
			crg::BufferSubresourceRange range{ 0u, VK_WHOLE_SIZE };

			memoryBarrier( context
				, commandBuffer
				, textBuffer->charsBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			memoryBarrier( context
				, commandBuffer
				, textBuffer->wordsBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			memoryBarrier( context
				, commandBuffer
				, textBuffer->linesBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline.pipeline );
			VkDescriptorSet descriptorSet = *set.descriptorSet;
			context.getContext().vkCmdBindDescriptorSets( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline.pipelineLayout
				, 0u
				, 1u
				, &descriptorSet
				, 0u
				, nullptr );
			uint32_t batchCountX = 32u;
			uint32_t batchCountY = 16u;
			uint32_t batchCount = batchCountX * batchCountY;
			std::array< uint32_t, 2u > data{ 0u, set.count };

			while ( data[0] < data[1] )
			{
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipeline.pipelineLayout
					, VK_SHADER_STAGE_COMPUTE_BIT
					, 0u
					, sizeof( uint32_t ) * 2u
					, data.data() );
				context.getContext().vkCmdDispatch( commandBuffer
					, batchCountX, batchCountY, 1u );
				data[0] += batchCount;
			}

			memoryBarrier( context
				, commandBuffer
				, textBuffer->linesBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
			memoryBarrier( context
				, commandBuffer
				, textBuffer->wordsBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
			memoryBarrier( context
				, commandBuffer
				, textBuffer->charsBuffer.buffer->getBuffer()
				, range
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
		}

		template< typename VertexBufferT >
		static void registerComputeBufferCommands( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, OverlayRenderer::ComputePipeline const & pipeline
			, VertexBufferT const & vertexBuffer )
		{
			crg::BufferSubresourceRange range{ 0u, VK_WHOLE_SIZE };
			memoryBarrier( context
				, commandBuffer
				, vertexBuffer.overlaysData->getBuffer()
				, range
				, crg::AccessState{ VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			memoryBarrier( context
				, commandBuffer
				, vertexBuffer.vertexBuffer.getBuffer().getBuffer()
				, range
				, crg::AccessState{ VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT }
				, crg::AccessState{ VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline.pipeline );
			VkDescriptorSet descriptorSet = *pipeline.descriptorSet;
			context.getContext().vkCmdBindDescriptorSets( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline.pipelineLayout
				, 0u
				, 1u
				, &descriptorSet
				, 0u
				, nullptr );
			context.getContext().vkCmdDispatch( commandBuffer
				, pipeline.count, 1u, 1u );
			memoryBarrier( context
				, commandBuffer
				, vertexBuffer.vertexBuffer.getBuffer().getBuffer()
				, range
				, crg::AccessState{ VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, crg::AccessState{ VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } );
			memoryBarrier( context
				, commandBuffer
				, vertexBuffer.overlaysData->getBuffer()
				, range
				, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, crg::AccessState{ VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
		}

		static ashes::DescriptorSetLayoutPtr createBaseDescriptorLayout( RenderDevice const & device )
		{
			auto & engine = *device.renderSystem.getEngine();
			auto & materials = engine.getMaterialCache();
			ashes::VkDescriptorSetLayoutBindingArray baseBindings;
			baseBindings.emplace_back( materials.getPassBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
			baseBindings.emplace_back( materials.getTexConfigBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
			baseBindings.emplace_back( materials.getTexAnimBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eCamera )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlays )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlaysIDs )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlaysSurfaces )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			return device->createDescriptorSetLayout( "OverlaysBase"
				, std::move( baseBindings ) );
		}
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderDevice const & device
		, Texture const & target
		, crg::FramePassTimer & timer
		, VkCommandBufferLevel level )
		: OwnedBy< RenderSystem >( device.renderSystem )
		, m_target{ target }
		, m_timer{ timer }
		, m_commands{ device, *device.graphicsData(), "OverlayRenderer", level }
		, m_noTexDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) } } }
		, m_texDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, texture ) } } }
		, m_noTexTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_texTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, texture ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_cameraUbo{ device }
		, m_baseDescriptorLayout{ ovrlrend::createBaseDescriptorLayout( device ) }
		, m_panelVertexBuffer{ std::make_unique< PanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "PanelOverlays"
			, device
			, m_cameraUbo
			, *m_baseDescriptorLayout
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxOverlaysPerBuffer ) }
		, m_borderVertexBuffer{ std::make_unique< BorderPanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "BorderOverlays"
			, device
			, m_cameraUbo
			, *m_baseDescriptorLayout
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxOverlaysPerBuffer ) }
		, m_textVertexBuffer{ std::make_unique< TextVertexBufferPool >( *getRenderSystem()->getEngine()
			, "TextOverlays"
			, device
			, m_cameraUbo
			, *m_baseDescriptorLayout
			, m_noTexTextDeclaration
			, m_texTextDeclaration
			, MaxOverlaysPerBuffer
			, std::make_unique< OverlayTextBufferPool >( *getRenderSystem()->getEngine()
				, "TextsGlyphs"
				, device ) ) }
		, m_size{ makeSize( m_target.getExtent() ) }
		, m_computePanelPipeline{ doCreatePanelPipeline( device ) }
		, m_computeBorderPipeline{ doCreateBorderPipeline( device ) }
		, m_computeTextPipeline{ doCreateTextPipeline( device ) }
	{
		std::string name = "Overlays";

		ashes::VkDescriptorSetLayoutBindingArray textBindings;
		textBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_textDescriptorLayout = device->createDescriptorSetLayout( name + "Text"
			, std::move( textBindings ) );
		m_textDescriptorPool = m_textDescriptorLayout->createPool( MaxOverlaysPerBuffer );

		m_cameraUbo.cpuUpdate( getSize()
			, getRenderSystem()->getOrtho( 0.0f
				, float( m_size.getWidth() )
				, 0.0f
				, float( m_size.getHeight() )
				, -1.0f
				, 1.0f ) );
	}

	OverlayRenderer::~OverlayRenderer()
	{
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();
		m_panelPipelines.clear();
		m_textPipelines.clear();
		m_panelVertexBuffer.reset();
		m_borderVertexBuffer.reset();
		m_textVertexBuffer.reset();
		m_commands = {};
	}

	void OverlayRenderer::update( GpuUpdater & updater )
	{
		if ( auto timerBlock = std::make_unique< crg::FramePassTimerBlock >( m_timer.start() ) )
		{
			auto size = updater.camera->getSize();

			if ( m_size != size )
			{
				m_sizeChanged = true;
				m_size = size;
				m_cameraUbo.cpuUpdate( getSize()
					, getRenderSystem()->getOrtho( 0.0f
						, float( m_size.getWidth() )
						, 0.0f
						, float( m_size.getHeight() )
						, -1.0f
						, 1.0f ) );
			}
		}
	}

	OverlayPreparer OverlayRenderer::beginPrepare( RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
	{
		return OverlayPreparer{ *this, device, renderPass, framebuffer };
	}

	void OverlayRenderer::upload( ashes::CommandBuffer const & cb )
	{
		m_panelVertexBuffer->upload( cb );
		m_borderVertexBuffer->upload( cb );
		m_textVertexBuffer->upload( cb );
	}

	void OverlayRenderer::registerComputeCommands( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )const
	{
		if ( m_computePanelPipeline.count )
		{
			ovrlrend::registerComputeBufferCommands( context
				, commandBuffer
				, m_computePanelPipeline
				, *m_panelVertexBuffer );
		}

		if ( m_computeBorderPipeline.count )
		{
			ovrlrend::registerComputeBufferCommands( context
				, commandBuffer
				, m_computeBorderPipeline
				, *m_borderVertexBuffer );
		}

		bool hasTexts{};

		for ( auto & it : m_computeTextPipeline.sets )
		{
			hasTexts = hasTexts || it.second.count > 0;
		}

		if ( hasTexts )
		{
			crg::BufferSubresourceRange range{ 0u, VK_WHOLE_SIZE };
			// Common buffers preparation
			ovrlrend::memoryBarrier( context
				, commandBuffer
				, m_textVertexBuffer->overlaysData->getBuffer()
				, range
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			ovrlrend::memoryBarrier( context
				, commandBuffer
				, m_textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
				, range
				, { VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT }
				, { VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );

			for ( auto & it : m_computeTextPipeline.sets )
			{
				if ( it.second.count )
				{
					ovrlrend::registerComputeBufferCommands( context
						, commandBuffer
						, m_computeTextPipeline
						, it.second );
				}
			}

			// Common buffers restore
			ovrlrend::memoryBarrier( context
				, commandBuffer
				, m_textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
				, range
				, { VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } );
			ovrlrend::memoryBarrier( context
				, commandBuffer
				, m_textVertexBuffer->overlaysData->getBuffer()
				, range
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
		}
	}

	void OverlayRenderer::doBeginPrepare( VkRenderPass renderPass
		, VkFramebuffer framebuffer )
	{
		m_timerBlock = std::make_unique< crg::FramePassTimerBlock >( m_timer.start() );
		m_retired.clear();
		m_commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, 0u
				, framebuffer
				, VK_FALSE
				, 0u
				, 0u ) );
		m_commands.commandBuffer->setViewport( makeViewport( m_size ) );
		m_commands.commandBuffer->setScissor( makeScissor( m_size ) );
	}

	void OverlayRenderer::doEndPrepare()
	{
		m_commands.commandBuffer->end();
		m_sizeChanged = false;
		m_timerBlock = {};
	}

	OverlayRenderNode & OverlayRenderer::doGetPanelNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_panelPipelines, false );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayRenderNode & OverlayRenderer::doGetTextNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_textPipelines, true );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	ashes::DescriptorSet const & OverlayRenderer::doCreateTextDescriptorSet( FontTexture & fontTexture )
	{
		auto ires = m_textDescriptorSets.emplace( &fontTexture, FontTextureDescriptorConnection{} );
		auto & descriptorConnection = ires.first->second;

		if ( ires.second || !descriptorConnection.descriptorSet )
		{
			auto result = m_textDescriptorPool->createDescriptorSet( "TextOverlays_" + std::to_string( intptr_t( &fontTexture ) ) );
			result->createBinding( m_textDescriptorLayout->getBinding( 0u )
				, fontTexture.getTexture()->getDefaultView().getSampledView()
				, fontTexture.getSampler().lock()->getSampler() );
			result->update();
			descriptorConnection.descriptorSet = std::move( result );
			descriptorConnection.connection = fontTexture.onResourceChanged.connect( [this, &descriptorConnection, &fontTexture]( DoubleBufferedTextureLayout const & )
				{
					m_retired.emplace_back( std::move( descriptorConnection.descriptorSet ) );
					m_textVertexBuffer->clearDrawDescriptorSets( &fontTexture );
				} );
		}

		return *descriptorConnection.descriptorSet;
	}

	OverlayPipeline OverlayRenderer::doCreatePipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, ashes::PipelineShaderStageCreateInfoArray program
		, TextureCombine const & texturesFlags
		, bool text )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments{ { VK_TRUE
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, defaultColorWriteMask } };
		ashes::PipelineColorBlendStateCreateInfo blState{ 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachments ) };
		std::string name = "Overlays";

		auto vertexLayout = ( texturesFlags.configCount == 0u
			? &m_noTexDeclaration
			: &m_texDeclaration );
		name += "_" + std::to_string( texturesFlags.configCount );
		ashes::DescriptorSetLayoutCRefArray descriptorLayouts;
		descriptorLayouts.push_back( *m_baseDescriptorLayout );
		descriptorLayouts.push_back( *getOwner()->getEngine()->getTextureUnitCache().getDescriptorLayout() );

		if ( text )
		{
			vertexLayout = ( texturesFlags.configCount == 0u
				? &m_noTexTextDeclaration
				: &m_texTextDeclaration );
			name = "Text" + name;
			descriptorLayouts.push_back( *m_textDescriptorLayout );
		}

		auto pipelineLayout = device->createPipelineLayout( name
			, descriptorLayouts
			, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_VERTEX_BIT, 0u, uint32_t( sizeof( DrawConstants ) ) } } );
		auto pipeline = device->createPipeline( name
			, { 0u
				, std::move( program )
				, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
				, ashes::nullopt
				, ashes::PipelineViewportStateCreateInfo{}
				, ashes::PipelineRasterizationStateCreateInfo{}
				, ashes::PipelineMultisampleStateCreateInfo{}
				, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE }
				, std::move( blState )
				, ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } }
				, *pipelineLayout
				, renderPass } );
		return OverlayPipeline{ std::move( pipelineLayout )
			, std::move( pipeline ) };
	}

	OverlayPipeline & OverlayRenderer::doGetPipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, std::map< uint32_t, OverlayPipeline > & pipelines
		, bool text )
	{
		// Remove unwanted flags
		auto & passComponents = device.renderSystem.getEngine()->getPassComponentsRegister();
		auto textures = passComponents.filterTextureFlags( ComponentModeFlag::eColour | ComponentModeFlag::eOpacity
			, pass.getTexturesMask() );
		auto key = ovrlrend::makeKey( passComponents, textures, text );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.emplace( key
				, doCreatePipeline( device
					, renderPass
					, pass
					, doCreateOverlayProgram( device, textures, text )
					, textures
					, text ) ).first;
		}

		return it->second;
	}

	OverlayRenderer::ComputePipeline OverlayRenderer::doCreatePanelPipeline( RenderDevice const & device )
	{
		ComputePipeline result;
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u // Camera
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 1u // Overlays
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 2u // Output
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		result.descriptorLayout = device->createDescriptorSetLayout( "PanelOverlayCompute"
			, layoutBindings );

		result.pipelineLayout = device->createPipelineLayout( "PanelOverlayCompute"
			, *result.descriptorLayout );
		result.pipeline = device->createPipeline( "PanelOverlayCompute"
			, ashes::ComputePipelineCreateInfo{ 0u
			, PanelOverlay::createProgram( device )
			, * result.pipelineLayout } );

		result.descriptorPool = result.descriptorLayout->createPool( "PanelOverlayCompute"
			, 1u );
		result.descriptorSet = result.descriptorPool->createDescriptorSet( "PanelOverlayCompute" );
		ashes::WriteDescriptorSetArray setBindings;
		m_cameraUbo.createSizedBinding( *result.descriptorSet
			, result.descriptorLayout->getBinding( 0u ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( 1u )
			, *m_panelVertexBuffer->overlaysData
			, 0u
			, uint32_t( m_panelVertexBuffer->overlaysData->getCount() ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( 2u )
			, m_panelVertexBuffer->vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( m_panelVertexBuffer->vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result.descriptorSet->update();

		return result;
	}

	OverlayRenderer::ComputePipeline OverlayRenderer::doCreateBorderPipeline( RenderDevice const & device )
	{
		std::string name = "BorderOverlayCompute";
		ComputePipeline result;
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u // Camera
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 1u // Overlays
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( 2u // Output
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		result.descriptorLayout = device->createDescriptorSetLayout( name
			, layoutBindings );

		result.pipelineLayout = device->createPipelineLayout( name
			, *result.descriptorLayout );
		result.pipeline = device->createPipeline( name
			, ashes::ComputePipelineCreateInfo{ 0u
			, BorderPanelOverlay::createProgram( device )
			, * result.pipelineLayout } );

		result.descriptorPool = result.descriptorLayout->createPool( name
			, 1u );
		result.descriptorSet = result.descriptorPool->createDescriptorSet( name );
		ashes::WriteDescriptorSetArray setBindings;
		m_cameraUbo.createSizedBinding( *result.descriptorSet
			, result.descriptorLayout->getBinding( 0u ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( 1u )
			, *m_borderVertexBuffer->overlaysData
			, 0u
			, uint32_t( m_borderVertexBuffer->overlaysData->getCount() ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( 2u )
			, m_borderVertexBuffer->vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( m_borderVertexBuffer->vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result.descriptorSet->update();

		return result;
	}

	OverlayRenderer::TextPipeline OverlayRenderer::doCreateTextPipeline( RenderDevice const & device )
	{
		std::string name = "TextOverlayCompute";
		OverlayRenderer::TextPipeline result;
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eCamera )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eOverlays )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eChars )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eWords )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eLines )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eFont )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eVertex )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		result.descriptorLayout = device->createDescriptorSetLayout( name
			, layoutBindings );

		result.pipelineLayout = device->createPipelineLayout( name
			, *result.descriptorLayout
			, VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( uint32_t ) * 2u } );
		result.pipeline = device->createPipeline( name
			, ashes::ComputePipelineCreateInfo{ 0u
				, TextOverlay::createProgram( device )
				, *result.pipelineLayout } );

		result.descriptorPool = result.descriptorLayout->createPool( name
			, 1000u );

		return result;
	}

	ashes::DescriptorSetPtr OverlayRenderer::doGetTextSet( FontTexture const & fontTexture )
	{
		std::string name = "TextOverlayCompute-" + fontTexture.getFontName();
		auto result = m_computeTextPipeline.descriptorPool->createDescriptorSet( name );
		auto & descriptorLayout = *m_computeTextPipeline.descriptorLayout;
		auto & descriptorSet = *result;
		ashes::WriteDescriptorSetArray setBindings;
		m_cameraUbo.createSizedBinding( descriptorSet
			, descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eCamera ) ) );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eOverlays ) )
			, *m_textVertexBuffer->overlaysData
			, 0u
			, uint32_t( m_textVertexBuffer->overlaysData->getCount() ) );
		m_textVertexBuffer->fillComputeDescriptorSet( &fontTexture
			, descriptorLayout
			, descriptorSet );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eVertex ) )
			, m_textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( m_textVertexBuffer->vertexBuffer.getBuffer().getBuffer().getSize() ) );
		descriptorSet.update();
		return result;
	}

	OverlayRenderer::TextPipelineSet & OverlayRenderer::doGetComputeTextPipeline( FontTexture & fontTexture )
	{
		auto it = m_computeTextPipeline.sets.find( &fontTexture );

		if ( it == m_computeTextPipeline.sets.end() )
		{
			it = m_computeTextPipeline.sets.emplace( &fontTexture
				, TextPipelineSet{ doGetTextSet( fontTexture )
				, 0u
				, m_textVertexBuffer->getTextBuffer( fontTexture ) } ).first;
		}

		return it->second;
	}

	ashes::PipelineShaderStageCreateInfoArray OverlayRenderer::doCreateOverlayProgram( RenderDevice const & device
		, TextureCombine const & texturesFlags
		, bool textOverlay )
	{
		using namespace sdw;
		using namespace shader;
		bool hasTexture = texturesFlags.configCount != 0u;

		// Vertex shader
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Overlay" };
		{
			VertexWriter writer;

			C3D_Camera( writer
				, OverlayBindingId::eCamera
				, 0u );
			C3D_Overlays( writer
				, OverlayBindingId::eOverlays
				, 0u );
			C3D_OverlaysIDs( writer
				, OverlayBindingId::eOverlaysIDs
				, 0u );
			C3D_OverlaysSurfaces( writer
				, OverlayBindingId::eOverlaysSurfaces
				, 0u
				, textOverlay
				, true  /* hasTextures */ );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
			auto overlaySubID = pcb.declMember< sdw::UInt >( "overlaySubID" );
			pcb.end();

			writer.implementMainT< sdw::VoidT, shader::OverlaySurfaceT >( sdw::VertexIn{ writer }
				, VertexOutT< shader::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, [&]( sdw::VertexIn in
					, VertexOutT< shader::OverlaySurfaceT > out )
				{
					auto overlayID = writer.declLocale( "overlayID"
						, c3d_overlaysIDs[overlaySubID] );
					auto overlay = writer.declLocale( "overlay"
						, c3d_overlaysData[overlayID] );
					auto vertexOffset = writer.declLocale( "vertexOffset"
						, writer.cast< sdw::UInt >( in.vertexIndex ) + overlay.vertexOffset() );
					auto surface = writer.declLocale( "vertex"
						, c3d_overlaysSurfaces[vertexOffset] );
					auto renderSize = writer.declLocale( "renderSize"
						, vec2( c3d_cameraData.renderSize() ) );
					auto parentSize = writer.declLocale( "parentSize"
						, overlay.parentRect().zw() - overlay.parentRect().xy() );
					auto absolutePosition = writer.declLocale( "absolutePosition"
						, overlay.relativePosition() * parentSize );

					out.texUV = surface.texUV;
					out.fontUV = surface.fontUV;
					out.vtx.position = c3d_cameraData.viewToProj( vec4( renderSize * ( absolutePosition + surface.position + overlay.parentRect().xy() )
							, 0.0_f
							, 1.0_f ) );
					out.position = out.vtx.position.xy();
					out.materialId = overlay.materialId();
				} );

			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		// Pixel shader
		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Overlay" };
		{
			FragmentWriter writer;

			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ getOwner()->getEngine()->getPassComponentsRegister()
				, texturesFlags
				, ( ComponentModeFlag::eOpacity
					| ComponentModeFlag::eColour )
				, utils };
			shader::Materials materials{ writer
				, passShaders
				, uint32_t( OverlayBindingId::eMaterials )
				, 0u };
			shader::TextureConfigurations textureConfigs{ writer
				, uint32_t( OverlayBindingId::eTexConfigs )
				, 0u
				, hasTexture };
			shader::TextureAnimations textureAnims{ writer
				, uint32_t( OverlayBindingId::eTexAnims )
				, 0u
				, hasTexture };

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
				, 0u
				, 1u
				, hasTexture ) );

			auto c3d_mapText = writer.declCombinedImg< FImg2DR32 >( "c3d_mapText"
				, 0u
				, 2u
				, textOverlay );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< shader::OverlaySurfaceT, VoidT >( FragmentInT< shader::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, FragmentOut{ writer }
					, [&]( FragmentInT< shader::OverlaySurfaceT > in
						, FragmentOut out )
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( in.materialId ) );

					if ( textOverlay )
					{
						material.opacity *= c3d_mapText.sample( in.fontUV, 0.0_f );
					}

					auto outComponents = writer.declLocale< ovrlrend::OverlayBlendComponents >( "outComponents"
						, ovrlrend::OverlayBlendComponents{ materials
							, material
							, in } );

					if ( hasTexture )
					{
						textureConfigs.computeMapsContributions( passShaders
							, texturesFlags
							, textureAnims
							, c3d_maps
							, material
							, outComponents );
					}

					outColour = vec4( outComponents.colour, outComponents.opacity );
				} );

			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	//*********************************************************************************************
}
