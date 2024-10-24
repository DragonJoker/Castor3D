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
#include "Castor3D/Shader/Ubos/FontUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/RecordContext.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

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
					, castor::move( expr )
					, enabled }
			{
			}

			explicit OverlayBlendComponents( shader::Materials const & materials )
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
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter const & writer
				, shader::Materials const & materials )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, initializers );
				return sdw::makeAggrInit( type, castor::move( initializers ) );
			}

			template< ast::var::Flag FlagT >
			static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter const & writer
				, shader::Materials const & materials
				, shader::Material const & material
				, shader::OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, material, surface, initializers );
				return sdw::makeAggrInit( type, castor::move( initializers ) );
			}
		};

		static castor::String makeName( PassComponentRegister const & passComponents
			, TextureCombine const & textures )
		{
			auto result = castor::string::toString( textures.configCount );

			if ( hasAny( textures, passComponents.getColourMapFlags() ) )
			{
				result += cuT( "Col" );
			}

			if ( hasAny( textures, passComponents.getOpacityMapFlags() ) )
			{
				result += cuT( "Opa" );
			}

			return result;
		}

		static size_t makeKey( Pass const & pass
			, VkRenderPass renderPass )
		{
			auto result = std::hash< Pass const * >{}( &pass );
			return castor::hashCombine( result, renderPass );
		}

		static size_t makeKey( PassComponentRegister const & passComponents
			, TextureCombine const & textures
			, VkRenderPass renderPass
			, bool borderOverlay
			, bool textOverlay
			, bool sdfFont )
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

			size_t result{ tex };
			result <<= 1u;
			result |= ( sdfFont ? 1u : 0u );
			result <<= 8u;
			result |= uint32_t( textures.configCount & 0x0F );
			result |= ( borderOverlay ? 1u : 0u );
			return castor::hashCombine( result, renderPass );
		}

		static ashes::DescriptorSetLayoutPtr createBaseDescriptorLayout( RenderDevice const & device )
		{
			auto const & engine = *device.renderSystem.getEngine();
			auto const & materials = engine.getMaterialCache();
			ashes::VkDescriptorSetLayoutBindingArray baseBindings;
			baseBindings.emplace_back( materials.getPassBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
			baseBindings.emplace_back( materials.getTexConfigBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
			baseBindings.emplace_back( materials.getTexAnimBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eCamera )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eHdrConfig )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlays )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlaysIDs )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlaysSurfaces )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			baseBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlaysFont )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );

			return device->createDescriptorSetLayout( "OverlaysBase"
				, castor::move( baseBindings ) );
		}

		struct TextBatchData
		{
			uint32_t batchOffset;
			uint32_t charCount;
		};
	}

	//*********************************************************************************************

	OverlayRenderer::OverlaysCommonData::OverlaysCommonData( RenderDevice const & device
		, HdrConfigUbo const & hdrConfigUbo )
		: baseDescriptorLayout{ ovrlrend::createBaseDescriptorLayout( device ) }
		, cameraUbo{ device }
		, panelVertexBuffer{ castor::make_unique< PanelVertexBufferPool >( *device.renderSystem.getEngine()
			, cuT( "PanelOverlays" )
			, device
			, cameraUbo
			, hdrConfigUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer ) }
		, borderVertexBuffer{ castor::make_unique< BorderPanelVertexBufferPool >( *device.renderSystem.getEngine()
			, cuT( "BorderOverlays" )
			, device
			, cameraUbo
			, hdrConfigUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer ) }
		, textVertexBuffer{ castor::make_unique< TextVertexBufferPool >( *device.renderSystem.getEngine()
			, cuT( "TextOverlays" )
			, device
			, cameraUbo
			, hdrConfigUbo
			, *baseDescriptorLayout
			, MaxOverlaysPerBuffer
			, castor::makeUnique< OverlayTextBufferPool >( *device.renderSystem.getEngine()
				, cuT( "TextsGlyphs" )
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

		for ( auto & [_, it] : textPipeline.sets )
		{
			it.count = 0u;
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

		for ( auto & [_, set] : textPipeline.sets )
		{
			hasTexts = hasTexts || set.count > 0;
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

			for ( auto & [_, set] : textPipeline.sets )
			{
				if ( set.count )
				{
					doRegisterComputeBufferCommands( context
						, commandBuffer
						, textPipeline
						, set );
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

	OverlayRenderer::TextComputePipelineDescriptor & OverlayRenderer::OverlaysComputeData::getTextPipeline( FontTexture const & fontTexture )
	{
		auto it = textPipeline.sets.find( &fontTexture );

		if ( it == textPipeline.sets.end() )
		{
			it = textPipeline.sets.try_emplace( &fontTexture
				, doGetTextDescriptorSet( fontTexture )
				, 0u
				, m_commonData.textVertexBuffer->getTextBuffer( fontTexture ) ).first;
		}

		return it->second;
	}

	OverlayRenderer::ComputePipeline OverlayRenderer::OverlaysComputeData::doCreatePanelPipeline( RenderDevice const & device
		, PanelVertexBufferPool & vertexBuffer
		, CameraUbo const & cameraUbo )const
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
		, CameraUbo const & cameraUbo )const
	{
		castor::MbString name = "BorderOverlayCompute";
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

	OverlayRenderer::TextComputePipeline OverlayRenderer::OverlaysComputeData::doCreateTextPipeline( RenderDevice const & device )const
	{
		castor::MbString name = "TextOverlayCompute";
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
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		layoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( TextOverlay::ComputeBindingIdx::eVertex )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_COMPUTE_BIT ) );
		result.descriptorLayout = device->createDescriptorSetLayout( name
			, layoutBindings );

		result.pipelineLayout = device->createPipelineLayout( name
			, *result.descriptorLayout
			, VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( ovrlrend::TextBatchData ) } );
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
		castor::MbString name = "TextOverlayCompute-" + castor::toUtf8( fontTexture.getFontName() );
		auto result = textPipeline.descriptorPool->createDescriptorSet( name );
		auto const & descriptorLayout = *textPipeline.descriptorLayout;
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
		ovrlrend::TextBatchData data{ 0u, set.count };

		while ( data.batchOffset < data.charCount )
		{
			context.getContext().vkCmdPushConstants( commandBuffer
				, *pipeline.pipelineLayout
				, VK_SHADER_STAGE_COMPUTE_BIT
				, 0u
				, sizeof( ovrlrend::TextBatchData )
				, &data );
			context.getContext().vkCmdDispatch( commandBuffer
				, batchCountX, batchCountY, 1u );
			data.batchOffset += batchCount;
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
		, OverlaysCommonData & commonData
		, bool isHdr )
		: commands{ device, *device.graphicsData(), cuT( "OverlayRenderer" ), level }
		, m_commonData{ commonData }
		, m_isHdr{ isHdr }
	{
		castor::MbString name = "Overlays";
		ashes::VkDescriptorSetLayoutBindingArray textBindings;
		textBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		textDescriptorLayout = device->createDescriptorSetLayout( name + "Text"
			, castor::move( textBindings ) );
		textDescriptorPool = textDescriptorLayout->createPool( name + "Text"
			, MaxOverlaysPerBuffer );
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getPanelNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto key = ovrlrend::makeKey( pass, renderPass );
		auto it = m_mapPanelNodes.find( key );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_panelPipelines, false, false, false );
			it = m_mapPanelNodes.try_emplace( key, pipeline, pass ).first;
		}

		return it->second;
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getBorderNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass )
	{
		auto key = ovrlrend::makeKey( pass, renderPass );
		auto it = m_mapBorderNodes.find( key );

		if ( it == m_mapBorderNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_borderPipelines, true, false, false );
			it = m_mapBorderNodes.try_emplace( key, pipeline, pass ).first;
		}

		return it->second;
	}

	OverlayDrawNode & OverlayRenderer::OverlaysDrawData::getTextNode( RenderDevice const & device
		, VkRenderPass renderPass
		, Pass const & pass
		, bool sdfFont )
	{
		auto key = ovrlrend::makeKey( pass, renderPass );
		auto it = m_mapTextNodes.find( key );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, renderPass, pass, m_textPipelines, false, true, sdfFont );
			it = m_mapTextNodes.try_emplace( key, pipeline, pass ).first;
		}

		return it->second;
	}

	ashes::DescriptorSet const & OverlayRenderer::OverlaysDrawData::createTextDescriptorSet( FontTexture & fontTexture)
	{
		auto [it, res] = textDescriptorSets.try_emplace( &fontTexture );
		auto & descriptorConnection = it->second;

		if ( res || !descriptorConnection.descriptorSet )
		{
			auto result = textDescriptorPool->createDescriptorSet( "TextOverlays_" + castor::string::toMbString( intptr_t( &fontTexture ) ) );
			result->createBinding( textDescriptorLayout->getBinding( 0u )
				, fontTexture.getTexture()->getDefaultSampledView()
				, fontTexture.getSampler()->getSampler() );
			result->update();
			descriptorConnection.descriptorSet = castor::move( result );
			descriptorConnection.connection = fontTexture.onResourceChanged.connect( [this, &descriptorConnection, &fontTexture]( DoubleBufferedTextureLayout const & )
				{
					retired.emplace_back( castor::move( descriptorConnection.descriptorSet ) );
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
		timerBlock = castor::make_unique< crg::FramePassTimerBlock >( timer.start() );
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
		, castor::UnorderedMap< size_t, OverlayDrawPipeline > & pipelines
		, bool borderOverlay
		, bool textOverlay
		, bool sdfFont )
	{
		// Remove unwanted flags
		auto const & passComponents = device.renderSystem.getEngine()->getPassComponentsRegister();
		auto textures = passComponents.filterTextureFlags( ComponentModeFlag::eColour | ComponentModeFlag::eOpacity
			, pass.getTexturesMask() );
		auto key = ovrlrend::makeKey( passComponents, textures, renderPass, borderOverlay, textOverlay, sdfFont );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.try_emplace( key
				, doCreatePipeline( device
					, renderPass
					, doCreateOverlayProgram( device, textures, textOverlay, sdfFont )
					, textures
					, borderOverlay
					, textOverlay
					, sdfFont ) ).first;
		}

		return it->second;
	}

	OverlayDrawPipeline OverlayRenderer::OverlaysDrawData::doCreatePipeline( RenderDevice const & device
		, VkRenderPass renderPass
		, ashes::PipelineShaderStageCreateInfoArray program
		, TextureCombine const & texturesFlags
		, bool borderOverlay
		, bool textOverlay
		, bool sdfFont )
	{
		auto const & engine = *device.renderSystem.getEngine();
		auto const & passComponents = engine.getPassComponentsRegister();
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
			, castor::move( attachments ) };
		castor::MbString name = "Overlays";

		name += "-" + castor::toUtf8( ovrlrend::makeName( passComponents, texturesFlags ) );
		ashes::DescriptorSetLayoutCRefArray descriptorLayouts;
		descriptorLayouts.push_back( *m_commonData.baseDescriptorLayout );
		descriptorLayouts.push_back( *engine.getTextureUnitCache().getDescriptorLayout() );

		if ( textOverlay )
		{
			if ( sdfFont )
			{
				name = "SdfText" + name;
			}
			else
			{
				name = "Text" + name;
			}

			descriptorLayouts.push_back( *textDescriptorLayout );
		}
		else if ( borderOverlay )
		{
			name = "Border" + name;
		}

		auto pipelineLayout = device->createPipelineLayout( name
			, descriptorLayouts
			, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, uint32_t( sizeof( OverlayDrawConstants ) ) } } );
		auto pipeline = device->createPipeline( name
			, { 0u
				, castor::move( program )
				, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
				, ashes::nullopt
				, ashes::PipelineViewportStateCreateInfo{}
				, ashes::PipelineRasterizationStateCreateInfo{}
				, ashes::PipelineMultisampleStateCreateInfo{}
				, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE }
				, castor::move( blState )
				, ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } }
				, *pipelineLayout
				, renderPass } );
		return OverlayDrawPipeline{ castor::move( pipelineLayout )
			, castor::move( pipeline ) };
	}

	ashes::PipelineShaderStageCreateInfoArray OverlayRenderer::OverlaysDrawData::doCreateOverlayProgram( RenderDevice const & device
		, TextureCombine const & texturesFlags
		, bool textOverlay
		, bool sdfFont )const
	{
		auto & engine = *device.renderSystem.getEngine();
		bool hasTexture = texturesFlags.configCount != 0u;
		ProgramModule programModule{ cuT( "Overlay" ) };
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

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
			C3D_Camera( writer
				, OverlayBindingId::eCamera
				, 0u );
			C3D_HdrConfig( writer
				, OverlayBindingId::eHdrConfig
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
			C3D_FontEx( writer
				, OverlayBindingId::eOverlaysFont
				, 0u
				, textOverlay );
			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
				, 0u
				, 1u
				, hasTexture ) );

			if ( sdfFont )
			{
				writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapText"
					, 0u
					, 2u
					, textOverlay );
			}
			else
			{
				writer.declCombinedImg< FImg2DR32 >( "c3d_mapText"
					, 0u
					, 2u
					, textOverlay );
			}

			auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0 );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
			auto pipelineBaseIndex = pcb.declMember< sdw::UInt >( "pipelineBaseIndex" );
			auto drawID = pcb.declMember< sdw::Int >( "drawID" );
			pcb.end();

			// Vertex shader
			writer.implementEntryPointT< sdw::VoidT, shader::OverlaySurfaceT >( sdw::VertexIn{ writer }
				, sdw::VertexOutT< shader::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, [&]( sdw::VertexIn const & in
					, sdw::VertexOutT< shader::OverlaySurfaceT > out )
				{
					auto overlaySubID = writer.declLocale( "overlaySubID"
						, pipelineBaseIndex + ( writer.cast< sdw::UInt >( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) ) );
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

			auto median = []( sdw::Float const & r, sdw::Float const & g, sdw::Float const & b )
				{
					return max( min( r, g ), min( max( r, g ), b ) );
				};

			auto screenPxRange = [&writer, &c3d_fontData]( sdw::Vec2 const & fontUV, sdw::IVec2 const & imgSize )
				{
					auto unitRange = writer.declLocale( "unitRange"
						, vec2( c3d_fontData.pixelRange() ) / vec2( imgSize ) );
					auto screenTexSize = writer.declLocale( "screenTexSize"
						, vec2( 1.0_f ) / fwidth( fontUV ) );
					return max( 0.5_f * dot( unitRange, screenTexSize ), 1.0_f );
				};

			// Pixel shader
			writer.implementEntryPointT< shader::OverlaySurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, sdw::FragmentOut{ writer }
					, [&]( sdw::FragmentInT< shader::OverlaySurfaceT > const & in
						, sdw::FragmentOut const & )
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( in.materialId ) );

					if ( textOverlay )
					{
						if ( sdfFont )
						{
							auto c3d_mapText = writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapText" );
							auto msd = writer.declLocale( "msd"
								, c3d_mapText.lod( in.fontUV, 0.0_f ) );
							auto sd = writer.declLocale( "sd"
								, median( msd.r(), msd.g(), msd.b() ) );
							auto screenPxDistance = writer.declLocale( "screenPxDistance"
								, screenPxRange( in.fontUV, c3d_mapText.getSize( 0_i ) ) * ( sd - 0.5_f ) );
							material.opacity *= clamp( screenPxDistance + 0.5_f, 0.0_f, 1.0_f );
						}
						else
						{
							auto c3d_mapText = writer.getVariable< sdw::CombinedImage2DR32 >( "c3d_mapText" );
							material.opacity *= c3d_mapText.lod( in.fontUV, 0.0_f );
						}
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

					if ( m_isHdr )
					{
						outComponents.colour = c3d_hdrConfigData.removeGamma( outComponents.colour );
					}

					outColour = vec4( outComponents.colour, outComponents.opacity );
				} );

			programModule.shader = castor::make_unique< sdw::Shader >( castor::move( writer.getShader() ) );
		}
		return makeProgramStates( device, programModule );
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderDevice const & device
		, Texture const & target
		, HdrConfigUbo const & hdrConfigUbo
		, crg::FramePassTimer & timer
		, VkCommandBufferLevel level )
		: OwnedBy< RenderSystem >( device.renderSystem )
		, m_target{ target }
		, m_timer{ timer }
		, m_size{ makeSize( m_target.getExtent() ) }
		, m_common{ device, hdrConfigUbo }
		, m_draw{ device, level, m_common, castor::isFloatingPoint( m_target.getFormat() ) }
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

	void OverlayRenderer::update( GpuUpdater & updater )
	{
		if ( auto timerBlock = castor::make_unique< crg::FramePassTimerBlock >( m_timer.start() ) )
		{
			if ( auto size = updater.camera->getSize();
				m_size != size )
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

	castor::Pair< OverlayDrawNode *, OverlayPipelineData * > OverlayRenderer::doGetDrawNodeData( RenderDevice const & device
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
				node = &m_draw.getTextNode( device, renderPass, pass, texture->getFont()->isSDF() );
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
