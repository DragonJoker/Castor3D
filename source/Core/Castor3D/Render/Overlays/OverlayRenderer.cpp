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
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

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

		template< ast::var::Flag FlagT >
		struct OverlaySurfaceT
			: public sdw::StructInstance
		{
			OverlaySurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstance{ writer, std::move( expr ), enabled }
				, position{ this->getMember< sdw::Vec2 >( "position", true ) }
				, texture{ this->getMember< sdw::Vec2 >( "texture", true ) }
				, text{ this->getMember< sdw::Vec2 >( "text", true ) }
				, materialId{ this->getMember< sdw::UInt >( "materialId", true ) }
			{
			}

			SDW_DeclStructInstance( , OverlaySurfaceT );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, bool hasPosition
				, bool isTextOverlay
				, bool hasTextures
				, bool isTransition )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC
					, "C3D_OverlaySurface" );

				if ( result->empty() )
				{
					result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, hasPosition );
					result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, hasTextures );
					result->declMember( "text", ast::type::Kind::eVec2F, ast::type::NotArray, isTextOverlay );
					result->declMember( "materialId", ast::type::Kind::eUInt, ast::type::NotArray, isTransition );
				}

				return result;
			}

			static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
				, bool hasPosition
				, bool isTextOverlay
				, bool hasTextures
				, bool isTransition )
			{
				auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
					, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Out" }
						: std::string{ "In" } ) + "OverlaySurface"
					, FlagT );

				if ( result->empty() )
				{
					result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, 0u, hasPosition );
					result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, 1u, hasTextures );
					result->declMember( "text", ast::type::Kind::eVec2F, ast::type::NotArray, 2u, isTextOverlay );
					result->declMember( "materialId", ast::type::Kind::eUInt, ast::type::NotArray, 3u, isTransition );
				}

				return result;
			}

			sdw::Vec2 position;
			sdw::Vec2 texture;
			sdw::Vec2 text;
			sdw::UInt materialId;
		};
		using OverlaySurface = OverlaySurfaceT< sdw::var::Flag::eNone >;

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
				, OverlaySurfaceT< FlagT > const & surface )
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
				, OverlaySurfaceT< FlagT > const & surface
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
				, OverlaySurfaceT< FlagT > const & surface )
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
				, OverlaySurfaceT< FlagT > const & surface )
			{
				sdw::expr::ExprList initializers;
				auto type = OverlayBlendComponents::makeType( writer.getTypesCache(), materials, material, surface, initializers );
				return sdw::makeAggrInit( type, std::move( initializers ) );
			}
		};

		ashes::DescriptorSetLayoutPtr createBaseDescriptorLayout( RenderDevice const & device )
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
			, MaxOverlayPanelsPerBuffer ) }
		, m_borderVertexBuffer{ std::make_unique< BorderPanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "BorderPanelOverlays"
			, device
			, m_cameraUbo
			, *m_baseDescriptorLayout
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxOverlayPanelsPerBuffer ) }
		, m_textVertexBuffer{ std::make_unique< TextVertexBufferPool >( *getRenderSystem()->getEngine()
			, "TextOverlays"
			, device
			, m_cameraUbo
			, *m_baseDescriptorLayout
			, m_noTexTextDeclaration
			, m_texTextDeclaration
			, MaxOverlayPanelsPerBuffer ) }
		, m_size{ makeSize( m_target.getExtent() ) }
	{
		std::string name = "Overlays";

		ashes::VkDescriptorSetLayoutBindingArray textBindings;
		textBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_textDescriptorLayout = device->createDescriptorSetLayout( name + "Text"
			, std::move( textBindings ) );
		m_textDescriptorPool = m_textDescriptorLayout->createPool( MaxOverlayPanelsPerBuffer );

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
			descriptorConnection.connection = fontTexture.onChanged.connect( [this, &descriptorConnection]( DoubleBufferedTextureLayout const & )
				{
					m_retired.emplace_back( std::move( descriptorConnection.descriptorSet ) );
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
				, *vertexLayout
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

			sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
			auto overlayID = pcb.declMember< sdw::UInt >( "overlayID" );
			pcb.end();

			writer.implementMainT< ovrlrend::OverlaySurfaceT, ovrlrend::OverlaySurfaceT >( VertexInT< ovrlrend::OverlaySurfaceT >{ writer, true, textOverlay, hasTexture, false }
				, VertexOutT< ovrlrend::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, [&]( VertexInT< ovrlrend::OverlaySurfaceT > in
					, VertexOutT< ovrlrend::OverlaySurfaceT > out )
				{
					out.texture = in.texture;
					out.text = in.text;
					auto overlaysData = writer.declLocale( "overlaysData"
						, c3d_overlaysData[overlayID] );
					out.vtx.position = c3d_cameraData.viewToProj( vec4( vec2( c3d_cameraData.renderSize() ) * overlaysData.modelToView( in.position )
							, 0.0_f
							, 1.0_f ) );
					out.position = out.vtx.position.xy();
					out.materialId = overlaysData.materialId();
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

			writer.implementMainT< ovrlrend::OverlaySurfaceT, VoidT >( FragmentInT< ovrlrend::OverlaySurfaceT >{ writer, false, textOverlay, hasTexture, true }
				, FragmentOut{ writer }
					, [&]( FragmentInT< ovrlrend::OverlaySurfaceT > in
						, FragmentOut out )
				{
					auto material = writer.declLocale( "material"
						, materials.getMaterial( in.materialId ) );

					if ( textOverlay )
					{
						material.opacity *= c3d_mapText.sample( in.text, 0.0_f );
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
