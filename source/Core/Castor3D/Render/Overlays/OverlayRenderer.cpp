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

CU_ImplementSmartPtr( castor3d, OverlayRenderer )

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

		static std::string makeName( PassComponentRegister const & passComponents
			, TextureCombine const & textures )
		{
			auto result = std::to_string( textures.configCount );

			if ( hasAny( textures, passComponents.getColourMapFlags() ) )
			{
				result += "Col";
			}

			if ( hasAny( textures, passComponents.getOpacityMapFlags() ) )
			{
				result += "Opa";
			}

			return result;
		}

		static uint32_t makeKey( PassComponentRegister const & passComponents
			, TextureCombine const & textures
			, bool borderOverlay
			, bool textOverlay )
		{
			auto tex{ uint32_t( textOverlay ? OverlayTexture::eText : OverlayTexture::eNone ) };

			if ( hasAny( textures, passComponents.getColourMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eColour );
			}

			if ( hasAny( textures, passComponents.getOpacityMapFlags() ) )
			{
				tex |= uint32_t( OverlayTexture::eOpacity );
			}

			uint32_t result{ tex };
			result <<= 8u;
			result |= uint32_t( textures.configCount & 0xF );
			result |= ( borderOverlay ? 1u : 0u );
			return result;
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

	OverlayRenderer::OverlaysCommonData::OverlaysCommonData( RenderDevice const & device )
		: baseDescriptorLayout{ ovrlrend::createBaseDescriptorLayout( device ) }
		, cameraUbo{ device }
		, panelVertexBuffer{ std::make_unique< PanelVertexBufferPool >( *device.renderSystem.getEngine()
			, "PanelOverlays"
			, device
			, cameraUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer ) }
		, borderVertexBuffer{ std::make_unique< BorderPanelVertexBufferPool >( *device.renderSystem.getEngine()
			, "BorderOverlays"
			, device
			, cameraUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer ) }
		, textVertexBuffer{ std::make_unique< TextVertexBufferPool >( *device.renderSystem.getEngine()
			, "TextOverlays"
			, device
			, cameraUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer
			, castor::makeUnique< OverlayTextBufferPool >( *device.renderSystem.getEngine()
				, "TextsGlyphs"
				, device ) ) }
	{
	}

	//*********************************************************************************************

	OverlayRenderer::OverlaysComputeData::OverlaysComputeData( RenderDevice const & device
		, OverlaysCommonData & commonData )
		: panelPipeline{ doCreatePanelPipeline( device, *commonData.panelVertexBuffer, commonData.cameraUbo ) }
		, borderPipeline{ doCreateBorderPipeline( device, *commonData.borderVertexBuffer, commonData.cameraUbo ) }
		, textPipeline{ doCreateTextPipeline( device ) }
		, m_commonData{ commonData }
	{
	}

	void OverlayRenderer::OverlaysComputeData::reset()
	{
		panelPipeline.count = 0u;
		borderPipeline.count = 0u;

		for ( auto & it : textPipeline.sets )
		{
			it.second.count = 0u;
		}
	}

	void OverlayRenderer::OverlaysComputeData::registerCommands( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )const
	{
		if ( panelPipeline.count )
		{
			doRegisterComputeBufferCommands( context
				, commandBuffer
				, panelPipeline
				, m_commonData.panelVertexBuffer->overlaysData->getBuffer()
				, m_commonData.panelVertexBuffer->vertexBuffer.getBuffer().getBuffer() );
		}

		if ( borderPipeline.count )
		{
			doRegisterComputeBufferCommands( context
				, commandBuffer
				, borderPipeline
				, m_commonData.borderVertexBuffer->overlaysData->getBuffer()
				, m_commonData.borderVertexBuffer->vertexBuffer.getBuffer().getBuffer() );
		}

		bool hasTexts{};

		for ( auto & it : textPipeline.sets )
		{
			hasTexts = hasTexts || it.second.count > 0;
		}

		if ( hasTexts )
		{
			crg::BufferSubresourceRange range{ 0u, VK_WHOLE_SIZE };
			// Common buffers preparation
			memoryBarrier( context
				, commandBuffer
				, m_commonData.textVertexBuffer->overlaysData->getBuffer()
				, range
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			memoryBarrier( context
				, commandBuffer
				, m_commonData.textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
				, range
				, { VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT }
				, { VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );

			for ( auto & it : textPipeline.sets )
			{
				if ( it.second.count )
				{
					doRegisterComputeBufferCommands( context
						, commandBuffer
						, textPipeline
						, it.second );
				}
			}

			// Common buffers restore
			memoryBarrier( context
				, commandBuffer
				, m_commonData.textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
				, range
				, { VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } );
			memoryBarrier( context
				, commandBuffer
				, m_commonData.textVertexBuffer->overlaysData->getBuffer()
				, range
				, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
				, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
		}
	}

	OverlayRenderer::TextComputePipelineDescriptor & OverlayRenderer::OverlaysComputeData::getTextPipeline( FontTexture & fontTexture )
	{
		auto it = textPipeline.sets.find( &fontTexture );

		if ( it == textPipeline.sets.end() )
		{
			it = textPipeline.sets.emplace( &fontTexture
				, TextComputePipelineDescriptor{ doGetTextDescriptorSet( fontTexture )
					, 0u
					, m_commonData.textVertexBuffer->getTextBuffer( fontTexture ) } ).first;
		}

		return it->second;
	}

	OverlayRenderer::ComputePipeline OverlayRenderer::OverlaysComputeData::doCreatePanelPipeline( RenderDevice const & device
		, PanelVertexBufferPool & vertexBuffer
		, CameraUbo const & cameraUbo )
	{
		ComputePipeline result;
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eCamera )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eOverlays )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eVertex )
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
		cameraUbo.createSizedBinding( *result.descriptorSet
			, result.descriptorLayout->getBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eCamera ) ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eOverlays ) )
			, *vertexBuffer.overlaysData
			, 0u
			, uint32_t( vertexBuffer.overlaysData->getCount() ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( uint32_t( PanelOverlay::ComputeBindingIdx::eVertex ) )
			, vertexBuffer.vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( vertexBuffer.vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result.descriptorSet->update();

		return result;
	}

	OverlayRenderer::ComputePipeline OverlayRenderer::OverlaysComputeData::doCreateBorderPipeline( RenderDevice const & device
		, BorderPanelVertexBufferPool & vertexBuffer
		, CameraUbo const & cameraUbo )
	{
		std::string name = "BorderOverlayCompute";
		ComputePipeline result;
		ashes::VkDescriptorSetLayoutBindingArray layoutBindings;
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eCamera )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eOverlays )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eVertex )
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
		cameraUbo.createSizedBinding( *result.descriptorSet
			, result.descriptorLayout->getBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eCamera ) ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eOverlays ) )
			, *vertexBuffer.overlaysData
			, 0u
			, uint32_t( vertexBuffer.overlaysData->getCount() ) );
		result.descriptorSet->createBinding( result.descriptorLayout->getBinding( uint32_t( BorderPanelOverlay::ComputeBindingIdx::eVertex ) )
			, vertexBuffer.vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( vertexBuffer.vertexBuffer.getBuffer().getBuffer().getSize() ) );
		result.descriptorSet->update();

		return result;
	}

	OverlayRenderer::TextComputePipeline OverlayRenderer::OverlaysComputeData::doCreateTextPipeline( RenderDevice const & device )
	{
		std::string name = "TextOverlayCompute";
		OverlayRenderer::TextComputePipeline result;
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

	ashes::DescriptorSetPtr OverlayRenderer::OverlaysComputeData::doGetTextDescriptorSet( FontTexture const & fontTexture )
	{
		std::string name = "TextOverlayCompute-" + fontTexture.getFontName();
		auto result = textPipeline.descriptorPool->createDescriptorSet( name );
		auto & descriptorLayout = *textPipeline.descriptorLayout;
		auto & descriptorSet = *result;
		ashes::WriteDescriptorSetArray setBindings;
		m_commonData.cameraUbo.createSizedBinding( descriptorSet
			, descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eCamera ) ) );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eOverlays ) )
			, *m_commonData.textVertexBuffer->overlaysData
			, 0u
			, uint32_t( m_commonData.textVertexBuffer->overlaysData->getCount() ) );
		m_commonData.textVertexBuffer->fillComputeDescriptorSet( &fontTexture
			, descriptorLayout
			, descriptorSet );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eVertex ) )
			, m_commonData.textVertexBuffer->vertexBuffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( m_commonData.textVertexBuffer->vertexBuffer.getBuffer().getBuffer().getSize() ) );
		descriptorSet.update();
		return result;
	}

	void OverlayRenderer::OverlaysComputeData::doRegisterComputeBufferCommands( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, OverlayRenderer::ComputePipeline const & pipeline
		, ashes::BufferBase const & overlaysBuffer
		, ashes::BufferBase const & vertexBuffer )const
	{
		crg::BufferSubresourceRange range{ 0u, VK_WHOLE_SIZE };
		memoryBarrier( context
			, commandBuffer
			, overlaysBuffer
			, range
			, crg::AccessState{ VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT }
			, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
		memoryBarrier( context
			, commandBuffer
			, vertexBuffer
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
			, vertexBuffer
			, range
			, crg::AccessState{ VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
			, crg::AccessState{ VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } );
		memoryBarrier( context
			, commandBuffer
			, overlaysBuffer
			, range
			, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
			, crg::AccessState{ VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
	}

	void OverlayRenderer::OverlaysComputeData::doRegisterComputeBufferCommands( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, OverlayRenderer::TextComputePipeline const & pipeline
		, OverlayRenderer::TextComputePipelineDescriptor const & set )const
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

	//*********************************************************************************************

	OverlayRenderer::OverlaysDrawData::OverlaysDrawData( RenderDevice const & device
		, VkCommandBufferLevel level
		, OverlaysCommonData & commonData )
		: commands{ device, *device.graphicsData(), "OverlayRenderer", level }
		, m_commonData{ commonData }
	{
		std::string name = "Overlays";
		ashes::VkDescriptorSetLayoutBindingArray textBindings;
		textBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		textDescriptorLayout = device->createDescriptorSetLayout( name + "Text"
			, std::move( textBindings ) );
		textDescriptorPool = textDescriptorLayout->createPool( name + "Text"
			, MaxOverlaysPerBuffer );
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getPanelNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_panelPipelines, false, false );
			it = m_mapPanelNodes.insert( { &pass, OverlayDrawNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getBorderNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto it = m_mapBorderNodes.find( &pass );

		if ( it == m_mapBorderNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_borderPipelines, true, false );
			it = m_mapBorderNodes.insert( { &pass, OverlayDrawNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getTextNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_textPipelines, false, true );
			it = m_mapTextNodes.insert( { &pass, OverlayDrawNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	ashes::DescriptorSet const & OverlayRenderer::OverlaysDrawData::createTextDescriptorSet( FontTexture & fontTexture )
	{
		auto ires = textDescriptorSets.emplace( &fontTexture, FontTextureDescriptorConnection{} );
		auto & descriptorConnection = ires.first->second;

		if ( ires.second || !descriptorConnection.descriptorSet )
		{
			auto result = textDescriptorPool->createDescriptorSet( "TextOverlays_" + std::to_string( intptr_t( &fontTexture ) ) );
			result->createBinding( textDescriptorLayout->getBinding( 0u )
				, fontTexture.getTexture()->getDefaultSampledView()
				, fontTexture.getSampler()->getSampler() );
			result->update();
			descriptorConnection.descriptorSet = std::move( result );
			descriptorConnection.connection = fontTexture.onResourceChanged.connect( [this, &descriptorConnection, &fontTexture]( DoubleBufferedTextureLayout const & )
				{
					retired.emplace_back( std::move( descriptorConnection.descriptorSet ) );
					m_commonData.textVertexBuffer->clearDrawPipelineData( &fontTexture );
				} );
		}

		return *descriptorConnection.descriptorSet;
	}

	void OverlayRenderer::OverlaysDrawData::beginPrepare( VkRenderPass renderPass
		, VkFramebuffer framebuffer
		, crg::Fence & fence
		, crg::FramePassTimer & timer
		, castor::Size const & size )
	{
		timerBlock = std::make_unique< crg::FramePassTimerBlock >( timer.start() );
		retired.clear();
		fence.wait( ashes::MaxTimeout );
		commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, 0u
				, framebuffer
				, VK_FALSE
				, 0u
				, 0u ) );
		commands.commandBuffer->setViewport( makeViewport( size ) );
		commands.commandBuffer->setScissor( makeScissor( size ) );
	}

	void OverlayRenderer::OverlaysDrawData::endPrepare()
	{
		commands.commandBuffer->end();
		timerBlock = {};
	}

	OverlayDrawPipeline & OverlayRenderer::OverlaysDrawData::doGetPipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, std::map< uint32_t, OverlayDrawPipeline > & pipelines
		, bool borderOverlay
		, bool textOverlay )
	{
		// Remove unwanted flags
		auto & passComponents = device.renderSystem.getEngine()->getPassComponentsRegister();
		auto textures = passComponents.filterTextureFlags( ComponentModeFlag::eColour | ComponentModeFlag::eOpacity
			, pass.getTexturesMask() );
		auto key = ovrlrend::makeKey( passComponents, textures, borderOverlay, textOverlay );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.emplace( key
				, doCreatePipeline( device
					, renderPass
					, pass
					, doCreateOverlayProgram( device, textures, borderOverlay, textOverlay )
					, textures
					, borderOverlay
					, textOverlay ) ).first;
		}

		return it->second;
	}

	OverlayDrawPipeline OverlayRenderer::OverlaysDrawData::doCreatePipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, ashes::PipelineShaderStageCreateInfoArray program
		, TextureCombine const & texturesFlags
		, bool borderOverlay
		, bool textOverlay )
	{
		auto & engine = *device.renderSystem.getEngine();
		auto & passComponents = engine.getPassComponentsRegister();
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

		name += "-" + ovrlrend::makeName( passComponents, texturesFlags );
		ashes::DescriptorSetLayoutCRefArray descriptorLayouts;
		descriptorLayouts.push_back( *m_commonData.baseDescriptorLayout );
		descriptorLayouts.push_back( *engine.getTextureUnitCache().getDescriptorLayout() );

		if ( textOverlay )
		{
			name = "Text" + name;
			descriptorLayouts.push_back( *textDescriptorLayout );
		}
		else if ( borderOverlay )
		{
			name = "Border" + name;
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
		return OverlayDrawPipeline{ std::move( pipelineLayout )
			, std::move( pipeline ) };
	}

	ashes::PipelineShaderStageCreateInfoArray OverlayRenderer::OverlaysDrawData::doCreateOverlayProgram( RenderDevice const & device
		, TextureCombine const & texturesFlags
		, bool borderOverlay
		, bool textOverlay )
	{
		using namespace sdw;
		using namespace shader;
		auto & engine = *device.renderSystem.getEngine();
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
			auto pipelineBaseIndex = pcb.declMember< sdw::UInt >( "pipelineBaseIndex" );
			pcb.end();

			writer.implementMainT< sdw::VoidT, shader::OverlaySurfaceT >( sdw::VertexIn{ writer }
				, VertexOutT< shader::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, [&]( sdw::VertexIn in
					, VertexOutT< shader::OverlaySurfaceT > out )
				{
					auto overlaySubID = writer.declLocale( "overlaySubID"
						, pipelineBaseIndex + ( writer.cast< sdw::UInt >( in.drawID ) ) );
					auto overlayID = writer.declLocale( "overlayID"
						, c3d_overlaysIDs[overlaySubID] );
					auto overlay = writer.declLocale( "overlay"
						, c3d_overlaysData[overlayID] );
					auto vertexOffset = writer.declLocale( "vertexOffset"
						, writer.cast< sdw::UInt >( in.vertexIndex ) );
					auto surface = writer.declLocale( "vertex"
						, c3d_overlaysSurfaces[vertexOffset] );
					auto renderSize = writer.declLocale( "renderSize"
						, vec2( c3d_cameraData.renderSize() ) );
					auto ssAbsParentSize = writer.declLocale( "ssAbsParentSize"
						, overlay.parentRect().zw() - overlay.parentRect().xy() );
					auto ssRelOvPosition = writer.declLocale( "ssRelOvPosition"
						, overlay.relativePosition() * ssAbsParentSize );
					auto ssAbsOvPosition = writer.declLocale( "ssAbsOvPosition"
						, ssRelOvPosition + overlay.parentRect().xy() );

					out.texUV = surface.texUV;
					out.fontUV = surface.fontUV;
					out.vtx.position = c3d_cameraData.viewToProj( vec4( renderSize * ( ssAbsOvPosition + surface.position )
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
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, texturesFlags
				, ( ComponentModeFlag::eOpacity
					| ComponentModeFlag::eColour )
				, utils
				, true };
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

	OverlayRenderer::OverlayRenderer( RenderDevice const & device
		, Texture const & target
		, crg::FramePassTimer & timer
		, VkCommandBufferLevel level )
		: OwnedBy< RenderSystem >( device.renderSystem )
		, m_target{ target }
		, m_timer{ timer }
		, m_size{ makeSize( m_target.getExtent() ) }
		, m_common{ device }
		, m_draw{ device, level, m_common }
		, m_compute{ device, m_common }
	{
		m_common.cameraUbo.cpuUpdate( getSize()
			, getRenderSystem()->getOrtho( 0.0f
				, float( m_size.getWidth() )
				, 0.0f
				, float( m_size.getHeight() )
				, -1.0f
				, 1.0f )
			, 0u );
	}

	OverlayRenderer::~OverlayRenderer()
	{
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
				m_common.cameraUbo.cpuUpdate( getSize()
					, getRenderSystem()->getOrtho( 0.0f
						, float( m_size.getWidth() )
						, 0.0f
						, float( m_size.getHeight() )
						, -1.0f
						, 1.0f )
					, 0u );
			}
		}
	}

	OverlayPreparer OverlayRenderer::beginPrepare( RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer
		, crg::Fence & fence )
	{
		return OverlayPreparer{ *this, device, renderPass, framebuffer, fence };
	}

	void OverlayRenderer::upload( UploadData & uploader )
	{
		m_common.panelVertexBuffer->upload( uploader );
		m_common.borderVertexBuffer->upload( uploader );
		m_common.textVertexBuffer->upload( uploader );
	}

	void OverlayRenderer::registerComputeCommands( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )const
	{
		m_compute.registerCommands( context, commandBuffer );
	}

	ashes::CommandBuffer & OverlayRenderer::doBeginPrepare( VkRenderPass renderPass
		, VkFramebuffer framebuffer
		, crg::Fence & fence )
	{
		m_draw.beginPrepare( renderPass, framebuffer, fence, m_timer, m_size );
		return *m_draw.commands.commandBuffer;
	}

	void OverlayRenderer::doEndPrepare()
	{
		m_draw.endPrepare();
		m_sizeChanged = false;
	}

	std::pair< OverlayDrawNode *, OverlayPipelineData * > OverlayRenderer::doGetDrawNodeData( RenderDevice const & device
		, VkRenderPass renderPass
		, Overlay const & overlay
		, Pass const & pass
		, bool secondary )
	{
		OverlayDrawNode * node{};
		OverlayPipelineData * pipelineData{};

		switch ( overlay.getType() )
		{
		case OverlayType::ePanel:
			node = &m_draw.getPanelNode( device, renderPass, pass );
			pipelineData = &m_common.panelVertexBuffer->getDrawPipelineData( node->pipeline
				, nullptr
				, nullptr );
			++m_compute.panelPipeline.count;
			break;
		case OverlayType::eBorderPanel:
			if ( secondary )
			{
				node = &m_draw.getBorderNode( device, renderPass, pass );
				pipelineData = &m_common.borderVertexBuffer->getDrawPipelineData( node->pipeline
					, nullptr
					, nullptr );
				++m_compute.borderPipeline.count;
			}
			else
			{
				node = &m_draw.getPanelNode( device, renderPass, pass );
				pipelineData = &m_common.panelVertexBuffer->getDrawPipelineData( node->pipeline
					, nullptr
					, nullptr );
				++m_compute.panelPipeline.count;
			}
			break;
		case OverlayType::eText:
			if ( auto text = overlay.getTextOverlay() )
			{
				auto texture = text->getFontTexture();
				node = &m_draw.getTextNode( device, renderPass, pass, *texture->getTexture(), *texture->getSampler() );
				pipelineData = &m_common.textVertexBuffer->getDrawPipelineData( node->pipeline
					, texture
					, &m_draw.createTextDescriptorSet( *texture ) );
				m_compute.getTextPipeline( *texture ).count += text->getCharCount();
			}
			break;
		default:
			break;
		}

		return { node, pipelineData };
	}

	//*********************************************************************************************
}
